/*
  ==============================================================================

    FilterComponent.cpp
    Created: 30 Aug 2025 10:29:23pm
    Author:  max

  ==============================================================================
*/

#include "../Components/FilterComponent.h"


FilterComponent::FilterComponent(juce::AudioProcessorValueTreeState& apvts)
{
    cutoffAttachment = std::make_unique<sliderAttachment>(apvts, "filterCutoff", cutoffSlider);
    resonanceAttachment = std::make_unique<sliderAttachment>(apvts, "filterResonance", resonanceSlider);
    filterModeAttachment = std::make_unique<comboBoxAttachment>(apvts, "filterMode", filterModeComboBox);

    setStyle(cutoffSlider);
    cutoffSlider.setTextValueSuffix(" Hz");
    addAndMakeVisible(cutoffSlider);

    setStyle(resonanceSlider);
    resonanceSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(resonanceSlider);

    // Set up filter mode combo box
    filterModeComboBox.addItem("LPF 12dB", 1);
    filterModeComboBox.addItem("LPF 24dB", 2);
    filterModeComboBox.addItem("HPF 12dB", 3);
    filterModeComboBox.addItem("HPF 24dB", 4);
    filterModeComboBox.addItem("BPF 12dB", 5);
    filterModeComboBox.addItem("BPF 24dB", 6);
    filterModeComboBox.setSelectedId(2); // Default to LPF 24dB
    addAndMakeVisible(filterModeComboBox);
}

void FilterComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void FilterComponent::resized()
{
    auto area = getLocalBounds();
    auto padding = 5;
    
    // Filter mode combo box at the top
    filterModeComboBox.setBounds(area.removeFromTop(30).reduced(padding));
    
    // Add vertical spacing
    area.removeFromTop(padding);
    
    // Split remaining area horizontally for the two knobs
    auto knobWidth = (area.getWidth() - 3 * padding) / 2; // 2 knobs + spacing
    
    // Cutoff slider (left)
    auto cutoffArea = area.removeFromLeft(knobWidth);
    cutoffSlider.setBounds(cutoffArea.reduced(padding));
    
    // Add horizontal spacing
    area.removeFromLeft(padding);
    
    // Resonance slider (right)
    resonanceSlider.setBounds(area.reduced(padding));
}

FilterComponent::~FilterComponent()
{
}

void FilterComponent::setStyle(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::red);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::blue);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    // Add any other styling you want
}