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
    g.fillAll(juce::Colours::black);
}

void ADSRComponent::resized()
{ 
    auto padding = 10;
    auto area = getLocalBounds().reduced(padding);

    int numComponents = 4;
    auto componentWidth = (area.getWidth() - (numComponents) * padding) / numComponents;

    attackSlider.setBounds(area.removeFromLeft(componentWidth).reduced(padding));
    decaySlider.setBounds(area.removeFromLeft(componentWidth).reduced(padding));
    sustainSlider.setBounds(area.removeFromLeft(componentWidth).reduced(padding));
    releaseSlider.setBounds(area.removeFromLeft(componentWidth).reduced(padding));
}

void ADSRComponent::setStyle(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
}