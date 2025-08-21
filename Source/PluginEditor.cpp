/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MaxSynthAudioProcessorEditor::MaxSynthAudioProcessorEditor(MaxSynthAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p), keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    // setAudioChannels (0, 2);
    setSize(600, 160);

    auto midiInputs = juce::MidiInput::getAvailableDevices();

    for (auto input : midiInputs)
    {
        if (deviceManager.isMidiInputDeviceEnabled(input.identifier))
        {
            setMidiInput(midiInputs.indexOf(input));
            break;
        }
    }

    // if no enabled devices were found just use the first one in the list
    if (midiInputList.getSelectedId() == 0)
        setMidiInput(0);

    // startTimer (400);
    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);
}

MaxSynthAudioProcessorEditor::~MaxSynthAudioProcessorEditor()
{
    keyboardState.removeListener(this);
    deviceManager.removeMidiInputDeviceCallback(juce::MidiInput::getAvailableDevices()[midiInputList.getSelectedItemIndex()].identifier, this);
}

//==============================================================================
void MaxSynthAudioProcessorEditor::paint(juce::Graphics &g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // g.setColour (juce::Colours::white);
    // g.setFont (15.0f);
    // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void MaxSynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds();

    keyboardComponent.setBounds(area.removeFromTop(80).reduced(8));
}

void MaxSynthAudioProcessorEditor::handleNoteOn(juce::MidiKeyboardState *, int midiChannel, int midiNoteNumber, float velocity)
{
    if (!isAddingFromMidiInput)
    {
        auto m = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
        m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
        postMessageToList(m, "On-Screen Keyboard");
    }
    std::cout << "Note On: " << midiNoteNumber << std::endl;
    audioProcessor.freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    audioProcessor.t = 0.0f; // Reset time for the new note
}

void MaxSynthAudioProcessorEditor::handleNoteOff(juce::MidiKeyboardState *, int midiChannel, int midiNoteNumber, float /*velocity*/)
{
    if (!isAddingFromMidiInput)
    {
        auto m = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber);
        m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
        postMessageToList(m, "On-Screen Keyboard");
    }
}