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
    updateWaveform(0, 1);
}

SynthVoice::~SynthVoice()
{
    // Destructor logic if needed
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = numChannels;

    // Prepare the DSP components
    oscillator1.prepare(spec);
    oscillator1.setFrequency(freq);
    oscillator2.prepare(spec);
    oscillator2.setFrequency(freq);
    oscillator3.prepare(spec);
    oscillator3.setFrequency(freq);

    gain.prepare(spec);
    gain.setGainLinear(volume);

    adsr.setSampleRate(sampleRate);
    filterADSR.setSampleRate(sampleRate);

    filter.prepare(spec);
    filter.setMode(juce::dsp::LadderFilterMode::LPF12);
    filter.setCutoffFrequencyHz(baseCutoff);
    filter.setResonance(baseResonance);
    filter.setEnabled(true);
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound *sound)
{
    return dynamic_cast<SynthSound *>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
   
    // Reset all envelopes completely
    adsr.reset();
    filterADSR.reset();
    
    // Start the note with the given MIDI note number and velocity
    freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    
    // Reset oscillator phases to avoid frequency sweeps
    oscillator1.reset();
    oscillator2.reset();
    oscillator3.reset();
    
    // Set frequencies
    oscillator1.setFrequency(freq, true);
    oscillator2.setFrequency(freq, true);
    oscillator3.setFrequency(freq, true);

    // Set gain based on velocity to prevent clipping
    gain.setGainLinear(velocity * 0.3f);
    
    // Reset filter state to avoid frequency sweeps
    filter.reset();
    
    // Initialize filter with base cutoff to ensure consistent starting point
    filter.setCutoffFrequencyHz(baseCutoff);

    // NOW start the envelopes (after everything is reset)
    adsr.noteOn();
    filterADSR.noteOn(); // Start filter envelope
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    // Stop the note with the given velocity
    adsr.noteOff();
    filterADSR.noteOff();
    
    // If not allowing tail off, force immediate stop
    if (!allowTailOff)
    {
        adsr.reset();
        filterADSR.reset();
        clearCurrentNote(); // Mark voice as not playing any note
    }
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
    oscillator1.process(synthContext);
    oscillator2.process(synthContext);
    oscillator3.process(synthContext);

    // Apply gain
    gain.process(synthContext);

    // TODO Make this block its own function
    // Use global LFO data if available, otherwise fall back to local generation
    const float* lfoData = globalLFOData;

    // Process filter in small chunks to balance performance and modulation smoothness
    const int chunkSize = 32; // Process 32 samples at a time
    
    for (int startPos = 0; startPos < numSamples; startPos += chunkSize)
    {
        int samplesToProcess = juce::jmin(chunkSize, numSamples - startPos);
        
        // Get filter envelope value for this chunk (if enabled)
        float filterEnvValue = filterADSREnabled ? filterADSR.getNextSample() : 1.0f;
        
        // Get average LFO value for this chunk
        float avgLfoValue = 0.0f;
        for (int i = 0; i < samplesToProcess; ++i)
        {
            avgLfoValue += lfoData[startPos + i];
        }
        avgLfoValue /= samplesToProcess;
        
        // Calculate modulated cutoff frequency (only apply envelope if enabled)
        float modulatedCutoff = filterADSREnabled ? 
            baseCutoff + filterEnvValue * adsrFilterAmount * (20000.0f - baseCutoff) :
            baseCutoff;

        // Apply LFO modulation
        modulatedCutoff *= (1.0f + avgLfoValue * lfoAmount * 4.0f); // Increased LFO amount for more audible effect

        // Clamp to reasonable range
        modulatedCutoff = juce::jlimit(20.0f, 20000.0f, modulatedCutoff);

        // Set filter cutoff for this chunk
        filter.setCutoffFrequencyHz(modulatedCutoff);
        
        // Process this chunk through the filter
        auto chunkBlock = synthBlock.getSubBlock(startPos, samplesToProcess);
        juce::dsp::ProcessContextReplacing<float> chunkContext(chunkBlock);
        filter.process(chunkContext);
    }
    // TODO until here

    // Apply ADSR envelope
    adsr.applyEnvelopeToBuffer(synthBuffer, 0, numSamples);

    // Add the synthesized audio to the output buffer with soft limiting
    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
    {
        auto* outputData = outputBuffer.getWritePointer(channel, startSample);
        auto* synthData = synthBuffer.getReadPointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float newSample = outputData[sample] + synthData[sample];
            
            // Soft limiting to prevent harsh clipping 
            // TODO Use this outside of voice
            if (newSample > 0.95f)
                newSample = 0.95f + 0.05f * std::tanh((newSample - 0.95f) / 0.05f);
            else if (newSample < -0.95f)
                newSample = -0.95f + 0.05f * std::tanh((newSample + 0.95f) / 0.05f);
                
            outputData[sample] = newSample;
        }
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

void SynthVoice::updateFilterEnvelope(const float attack, const float decay, const float sustain, const float release, const bool enabled, const float amount)
{
    filterADSR.updateEnvelope(attack, decay, sustain, release);
    filterADSREnabled = enabled;
    adsrFilterAmount = amount;
}

void SynthVoice::setGlobalLFOData(const float* lfoData, const float amount)
{
    globalLFOData = lfoData;
    lfoAmount = amount;
}

void SynthVoice::updateWaveform(const int waveformType, const int oscIndex)
{
    currentWaveform = waveformType;
    auto osc = (oscIndex == 1) ? &oscillator1 : (oscIndex == 2) ? &oscillator2 : &oscillator3;

    switch (waveformType)
    {
    case 0: // Sine
        osc->initialise([](float x)
                              { return std::sin(x); });
        break;

    case 1: // Square
        osc->initialise([](float x)
                              { return x < 0.0f ? -1.0f : 1.0f; });
        break;

    case 2: // Sawtooth
        osc->initialise([](float x)
                              { return x / juce::MathConstants<float>::pi; });
        break;

    case 3: // Triangle
        osc->initialise([](float x)
                              { return 2.0f * std::abs(2.0f * (x / juce::MathConstants<float>::twoPi - std::floor(x / juce::MathConstants<float>::twoPi + 0.5f))) - 1.0f; });
        break;

    case 4: // Noise
        osc->initialise([this](float x)
                              { 
                juce::ignoreUnused(x);
                return random.nextFloat() * 2.0f - 1.0f; });
        break;

    default: // Default to sine
        osc->initialise([](float x)
                              { return std::sin(x); });
        break;
    }
}