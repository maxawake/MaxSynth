/*
  ==============================================================================

    ADSRComponent.cpp
    Created: 29 Aug 2025 10:28:43pm
    Author:  max

  ==============================================================================
*/

#include "ADSRComponent.h"


ADSRComponent::ADSRComponent(juce::AudioProcessorValueTreeState& apvts)
{ 
    using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    attackAttachment = std::make_unique<sliderAttachment>(apvts, "attack", attackSlider);
    decayAttachment = std::make_unique<sliderAttachment>(apvts, "decay", decaySlider);
    sustainAttachment = std::make_unique<sliderAttachment>(apvts, "sustain", sustainSlider);
    releaseAttachment = std::make_unique<sliderAttachment>(apvts, "release", releaseSlider);

    setStyle(attackSlider);
    setStyle(decaySlider); 
    setStyle(sustainSlider);
    setStyle(releaseSlider);

    addAndMakeVisible(attackSlider);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(sustainSlider);
    addAndMakeVisible(releaseSlider);
}

ADSRComponent::~ADSRComponent()
{
}

void ADSRComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void ADSRComponent::resized()
{
    auto area = getLocalBounds().reduced(8);

    attackSlider.setBounds(area.removeFromLeft(40).reduced(8));
    decaySlider.setBounds(area.removeFromLeft(40).reduced(8));
    sustainSlider.setBounds(area.removeFromLeft(40).reduced(8));
    releaseSlider.setBounds(area.removeFromLeft(40).reduced(8));
}

void ADSRComponent::setStyle(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
}