/*
  ==============================================================================

    SynthSound.h
    Created: 22 Aug 2025 5:16:43pm
    Author:  max

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound
{
public:
    SynthSound() {}

    bool appliesToNote (int midiNoteNumber) override
    {
        return true;
    }

    bool appliesToChannel (int midiChannel) override
    {
        return true;
    }
};
