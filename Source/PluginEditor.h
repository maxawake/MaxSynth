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
//==============================================================================
/**
 */
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
    void postMessageToList(const juce::MidiMessage &message, const juce::String &source)
    {
        std::cout << "Message from " << std::endl;
    }
    bool isAddingFromMidiInput = false;

private:
    void setMidiInput(int index)
    {
        auto list = juce::MidiInput::getAvailableDevices();

        deviceManager.removeMidiInputDeviceCallback(list[lastInputIndex].identifier, this);

        auto newInput = list[index];

        if (!deviceManager.isMidiInputDeviceEnabled(newInput.identifier))
            deviceManager.setMidiInputDeviceEnabled(newInput.identifier, true);

        deviceManager.addMidiInputDeviceCallback(newInput.identifier, this);
        midiInputList.setSelectedId(index + 1, juce::dontSendNotification);

        lastInputIndex = index;
    }

    // These methods handle callbacks from the midi device + on-screen keyboard..
    void handleIncomingMidiMessage(juce::MidiInput *source, const juce::MidiMessage &message) override
    {
        const juce::ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
        keyboardState.processNextMidiEvent(message);
        postMessageToList(message, source->getName());
    }
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    juce::AudioDeviceManager deviceManager; // [1]
    juce::ComboBox midiInputList;           // [2]
    juce::Label midiInputListLabel;
    int lastInputIndex = 0; // [3]
    MaxSynthAudioProcessor &audioProcessor;
    juce::MidiKeyboardState keyboardState;
    // SynthAudioSource synthAudioSource;
    juce::MidiKeyboardComponent keyboardComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MaxSynthAudioProcessorEditor)
};
