/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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
                       )
#endif
{
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
    return 0.0;
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
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

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float sampleRate = 48000.0f; // Default sample rate, can be set dynamically
    float dt = 1.0 / sampleRate;
    float decayTime = 0.1f; // Example decay time
    float volume = 0.5f; // Example volume
    std::cout << "i got caled" << this->t << std::endl;


    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        float localTime = t; // Use local copy for this channel

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // Calculate phase from time
            float currentPhase = 2.0f * juce::MathConstants<float>::pi * freq * localTime;
            
            // Apply exponential decay
            float decayFactor = std::exp(-localTime / decayTime);
            float currentVolume = volume * decayFactor;
            
            // Generate sine wave with decay
            channelData[sample] = currentVolume * std::sin(currentPhase);
            
            // Increment time
            localTime += dt;
        }
    }
    
    // Update time for next buffer
    t += dt * buffer.getNumSamples();
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
