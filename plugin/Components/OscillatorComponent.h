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
    void setLabels(juce::ComboBox& box);
private:
    
    using comboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using buttonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    juce::ComboBox waveformSelector;
    std::unique_ptr<comboBoxAttachment> waveformAttachment;

    juce::ComboBox waveformSelector2;
    std::unique_ptr<comboBoxAttachment> waveformAttachment2;

    juce::ComboBox waveformSelector3;
    std::unique_ptr<comboBoxAttachment> waveformAttachment3;

    juce::TextButton osc1ToggleButton;
    juce::TextButton osc2ToggleButton;
    juce::TextButton osc3ToggleButton;
    std::unique_ptr<buttonAttachment> osc1ToggleAttachment;
    std::unique_ptr<buttonAttachment> osc2ToggleAttachment;
    std::unique_ptr<buttonAttachment> osc3ToggleAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscillatorComponent)
};