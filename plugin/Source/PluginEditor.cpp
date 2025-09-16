/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MaxSynthAudioProcessorEditor::MaxSynthAudioProcessorEditor(MaxSynthAudioProcessor &p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      adsrComponent(audioProcessor.getAPVTS()),   // Initialize adsrComponent first
      filterComponent(audioProcessor.getAPVTS()), // Initialize filterComponent second
      oscillatorComponent(audioProcessor.getAPVTS()),
      lfoComponent(audioProcessor.getAPVTS()), // Initialize lfoComponent third
      keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
      scopeComponent(audioProcessor.getAudioBufferQueue())
{
    setLookAndFeel(&otherLookAndFeel);
    setSize(1200, 500); // Increased height for three-row layout
    setResizable(true, true);
    setResizeLimits(600, 400, 2000, 1500); // Increased minimum size

    setMidiInput(1);

    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);

    addAndMakeVisible(adsrComponent);
    addAndMakeVisible(filterComponent);
    addAndMakeVisible(oscillatorComponent);
    addAndMakeVisible(lfoComponent);

    addAndMakeVisible (scopeComponent);

}

MaxSynthAudioProcessorEditor::~MaxSynthAudioProcessorEditor()
{
    keyboardState.removeListener(this);
}

//==============================================================================
void MaxSynthAudioProcessorEditor::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::black);

    // Scale font size based on component size
    float fontSize = 15.0f * getScaleFactor();
    g.setFont(fontSize);
    g.setColour(juce::Colours::black);
}

void MaxSynthAudioProcessorEditor::resized()
{
    auto editorArea = getLocalBounds();
    auto padding = 10;

    // Define row heights
    auto keyboardHeight = 80;
    auto scopeHeight = 120;
    auto controlsHeight = editorArea.getHeight() - keyboardHeight - scopeHeight - (padding * 4); // Remaining space for controls

    // Row 1: Controls at the top
    auto controlsArea = editorArea.removeFromTop(controlsHeight);
    editorArea.removeFromTop(padding); // Add spacing

    // Create horizontal layout for the control components
    int numComponents = 4;
    auto componentWidth = (controlsArea.getWidth() - (numComponents -1) * padding) / numComponents;

    // Oscillator Component (middle)
    controlsArea.removeFromLeft(padding);
    auto oscillatorArea = controlsArea.removeFromLeft(componentWidth);
    oscillatorComponent.setBounds(oscillatorArea.reduced(padding));

    // Filter Component
    controlsArea.removeFromLeft(padding);
    auto filterArea = controlsArea.removeFromLeft(componentWidth);
    filterComponent.setBounds(filterArea.reduced(padding));

    // LFO Component
    controlsArea.removeFromLeft(padding);
    auto lfoArea = controlsArea.removeFromLeft(componentWidth);
    lfoComponent.setBounds(lfoArea.reduced(padding));

    // ADSR Component
    controlsArea.removeFromLeft(padding);
    auto adsrArea = controlsArea.removeFromLeft(componentWidth);
    adsrComponent.setBounds(adsrArea.reduced(padding));      

    // Row 2: Scope in the middle
    auto scopeArea = editorArea.removeFromTop(scopeHeight);
    scopeComponent.setBounds(scopeArea.reduced(padding));
    editorArea.removeFromTop(padding); // Add spacing

    // Row 3: Keyboard at the bottom
    auto keyboardArea = editorArea.removeFromTop(keyboardHeight);
    keyboardComponent.setBounds(keyboardArea.reduced(padding));
}

void MaxSynthAudioProcessorEditor::handleNoteOn(juce::MidiKeyboardState *, int midiChannel, int midiNoteNumber, float velocity)
{
    if (!isAddingFromMidiInput)
    {
        auto m = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
        m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
        audioProcessor.getMidiMessageCollector().addMessageToQueue(m);
    }
}

void MaxSynthAudioProcessorEditor::handleNoteOff(juce::MidiKeyboardState *, int midiChannel, int midiNoteNumber, float /*velocity*/)
{
    if (!isAddingFromMidiInput)
    {
        auto m = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber);
        m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);

        audioProcessor.getMidiMessageCollector().addMessageToQueue(m);
    }
}

void MaxSynthAudioProcessorEditor::setMidiInput(int index)
{
    auto list = juce::MidiInput::getAvailableDevices();

    deviceManager.removeMidiInputDeviceCallback(list[lastInputIndex].identifier, this);

    auto newInput = list[index];

    if (!deviceManager.isMidiInputDeviceEnabled(newInput.identifier))
        deviceManager.setMidiInputDeviceEnabled(newInput.identifier, true);

    deviceManager.addMidiInputDeviceCallback(newInput.identifier, this);

    lastInputIndex = index;
}
