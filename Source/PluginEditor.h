/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <iostream>
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <juce_audio_basics/juce_audio_basics.h>


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

    MaxSynthAudioProcessor &audioProcessor;

    juce::AudioDeviceManager deviceManager; 
    juce::ComboBox midiInputList;          
    juce::Label midiInputListLabel;
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MaxSynthAudioProcessorEditor)
};
