/*
  ==============================================================================

    SynthVoice.h
    Created: 22 Aug 2025 5:16:55pm
    Author:  max

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthSound.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice();
    ~SynthVoice();
    void prepareToPlay (double sampleRate, int samplesPerBlock, int numChannels);
    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int newValue) override;
    void controllerMoved (int controllerNumber, int newValue) override;
    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void updateEnvelope(const float attack, const float decay, const float sustain, const float release);

private:
    float freq = 440.0f; // Frequency of the note
    float volume = 1.0f; // Volume of the note

    juce::ADSR envelope; // Envelope for the note
    juce::ADSR::Parameters envelopeParams; // Parameters for the envelope

    juce::dsp::Oscillator<float> oscillator { [](float x) { return x / juce::MathConstants<float>::pi; } }; // Oscillator for generating the sine wave
    juce::dsp::Gain<float> gain; // Gain for volume control
};
