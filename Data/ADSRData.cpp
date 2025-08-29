/*
  ==============================================================================

    ADSRData.cpp
    Created: 29 Aug 2025 10:29:05pm
    Author:  max

  ==============================================================================
*/

#include "ADSRData.h"

ADSRData::ADSRData()
{
    // Initialize with default ADSR values
    setParameters(juce::ADSR::Parameters{
        0.1f,  // attack
        0.2f,  // decay  
        0.7f,  // sustain
        0.3f   // release
    });
}

void ADSRData::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
        // Initialize with default envelope parameters
    envelopeParams.attack = 0.1f;
    envelopeParams.decay = 0.2f;
    envelopeParams.sustain = 0.7f;
    envelopeParams.release = 0.3f;
    setParameters(envelopeParams);
}

void ADSRData::updateEnvelope(const float attack, const float decay, const float sustain, const float release)
{
    // Set more reasonable envelope parameters
    envelopeParams.attack = attack;
    envelopeParams.decay = decay;
    envelopeParams.sustain = sustain;
    envelopeParams.release = release;

    // Update the envelope parameters
    setParameters(envelopeParams);
}