/*
  ==============================================================================

    Synth.h
    Created: 21 Aug 2025 6:02:45pm
    Author:  max

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Synth : public juce::Synthesiser
{
public:
    Synth();
    ~Synth();

    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Synth)
};
