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
    
    // ADSR sliders for filter envelope
    juce::Slider filterAttackSlider, filterDecaySlider, filterSustainSlider, filterReleaseSlider;
    
    // LFO controls
    juce::Slider lfoFreqSlider, lfoAmountSlider;
    
    using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using comboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    
    std::unique_ptr<sliderAttachment> cutoffAttachment;
    std::unique_ptr<sliderAttachment> resonanceAttachment;
    std::unique_ptr<comboBoxAttachment> filterModeAttachment;
    
    // ADSR attachments
    std::unique_ptr<sliderAttachment> filterAttackAttachment;
    std::unique_ptr<sliderAttachment> filterDecayAttachment;
    std::unique_ptr<sliderAttachment> filterSustainAttachment;
    std::unique_ptr<sliderAttachment> filterReleaseAttachment;
    
    // LFO attachments
    std::unique_ptr<sliderAttachment> lfoFreqAttachment;
    std::unique_ptr<sliderAttachment> lfoAmountAttachment;

    void setStyle(juce::Slider& slider);
    void setSmallStyle(juce::Slider& slider);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterComponent)
};
