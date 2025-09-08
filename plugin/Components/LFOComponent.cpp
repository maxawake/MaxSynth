#include "../Components/LFOComponent.h"

LFOComponent::LFOComponent(juce::AudioProcessorValueTreeState& apvts)
{
    targetAttachment = std::make_unique<comboBoxAttachment>(apvts, "lfoTarget", targetSelector);
    targetSelector.addItem("Pitch", 1);
    targetSelector.addItem("Filter Cutoff", 2);
    targetSelector.addItem("Amplitude", 3);
    targetSelector.addItem("Filter Resonance", 4);
    addAndMakeVisible(targetSelector);

    // Set up LFO controls
    lfoFreqAttachment = std::make_unique<sliderAttachment>(apvts, "lfoFreq", lfoFreqSlider);
    lfoAmountAttachment = std::make_unique<sliderAttachment>(apvts, "lfoAmount", lfoAmountSlider);

    lfoFreqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lfoFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    lfoFreqSlider.setTextValueSuffix(" Hz");
    addAndMakeVisible(lfoFreqSlider);

    lfoAmountSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lfoAmountSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    lfoAmountSlider.setTextValueSuffix(" %");
    addAndMakeVisible(lfoAmountSlider);
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

    auto padding = 5;

    targetSelector.setBounds(area.removeFromTop(30).reduced(padding));
    area.removeFromTop(padding);

    auto knobWidth = (area.getWidth() - 3 * padding) / 2;
    lfoFreqSlider.setBounds(area.removeFromLeft(knobWidth).reduced(padding));
    area.removeFromLeft(padding);
    lfoAmountSlider.setBounds(area.removeFromLeft(knobWidth).reduced(padding));
    area.removeFromLeft(padding);

}