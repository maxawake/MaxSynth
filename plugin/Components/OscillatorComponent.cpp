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
    waveformAttachment2 = std::make_unique<comboBoxAttachment>(apvts, "waveform2", waveformSelector2);
    waveformAttachment3 = std::make_unique<comboBoxAttachment>(apvts, "waveform3", waveformSelector3);

    // Set up waveform selector
    setLabels(waveformSelector);
    setLabels(waveformSelector2);
    setLabels(waveformSelector3);
}

OscillatorComponent::~OscillatorComponent()
{
}

void OscillatorComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void OscillatorComponent::resized()
{
    auto area = getLocalBounds();
    waveformSelector.setBounds(area.removeFromTop(30).reduced(6));
    area.removeFromTop(30);
    waveformSelector2.setBounds(area.removeFromTop(30).reduced(6));
    area.removeFromTop(30);
    waveformSelector3.setBounds(area.removeFromTop(30).reduced(6));
}

void OscillatorComponent::setLabels(juce::ComboBox& box)
{
    box.addItem("Sine", 1);
    box.addItem("Square", 2);
    box.addItem("Saw", 3);
    box.addItem("Triangle", 4);
    box.addItem("Noise", 5);
    box.setSelectedId(1); // Default to Sine
    addAndMakeVisible(box);
}