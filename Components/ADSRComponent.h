/*
  ==============================================================================

    ADSRComponent.h
    Created: 29 Aug 2025 10:28:43pm
    Author:  max

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class ADSRComponent : public juce::Component
{
public:
    ADSRComponent(juce::AudioProcessorValueTreeState& apvts);
    ~ADSRComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;

    using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<sliderAttachment> attackAttachment;
    std::unique_ptr<sliderAttachment> decayAttachment;
    std::unique_ptr<sliderAttachment> sustainAttachment;
    std::unique_ptr<sliderAttachment> releaseAttachment;

    void setStyle(juce::Slider& slider);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRComponent)
};