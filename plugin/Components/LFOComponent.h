#pragma once

#include <JuceHeader.h>

class LFOComponent : public juce::Component
{
public:
    LFOComponent(juce::AudioProcessorValueTreeState& apvts);
    ~LFOComponent();

    void paint(juce::Graphics&) override;
    void resized() override;
private:
    juce::ComboBox targetSelector;
    
    using comboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::unique_ptr<comboBoxAttachment> targetAttachment;

    // LFO controls
    juce::Slider lfoFreqSlider, lfoAmountSlider;

    // LFO attachments
    using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<sliderAttachment> lfoFreqAttachment;
    std::unique_ptr<sliderAttachment> lfoAmountAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOComponent)
};