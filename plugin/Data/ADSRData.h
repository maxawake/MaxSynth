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
