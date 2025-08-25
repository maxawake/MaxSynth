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
}

SynthVoice::~SynthVoice()
{
    // Destructor logic if needed
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    // Start the note with the given MIDI note number and velocity
    freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    oscillator.setFrequency(freq);
    
    // Set gain based on velocity to prevent clipping
    gain.setGainLinear(velocity * 0.3f); // Scale down to prevent clipping
    
    envelope.noteOn();
}

void SynthVoice::stopNote (float velocity, bool allowTailOff)
{
    // Stop the note with the given velocity
    envelope.noteOff();
    
    // If not allowing tail off, clear the voice immediately
    if (!allowTailOff)
        clearCurrentNote();
}

void SynthVoice::pitchWheelMoved (int newValue)
{
    // Handle pitch wheel changes
}

void SynthVoice::controllerMoved (int controllerNumber, int newValue)
{
    // Handle MIDI controller changes
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{   
    // Check if the voice should be playing
    if (!isVoiceActive())
        return;
        
    // Create a buffer for just the samples we need to render
    juce::AudioBuffer<float> synthBuffer(outputBuffer.getNumChannels(), numSamples);
    synthBuffer.clear();
    
    // Create audio block for processing
    juce::dsp::AudioBlock<float> block(synthBuffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    
    // Generate oscillator output
    oscillator.process(context);
    
    // Apply gain
    gain.process(context);
    
    // Apply envelope to the buffer
    envelope.applyEnvelopeToBuffer(synthBuffer, 0, numSamples);
    
    // Add the synthesized audio to the output buffer
    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
    {
        outputBuffer.addFrom(channel, startSample, synthBuffer, channel, 0, numSamples);
    }
    
    // Clear the voice if the envelope has finished
    if (!envelope.isActive())
        clearCurrentNote();
}

void SynthVoice::prepareToPlay (double sampleRate, int samplesPerBlock, int numChannels)
{
    // Prepare the voice for playback
    envelope.setSampleRate(sampleRate);
    
    // Set more reasonable envelope parameters
    envelopeParams.attack = 0.1f;   // Quick attack (10ms)
    envelopeParams.decay = 0.2f;     // Moderate decay (200ms)
    envelopeParams.sustain = 0.7f;   // Good sustain level
    envelopeParams.release = 0.3f;   // Moderate release (300ms)

    envelope.setParameters(envelopeParams);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = numChannels;

    // Prepare the DSP components
    oscillator.prepare(spec);
    oscillator.setFrequency(freq);
    gain.prepare(spec);
    gain.setGainLinear(volume); // Set a safe default volume
}