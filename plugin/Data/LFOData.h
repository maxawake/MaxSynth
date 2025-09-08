/*
  ==============================================================================

    ADSRData.h
    Created: 29 Aug 2025 10:29:05pm
    Author:  max

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class ADSRData : public juce::ADSR
{
public:
    ADSRData();
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void updateEnvelope(const float attack, const float decay, const float sustain, const float release);


private:
    juce::ADSR::Parameters envelopeParams; // Parameters for the envelope
};
/*
  ==============================================================================

    ADSRData.h
    Created: 29 Aug 2025 10:29:05pm
    Author:  max

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class LFOData
{
public:
    LFOData();
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void updateLFO(const float frequency, const float amount);

private:
    float lfoFrequency;
    float lfoAmount;
    juce::dsp::Oscillator<float> lfoOscillator { [](float x) { return std::sin(x); } }; // LFO oscillator
};
