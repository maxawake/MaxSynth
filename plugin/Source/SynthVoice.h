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
#include "../Data/ADSRData.h"

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
    void updateFilter(const float cutoff, const float resonance, const int mode);
    void updateFilterEnvelope(const float attack, const float decay, const float sustain, const float release);
    void updateLFO(const float frequency, const float amount);
    void updateWaveform(const int waveformType);

private:
    float freq = 440.0f; // Frequency of the note
    float volume = 1.0f; // Volume of the note
    int currentWaveform = 0; // Current waveform type (0=Sine, 1=Square, 2=Saw, 3=Triangle, 4=Noise)
    
    // Filter parameters
    float baseCutoff = 1000.0f; // Base cutoff frequency
    float baseResonance = 0.1f; // Base resonance
    int filterMode = 1; // Filter mode
    
    // LFO parameters
    float lfoFrequency = 2.0f;
    float lfoAmount = 0.2f; // Set a default amount that's actually audible
    
    ADSRData adsr; // ADSR envelope
    ADSRData filterADSR; // Filter envelope

    juce::dsp::Oscillator<float> oscillator { [](float x) { return std::sin(x); } }; // Oscillator - will be updated dynamically
    juce::dsp::Oscillator<float> lfoOscillator { [](float x) { return std::sin(x); } }; // LFO oscillator
    juce::dsp::Gain<float> gain; // Gain for volume control
    juce::dsp::LadderFilter<float> filter; // Ladder filter for sound shaping
    
    juce::Random random; // For noise generation
};
