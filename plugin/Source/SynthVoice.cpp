/*
  ==============================================================================

    SynthVoice.cpp
    Created: 22 Aug 2025 5:16:55pm
    Author:  max

  ==============================================================================
*/

#include "SynthVoice.h"

SynthVoice::SynthVoice()
{
    // Initialize the oscillator and gain
    gain.setGainLinear(volume);

    // Initialize with sine wave
    updateWaveform(0);
}

SynthVoice::~SynthVoice()
{
    // Destructor logic if needed
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
    // Prepare the voice for playback
    adsr.setSampleRate(sampleRate);
    filterADSR.setSampleRate(sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = numChannels;

    // Prepare the DSP components
    oscillator.prepare(spec);
    oscillator.setFrequency(freq);

    lfoOscillator.prepare(spec); // Use the same sample rate as the main oscillator
    lfoOscillator.setFrequency(lfoFrequency);

    gain.prepare(spec);
    gain.setGainLinear(volume); // Set a safe default volume

    filter.prepare(spec);
    filter.setMode(juce::dsp::LadderFilterMode::LPF12); // Set filter mode
    filter.setCutoffFrequencyHz(baseCutoff);            // Set a high default cutoff
    filter.setResonance(baseResonance);                 // Set a low default resonance
    filter.setEnabled(true);                            // Enable the filter
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound *sound)
{
    return dynamic_cast<SynthSound *>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    // Start the note with the given MIDI note number and velocity
    freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    oscillator.setFrequency(freq);

    // Set gain based on velocity to prevent clipping
    gain.setGainLinear(velocity);

    adsr.noteOn();
    filterADSR.noteOn(); // Start filter envelope
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    // Stop the note with the given velocity
    adsr.noteOff();
    filterADSR.noteOff(); // Stop filter envelope

    // // If not allowing tail off, clear the voice immediately
    // if (!allowTailOff)
    //     clearCurrentNote();
}

void SynthVoice::pitchWheelMoved(int newValue)
{
    // Handle pitch wheel changes
}

void SynthVoice::controllerMoved(int controllerNumber, int newValue)
{
    // Handle MIDI controller changes
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    // Check if the voice should be playing
    if (!isVoiceActive())
        return;

    // Create a buffer for just the samples we need to render
    juce::AudioBuffer<float> synthBuffer(outputBuffer.getNumChannels(), numSamples);
    synthBuffer.clear();

    // Create audio block for processing
    juce::dsp::AudioBlock<float> synthBlock(synthBuffer);
    juce::dsp::ProcessContextReplacing<float> synthContext(synthBlock);

    // Generate oscillator output
    oscillator.process(synthContext);

    // Apply gain
    gain.process(synthContext);

    // Create a buffer for LFO processing
    juce::AudioBuffer<float> lfoBuffer(1, numSamples);
    lfoBuffer.clear();
    juce::dsp::AudioBlock<float> lfoBlock(lfoBuffer);
    juce::dsp::ProcessContextReplacing<float> lfoContext(lfoBlock);

    // Generate LFO samples for the entire block
    lfoOscillator.process(lfoContext);
    auto *lfoData = lfoBuffer.getReadPointer(0);

    // Process filter in small chunks to balance performance and modulation smoothness
    const int chunkSize = 32; // Process 32 samples at a time
    
    for (int startPos = 0; startPos < numSamples; startPos += chunkSize)
    {
        int samplesToProcess = juce::jmin(chunkSize, numSamples - startPos);
        
        // Get filter envelope value for this chunk
        float filterEnvValue = filterADSR.getNextSample();
        
        // Get average LFO value for this chunk
        float avgLfoValue = 0.0f;
        for (int i = 0; i < samplesToProcess; ++i)
        {
            avgLfoValue += lfoData[startPos + i];
        }
        avgLfoValue /= samplesToProcess;

        float filterEnvPercentage = 0.3f; // More conservative modulation amount
        
        // Calculate modulated cutoff frequency
        float modulatedCutoff = baseCutoff + filterEnvValue * filterEnvPercentage * (20000.0f - baseCutoff);

        // Apply LFO modulation
        modulatedCutoff *= (1.0f + avgLfoValue * lfoAmount * 4.0f); // Increased LFO amount for more audible effect

        // Clamp to reasonable range
        modulatedCutoff = juce::jlimit(50.0f, 18000.0f, modulatedCutoff);

        // Set filter cutoff for this chunk
        filter.setCutoffFrequencyHz(modulatedCutoff);
        
        // Process this chunk through the filter
        auto chunkBlock = synthBlock.getSubBlock(startPos, samplesToProcess);
        juce::dsp::ProcessContextReplacing<float> chunkContext(chunkBlock);
        filter.process(chunkContext);
    }

    // Apply ADSR envelope
    adsr.applyEnvelopeToBuffer(synthBuffer, 0, numSamples);

    // Add the synthesized audio to the output buffer
    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
    {
        outputBuffer.addFrom(channel, startSample, synthBuffer, channel, 0, numSamples);
    }

    // Clear the voice if the envelope has finished
    if (!adsr.isActive())
        clearCurrentNote();
}

void SynthVoice::updateEnvelope(const float attack, const float decay, const float sustain, const float release)
{
    adsr.updateEnvelope(attack, decay, sustain, release);
}

void SynthVoice::updateFilter(const float cutoff, const float resonance, const int mode)
{
    baseCutoff = cutoff;
    baseResonance = resonance;
    filterMode = mode;

    filter.setResonance(baseResonance);

    // Map the mode parameter to LadderFilterMode
    switch (mode)
    {
    case 0:
        filter.setMode(juce::dsp::LadderFilterMode::LPF12);
        break;
    case 1:
        filter.setMode(juce::dsp::LadderFilterMode::LPF24);
        break;
    case 2:
        filter.setMode(juce::dsp::LadderFilterMode::HPF12);
        break;
    case 3:
        filter.setMode(juce::dsp::LadderFilterMode::HPF24);
        break;
    case 4:
        filter.setMode(juce::dsp::LadderFilterMode::BPF12);
        break;
    case 5:
        filter.setMode(juce::dsp::LadderFilterMode::BPF24);
        break;
    default:
        filter.setMode(juce::dsp::LadderFilterMode::LPF24);
        break;
    }
}

void SynthVoice::updateFilterEnvelope(const float attack, const float decay, const float sustain, const float release)
{
    filterADSR.updateEnvelope(attack, decay, sustain, release);
}

void SynthVoice::updateLFO(const float frequency, const float amount)
{
    lfoFrequency = frequency;
    lfoAmount = amount;
    lfoOscillator.setFrequency(frequency);
}

void SynthVoice::updateWaveform(const int waveformType)
{
    currentWaveform = waveformType;

    switch (waveformType)
    {
    case 0: // Sine
        oscillator.initialise([](float x)
                              { return std::sin(x); });
        break;

    case 1: // Square
        oscillator.initialise([](float x)
                              { return x < 0.0f ? -1.0f : 1.0f; });
        break;

    case 2: // Sawtooth
        oscillator.initialise([](float x)
                              { return x / juce::MathConstants<float>::pi; });
        break;

    case 3: // Triangle
        oscillator.initialise([](float x)
                              { return 2.0f * std::abs(2.0f * (x / juce::MathConstants<float>::twoPi - std::floor(x / juce::MathConstants<float>::twoPi + 0.5f))) - 1.0f; });
        break;

    case 4: // Noise
        oscillator.initialise([this](float x)
                              { 
                juce::ignoreUnused(x);
                return random.nextFloat() * 2.0f - 1.0f; });
        break;

    default: // Default to sine
        oscillator.initialise([](float x)
                              { return std::sin(x); });
        break;
    }
}