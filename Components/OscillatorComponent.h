/*
  ==============================================================================

    OscillatorComponent.h
    Created: 30 Aug 2025 11:19:44pm
    Author:  max

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class OscillatorComponent : public juce::Component
{
public:
    OscillatorComponent(juce::AudioProcessorValueTreeState& apvts);
    ~OscillatorComponent();

    void paint(juce::Graphics&) override;
    void resized() override;
private:
    juce::ComboBox waveformSelector;
    
    using comboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::unique_ptr<comboBoxAttachment> waveformAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscillatorComponent)
};