#include "../Components/LFOComponent.h"

LFOComponent::LFOComponent(juce::AudioProcessorValueTreeState& apvts)
{
    targetAttachment = std::make_unique<comboBoxAttachment>(apvts, "lfoTarget", targetSelector);
    addAndMakeVisible(targetSelector);
}

LFOComponent::~LFOComponent()
{
}

void LFOComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void LFOComponent::resized()
{
    auto area = getLocalBounds();
    targetSelector.setBounds(area);
}