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

    // Set up buttons
    osc1ToggleButton.setButtonText("OSC 1");
    osc1ToggleButton.setClickingTogglesState(true); // Make it a toggle button
    osc1ToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "osc1Enabled", osc1ToggleButton);
    addAndMakeVisible(osc1ToggleButton);

    osc2ToggleButton.setButtonText("OSC 2");
    osc2ToggleButton.setClickingTogglesState(true); // Make it a toggle button
    osc2ToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "osc2Enabled", osc2ToggleButton);
    addAndMakeVisible(osc2ToggleButton);

    osc3ToggleButton.setButtonText("OSC 3");
    osc3ToggleButton.setClickingTogglesState(true); // Make it a toggle
    osc3ToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "osc3Enabled", osc3ToggleButton);
    addAndMakeVisible(osc3ToggleButton);
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
    auto padding = 10;
    auto size = 30;
    auto area = getLocalBounds().reduced(padding);

    auto numComponents = 3;
    auto componentWidth = (area.getWidth() - (numComponents - 1) * padding) / numComponents;

    auto left = area.removeFromLeft(componentWidth);
    auto right = area.removeFromRight(componentWidth);

    osc1ToggleButton.setBounds(left.removeFromTop(size));
    left.removeFromTop(padding);
    osc2ToggleButton.setBounds(left.removeFromTop(size));
    left.removeFromTop(padding);
    osc3ToggleButton.setBounds(left.removeFromTop(size));

    waveformSelector.setBounds(right.removeFromTop(size));
    right.removeFromTop(padding);
    waveformSelector2.setBounds(right.removeFromTop(size));
    right.removeFromTop(padding);
    waveformSelector3.setBounds(right.removeFromTop(size));
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