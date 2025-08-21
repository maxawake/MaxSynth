/*
  ==============================================================================

    Synth.cpp
    Created: 21 Aug 2025 6:02:45pm
    Author:  max

  ==============================================================================
*/

#include "Synth.h"
#include <iostream>

Synth::Synth()
{
    // Constructor implementation
}

Synth::~Synth()
{
    // Destructor implementation
}

void Synth::releaseResources()
{
    // Release resources implementation
    // This is where you would clean up any resources used during playback
    std::cout << "release" << std::endl;
}

void Synth::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare to play implementation
    // This is where you would initialize any resources needed for playback
    std::cout << "prepare" << std::endl;
}

void Synth::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Process block implementation
    // This is where you would handle the audio processing for each block of audio
    std::cout << "process" << std::endl;
}