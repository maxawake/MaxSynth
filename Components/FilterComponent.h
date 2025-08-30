/*
  ==============================================================================

    FilterComponent.h
    Created: 30 Aug 2025 10:29:23pm
    Author:  max

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class FilterComponent : public juce::Component
{
public:
    FilterComponent(juce::AudioProcessorValueTreeState& apvts);
    ~FilterComponent();

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider cutoffSlider, resonanceSlider;
    juce::ComboBox filterModeComboBox;
    
    using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using comboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    
    std::unique_ptr<sliderAttachment> cutoffAttachment;
    std::unique_ptr<sliderAttachment> resonanceAttachment;
    std::unique_ptr<comboBoxAttachment> filterModeAttachment;

    void setStyle(juce::Slider& slider);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterComponent)
};
