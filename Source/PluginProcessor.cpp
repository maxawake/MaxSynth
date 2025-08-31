/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SynthVoice.h"
#include "SynthSound.h"

//==============================================================================
MaxSynthAudioProcessor::MaxSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), 
#endif
    apvts(*this, nullptr, "Parameters", createParameters())                   
{
    synth.addSound (new SynthSound());
    
    // Add multiple voices for polyphony (typically 8-16 voices)
    for (int i = 0; i < 10; ++i)
        synth.addVoice (new SynthVoice());
}

MaxSynthAudioProcessor::~MaxSynthAudioProcessor()
{
}

//==============================================================================
const juce::String MaxSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MaxSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MaxSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MaxSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MaxSynthAudioProcessor::getTailLengthSeconds() const
{
    return 2.0;
}

int MaxSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MaxSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MaxSynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MaxSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void MaxSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MaxSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{   
    synth.setCurrentPlaybackSampleRate(sampleRate);
    midiCollector.reset(sampleRate);  // Add this line

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }
}

void MaxSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MaxSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MaxSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    midiCollector.removeNextBlockOfMessages(midiMessages, buffer.getNumSamples());

    // Debug MIDI messages
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();
        std::cout << "MIDI: " << message.getDescription() << std::endl;
    }

    for (auto i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            auto& attack = *apvts.getRawParameterValue("attack");
            auto& decay = *apvts.getRawParameterValue("decay");
            auto& sustain = *apvts.getRawParameterValue("sustain");
            auto& release = *apvts.getRawParameterValue("release");

            auto& filterCutoff = *apvts.getRawParameterValue("filterCutoff");
            auto& filterResonance = *apvts.getRawParameterValue("filterResonance");
            auto& filterMode = *apvts.getRawParameterValue("filterMode");
            auto& waveform = *apvts.getRawParameterValue("waveform");

            // Get filter envelope and LFO parameters
            auto& filterAttack = *apvts.getRawParameterValue("filterAttack");
            auto& filterDecay = *apvts.getRawParameterValue("filterDecay");
            auto& filterSustain = *apvts.getRawParameterValue("filterSustain");
            auto& filterRelease = *apvts.getRawParameterValue("filterRelease");
            auto& lfoFreq = *apvts.getRawParameterValue("lfoFreq");
            auto& lfoAmount = *apvts.getRawParameterValue("lfoAmount");

            // filterCutoff is already in Hz due to NormalisableRange with skew factor
            // filterResonance is linear and doesn't need conversion

            voice->updateEnvelope(attack, decay, sustain, release);
            voice->updateFilter(filterCutoff, filterResonance, static_cast<int>(filterMode));
            voice->updateFilterEnvelope(filterAttack, filterDecay, filterSustain, filterRelease);
            voice->updateLFO(lfoFreq, lfoAmount);
            voice->updateWaveform(static_cast<int>(waveform));
        }
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool MaxSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MaxSynthAudioProcessor::createEditor()
{
    return new MaxSynthAudioProcessorEditor (*this);
}

//==============================================================================
void MaxSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MaxSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MaxSynthAudioProcessor();
}


juce::AudioProcessorValueTreeState::ParameterLayout MaxSynthAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    // More practical ranges: Attack, Decay, Release in seconds (0.01 to 5.0 seconds)
    // Sustain as a level (0.0 to 1.0)
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("attack", "Attack", 0.01f, 5.0f, 0.1f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("decay", "Decay", 0.01f, 5.0f, 0.2f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("sustain", "Sustain", 0.0f, 1.0f, 0.7f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("release", "Release", 0.01f, 5.0f, 0.3f));

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("filterCutoff", "Filter Cutoff", 
        juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.3f), 1000.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("filterResonance", "Filter Resonance", 0.0f, 10.0f, 0.0f));

    // Filter mode parameter (0=LPF12, 1=LPF24, 2=HPF12, 3=HPF24, 4=BPF12, 5=BPF24)
    parameters.push_back(std::make_unique<juce::AudioParameterChoice>("filterMode", "Filter Mode", 
        juce::StringArray{"LPF 12dB", "LPF 24dB", "HPF 12dB", "HPF 24dB", "BPF 12dB", "BPF 24dB"}, 1));

    // Waveform parameter (0=Sine, 1=Square, 2=Saw, 3=Triangle, 4=Noise)
    parameters.push_back(std::make_unique<juce::AudioParameterChoice>("waveform", "Waveform", 
        juce::StringArray{"Sine", "Square", "Saw", "Triangle", "Noise"}, 0));

    // Filter ADSR parameters
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("filterAttack", "Filter Attack", 0.01f, 5.0f, 0.1f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("filterDecay", "Filter Decay", 0.01f, 5.0f, 0.2f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("filterSustain", "Filter Sustain", 0.0f, 1.0f, 0.7f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("filterRelease", "Filter Release", 0.01f, 5.0f, 0.3f));

    // LFO parameters
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("lfoFreq", "LFO Frequency", 0.1f, 20.0f, 2.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("lfoAmount", "LFO Amount", 0.0f, 1.0f, 0.0f));

    return { parameters.begin(), parameters.end() };
}
