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

#include <sstream>
#include <iomanip>

//==============================================================================
AndesAudioProcessorEditor::AndesAudioProcessorEditor (AndesAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p), waveformVisualiser(processor),
      keyboardComponent (keyboardState, MidiKeyboardComponent::horizontalKeyboard)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 414);

    noiseGroup.setText ("Noise");
    addAndMakeVisible (&noiseGroup);

    xSlider.setSliderStyle (Slider::Rotary);
    xSlider.setRange(0, 1, 0);
    xSlider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 15);
    xSlider.addListener (this);
    noiseGroup.addAndMakeVisible (&xSlider);

    ySlider.setSliderStyle (Slider::Rotary);
    ySlider.setRange(0, 1, 0);
    ySlider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 15);
    ySlider.addListener (this);
    noiseGroup.addAndMakeVisible (&ySlider);

    octavesSlider.setSliderStyle (Slider::Rotary);
    octavesSlider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 15);
    octavesSlider.setRange(1, 16, 1);
    octavesSlider.addListener (this);
    noiseGroup.addAndMakeVisible (&octavesSlider);

    persistenceSlider.setSliderStyle (Slider::Rotary);
    persistenceSlider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 15);
    persistenceSlider.setRange(0, 1, 0);
    persistenceSlider.addListener (this);
    noiseGroup.addAndMakeVisible (&persistenceSlider);

    seedEditor.setReadOnly(true); // TODO: actually should be editable
    seedEditor.setText (String::toHexString((int) processor.noise.getSeed()));

    randomizeButton.addListener (this);
    randomizeButton.setButtonText ("Randomize");

    addAndMakeVisible (&seedEditor);
    addAndMakeVisible (&randomizeButton);
    addAndMakeVisible (&waveformVisualiser);

    keyboardComponent.setWantsKeyboardFocus(true);
    addAndMakeVisible (&keyboardComponent);

    startTimerHz(30);
    srand (time(NULL));
}

AndesAudioProcessorEditor::~AndesAudioProcessorEditor()
{
}

//==============================================================================
void AndesAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void AndesAudioProcessorEditor::resized()
{
    seedEditor.setBounds (160, 190, 80, 26);
    randomizeButton.setBounds (255, 190, 80, 26);
    waveformVisualiser.setBounds (165, 10, 170, 170);
    noiseGroup.setBounds (100, 230, 300, 100);

    int sliderY = 10;
    int sliderHeight = noiseGroup.getHeight() - 30;    
    int sliderWidth = noiseGroup.getWidth() / 4;

    xSlider.setBounds (sliderWidth * 0, sliderY, sliderWidth, sliderHeight);
    ySlider.setBounds (sliderWidth * 1, sliderY, sliderWidth, sliderHeight);
    octavesSlider.setBounds (sliderWidth * 2, sliderY, sliderWidth, sliderHeight);
    persistenceSlider.setBounds (sliderWidth * 3, sliderY, sliderWidth, sliderHeight);

    keyboardComponent.setBounds (0, getHeight() - 64, getWidth(), 64);
}

void AndesAudioProcessorEditor::buttonClicked (Button* button)
{
    uint32_t seed = rand();
    processor.noise.setSeed (seed);

    if (button == &randomizeButton)
    {
        seedEditor.setText (String::toHexString((int) seed));
        waveformVisualiser.update();
    }
}

void AndesAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if (slider == &octavesSlider)
    {
        *processor.octaves = (int) slider->getValue();
        waveformVisualiser.update();
    }
    if (slider == &persistenceSlider)
    {
        *processor.persistence = (float) slider->getValue();
        waveformVisualiser.update();
    }
    if (slider == &xSlider)
    {
        *processor.oscX = (float) slider->getValue();
        waveformVisualiser.update();
    }
    if (slider == &ySlider)
    {
        *processor.oscY = (float) slider->getValue();
        waveformVisualiser.update();
    }
}

void AndesAudioProcessorEditor::handleNoteOn (MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{

}

void AndesAudioProcessorEditor::handleNoteOff (MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{

}

void AndesAudioProcessorEditor::timerCallback ()
{
    const OwnedArray<AudioProcessorParameter>& params = getAudioProcessor()->getParameters();
    for (int i = 0; i < params.size(); ++i)
    {
        if (const AudioParameterFloat* param = dynamic_cast<AudioParameterFloat*> (params[i]))
        {
            if (param->paramID == "persistence") {
                persistenceSlider.setValue ((double) *param, dontSendNotification);
                waveformVisualiser.update();
            }

            if (param->paramID == "x") {
                xSlider.setValue ((double) *param, dontSendNotification);
                waveformVisualiser.update();
            }

            if (param->paramID == "y") {
                ySlider.setValue ((double) *param, dontSendNotification);
                waveformVisualiser.update();
            }
        }

        if (const AudioParameterInt* param = dynamic_cast<AudioParameterInt*> (params[i]))
        {
            if (param->paramID == "octaves") {
                octavesSlider.setValue ((int) *param, dontSendNotification);
                waveformVisualiser.update();
            }
        }
    }
}