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
    adsrFilterAmountAttachment = std::make_unique<sliderAttachment>(apvts, "adsrFilterAmount", adsrFilterAmountSlider);

    setStyle(cutoffSlider);
    cutoffSlider.setTextValueSuffix(" Hz");
    addAndMakeVisible(cutoffSlider);

    setStyle(resonanceSlider);
    resonanceSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(resonanceSlider);

    setStyle(adsrFilterAmountSlider);
    adsrFilterAmountSlider.setTextValueSuffix(" %");
    addAndMakeVisible(adsrFilterAmountSlider);

    // Set up filter mode combo box
    filterModeComboBox.addItem("LPF 12dB", 1);
    filterModeComboBox.addItem("LPF 24dB", 2);
    filterModeComboBox.addItem("HPF 12dB", 3);
    filterModeComboBox.addItem("HPF 24dB", 4);
    filterModeComboBox.addItem("BPF 12dB", 5);
    filterModeComboBox.addItem("BPF 24dB", 6);
    filterModeComboBox.setSelectedId(2); // Default to LPF 24dB
    addAndMakeVisible(filterModeComboBox);

    // Set up ADSR button
    adsrToggleButton.setButtonText("ADSR");
    adsrToggleButton.setClickingTogglesState(true); // Make it a toggle button
    adsrToggleButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::green);
    adsrToggleButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    adsrToggleButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    adsrToggleAttachment = std::make_unique<buttonAttachment>(apvts, "filterADSREnabled", adsrToggleButton);
    addAndMakeVisible(adsrToggleButton);

    // Set up filter ADSR sliders
    filterAttackAttachment = std::make_unique<sliderAttachment>(apvts, "filterAttack", filterAttackSlider);
    filterDecayAttachment = std::make_unique<sliderAttachment>(apvts, "filterDecay", filterDecaySlider);
    filterSustainAttachment = std::make_unique<sliderAttachment>(apvts, "filterSustain", filterSustainSlider);
    filterReleaseAttachment = std::make_unique<sliderAttachment>(apvts, "filterRelease", filterReleaseSlider);

    setSmallStyle(filterAttackSlider);
    filterAttackSlider.setTextValueSuffix(" s");
    addAndMakeVisible(filterAttackSlider);

    setSmallStyle(filterDecaySlider);
    filterDecaySlider.setTextValueSuffix(" s");
    addAndMakeVisible(filterDecaySlider);

    setSmallStyle(filterSustainSlider);
    addAndMakeVisible(filterSustainSlider);

    setSmallStyle(filterReleaseSlider);
    filterReleaseSlider.setTextValueSuffix(" s");
    addAndMakeVisible(filterReleaseSlider);

    
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
    area.removeFromTop(padding);
    
    // Main controls section (cutoff and resonance)
    auto mainControlsHeight = 120;
    auto mainControlsArea = area.removeFromTop(mainControlsHeight);
    
    // Split main controls horizontally
    auto knobWidth = (mainControlsArea.getWidth() - 3 * padding) / 2;
    
    // Cutoff slider (left)
    auto cutoffArea = mainControlsArea.removeFromLeft(knobWidth);

    adsrToggleButton.setBounds(cutoffArea.removeFromTop(30).reduced(padding));
    cutoffSlider.setBounds(cutoffArea.reduced(padding));
    
    // Add horizontal spacing
    mainControlsArea.removeFromLeft(padding);
    
    // Resonance slider (right)
    resonanceSlider.setBounds(mainControlsArea.reduced(padding));
    
    // Add vertical spacing
    area.removeFromTop(padding);
    
    // ADSR section
    auto adsrHeight = 80;
    auto adsrArea = area.removeFromTop(adsrHeight);
    auto adsrSliderWidth = (adsrArea.getWidth() - 6 * padding) / 5; // 5 sliders

    filterAttackSlider.setBounds(adsrArea.removeFromLeft(adsrSliderWidth).reduced(padding));
    adsrArea.removeFromLeft(padding);
    filterDecaySlider.setBounds(adsrArea.removeFromLeft(adsrSliderWidth).reduced(padding));
    adsrArea.removeFromLeft(padding);
    filterSustainSlider.setBounds(adsrArea.removeFromLeft(adsrSliderWidth).reduced(padding));
    adsrArea.removeFromLeft(padding);
    filterReleaseSlider.setBounds(adsrArea.removeFromLeft(adsrSliderWidth).reduced(padding));
    adsrFilterAmountSlider.setBounds(adsrArea.reduced(padding));
}

FilterComponent::~FilterComponent()
{
}

void FilterComponent::setStyle(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
}

void FilterComponent::setSmallStyle(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 15);
}