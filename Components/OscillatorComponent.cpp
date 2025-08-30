/*
  ==============================================================================

    OscillatorComponent.cpp
    Created: 30 Aug 2025 11:19:44pm
    Author:  max

  ==============================================================================
*/

#include "../Components/OscillatorComponent.h"

OscillatorComponent::OscillatorComponent(juce::AudioProcessorValueTreeState& apvts)
{
    waveformAttachment = std::make_unique<comboBoxAttachment>(apvts, "waveform", waveformSelector);

    // Set up waveform selector
    waveformSelector.addItem("Sine", 1);
    waveformSelector.addItem("Square", 2);
    waveformSelector.addItem("Saw", 3);
    waveformSelector.addItem("Triangle", 4);
    waveformSelector.addItem("Noise", 5);
    waveformSelector.setSelectedId(1); // Default to Sine
    addAndMakeVisible(waveformSelector);
}

OscillatorComponent::~OscillatorComponent()
{
}

void OscillatorComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void OscillatorComponent::resized()
{
    auto area = getLocalBounds();
    waveformSelector.setBounds(area.removeFromTop(30).reduced(6));
}

