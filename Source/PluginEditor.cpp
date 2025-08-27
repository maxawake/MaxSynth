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
    // Initialize audio device manager
    // deviceManager.initialiseWithDefaultDevices(0, 2); // 0 inputs, 2 outputs
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
    // if (midiInputList.getSelectedId() == 0)
    //     setMidiInput(0);

    setMidiInput(1);

    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);

    using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    attackAttachment = std::make_unique<sliderAttachment>(audioProcessor.apvts, "attack", attackSlider);
    decayAttachment = std::make_unique<sliderAttachment>(audioProcessor.apvts, "decay", decaySlider);
    sustainAttachment = std::make_unique<sliderAttachment>(audioProcessor.apvts, "sustain", sustainSlider);
    releaseAttachment = std::make_unique<sliderAttachment>(audioProcessor.apvts, "release", releaseSlider);
    waveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "waveform", waveformSelector);

    attackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    decaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    sustainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    sustainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    releaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    addAndMakeVisible(attackSlider);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(sustainSlider);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(waveformSelector);
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

    attackSlider.setBounds(area.removeFromLeft(40).reduced(8));
    decaySlider.setBounds(area.removeFromLeft(40).reduced(8));
    sustainSlider.setBounds(area.removeFromLeft(40).reduced(8));
    releaseSlider.setBounds(area.removeFromLeft(40).reduced(8));
    waveformSelector.setBounds(area.removeFromLeft(40).reduced(8));
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
    midiInputList.setSelectedId(index + 1, juce::dontSendNotification);

    lastInputIndex = index;
}
