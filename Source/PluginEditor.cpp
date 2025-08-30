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
    adsrComponent(audioProcessor.getAPVTS()),  // Initialize adsrComponent first
    filterComponent(audioProcessor.getAPVTS()), // Initialize filterComponent second
    oscillatorComponent(audioProcessor.getAPVTS()),
    keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setSize(800, 400);
    setResizable(true, true);
    setResizeLimits(400, 300, 2000, 1500);

    setMidiInput(1);

    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);

    addAndMakeVisible(adsrComponent);
    addAndMakeVisible(filterComponent);
    addAndMakeVisible(oscillatorComponent);
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
    auto area = getLocalBounds();
    auto padding = scaled(8);
    
    // Reserve space for keyboard at the bottom
    auto keyboardHeight = scaled(80);
    auto keyboardArea = area.removeFromBottom(keyboardHeight);
    keyboardComponent.setBounds(keyboardArea.reduced(padding));
    
    // Add vertical spacing between keyboard and other components
    area.removeFromBottom(padding);
    
    // Create horizontal layout for the main components
    auto componentHeight = area.getHeight();
    auto componentWidth = (area.getWidth() - 4 * padding) / 3; // 3 components + margins
    
    // ADSR Component (left)
    auto adsrArea = area.removeFromLeft(componentWidth);
    adsrComponent.setBounds(adsrArea.reduced(padding));
    
    // Add horizontal spacing
    area.removeFromLeft(padding);
    
    // Oscillator Component (middle)
    auto oscillatorArea = area.removeFromLeft(componentWidth);
    oscillatorComponent.setBounds(oscillatorArea.reduced(padding));
    
    // Add horizontal spacing
    area.removeFromLeft(padding);
    
    // Filter Component (right - takes remaining space)
    filterComponent.setBounds(area.reduced(padding));
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
