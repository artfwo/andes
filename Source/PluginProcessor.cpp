/*
    Andes - synthesiser plugin based on Perlin noise
    Copyright (C) 2017  Artem Popov <art@artfwo.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "AndesSound.h"
#include "AndesVoice.h"

//==============================================================================
AndesAudioProcessor::AndesAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    synth.clearVoices();
    synth.clearSounds();

    for (int i = 8; --i >= 0; ) {
        synth.addVoice (new AndesVoice (*this));
    }

    synth.addSound (new AndesSound());

    addParameter (oscX = new AudioParameterFloat ("x", // parameter ID
                                                  "x", // parameter name
                                                  0.0f,   // mininum value
                                                  1.0f,   // maximum value
                                                  0.0f)); // default value

    addParameter (oscY = new AudioParameterFloat ("y", // parameter ID
                                                  "y", // parameter name
                                                  0.0f,   // mininum value
                                                  1.0f,   // maximum value
                                                  0.0f)); // default value

    addParameter (octaves = new AudioParameterInt ("octaves", // parameter ID
                                                  "octaves", // parameter name
                                                  1,   // mininum value
                                                  16,   // maximum value
                                                  1)); // default value

    addParameter (persistence = new AudioParameterFloat ("persistence", // parameter ID
                                                  "persistence", // parameter name
                                                  0.0f,   // mininum value
                                                  1.0f,   // maximum value
                                                  0.5f)); // default value

}

AndesAudioProcessor::~AndesAudioProcessor()
{
}

//==============================================================================
const String AndesAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AndesAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AndesAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double AndesAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AndesAudioProcessor::getNumPrograms()
{
    return 100;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AndesAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AndesAudioProcessor::setCurrentProgram (int index)
{
}

const String AndesAudioProcessor::getProgramName (int index)
{
    return "Default";
}

void AndesAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void AndesAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synth.setCurrentPlaybackSampleRate (sampleRate);
}

void AndesAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AndesAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AndesAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (AudioProcessorEditor* editor = getActiveEditor())
    {
        if (AndesAudioProcessorEditor* andesEditor = dynamic_cast<AndesAudioProcessorEditor*> (editor))
        {
            andesEditor->keyboardState.processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);
        }
    }

    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool AndesAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* AndesAudioProcessor::createEditor()
{
    return new AndesAudioProcessorEditor (*this);
}

//==============================================================================
void AndesAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AndesAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AndesAudioProcessor();
}
