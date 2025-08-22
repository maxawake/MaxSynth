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

    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int newValue) override;
    void controllerMoved (int controllerNumber, int newValue) override;
    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    void prepareToPlay (double sampleRate, int samplesPerBlock, int numChannels);

private:
    float freq = 440.0f; // Frequency of the note
    float volume = 0.5f; // Volume of the note

    juce::ADSR envelope; // Envelope for the note
    juce::ADSR::Parameters envelopeParams; // Parameters for the envelope

    juce::dsp::Oscillator<float> oscillator { [](float x) { return std::sinf(x); } }; // Oscillator for generating the sine wave
    juce::dsp::Gain<float> gain; // Gain for volume control
};
