#include "LFOData.h"

LFOData::LFOData()
    : lfoFrequency(1.0f), lfoAmount(0.5f)
{
}

void LFOData::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(numChannels);
    lfoOscillator.prepare(spec);
    lfoOscillator.setFrequency(lfoFrequency);
}

void LFOData::updateLFO(const float frequency, const float amount)
{
    lfoFrequency = frequency;
    lfoAmount = amount;
}
