/*
  ==============================================================================

    osc.h
    Created: 21 Aug 2025 6:08:43pm
    Author:  max

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Synth.h"
#include <juce_dsp/juce_dsp.h>

class Oscillator : public juce::dsp::Oscillator<float>
{
public:
  Oscillator();
  ~Oscillator();

}
