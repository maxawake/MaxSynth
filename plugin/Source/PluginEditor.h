/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <iostream>
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../Components/ADSRComponent.h"
#include "../Components/FilterComponent.h"
#include "../Components/OscillatorComponent.h"
#include "../Components/ScopeComponent.h"

class MaxSynthAudioProcessorEditor : public juce::AudioProcessorEditor,
                                     private juce::MidiInputCallback,
                                     private juce::MidiKeyboardStateListener
{
public:
    MaxSynthAudioProcessorEditor(MaxSynthAudioProcessor &);
    ~MaxSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;
    void handleNoteOn(juce::MidiKeyboardState *, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState *, int midiChannel, int midiNoteNumber, float /*velocity*/) override;
    void setMidiInput(int index);

    bool isAddingFromMidiInput = false;
    int lastInputIndex = 0; 


private:
    void handleIncomingMidiMessage(juce::MidiInput *source, const juce::MidiMessage &message) override
    {
        const juce::ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
        keyboardState.processNextMidiEvent(message);
    }

    // Add this helper function
    float getScaleFactor() const
    {
        auto bounds = getLocalBounds();
        // Base size for scaling reference (adjust as needed)
        const float baseWidth = 600.0f;
        const float baseHeight = 170.0f;
        
        float scaleX = bounds.getWidth() / baseWidth;
        float scaleY = bounds.getHeight() / baseHeight;
        
        // Use minimum to maintain aspect ratio, or average for more flexible scaling
        return (scaleX + scaleY) * 0.5f; // Average scaling
    }
    
    int scaled(int value) const 
    {
        return static_cast<int>(value * getScaleFactor());
    }

    MaxSynthAudioProcessor &audioProcessor;
    ADSRComponent adsrComponent;
    FilterComponent filterComponent;
    OscillatorComponent oscillatorComponent;

    juce::AudioDeviceManager deviceManager; 
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;
    ScopeComponent<float> scopeComponent;

    juce::ComboBox waveformSelector;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveformAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MaxSynthAudioProcessorEditor)
};
