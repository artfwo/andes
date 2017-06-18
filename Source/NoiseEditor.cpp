/*
    Andes - synthesiser plugin based on Perlin EnvelopeGenerator
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

#include "../JuceLibraryCode/JuceHeader.h"
#include "NoiseEditor.h"

//==============================================================================
NoiseEditor::NoiseEditor(AndesAudioProcessor& processor) : processor(processor)
{
    sliderGroup.setText ("Noise");
    addAndMakeVisible (sliderGroup);

    xSlider.setSliderStyle (Slider::Rotary);
    xSlider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 15);
    xSlider.setGetTextFromValueFunc([](double value) {
        return "." + String(round(value * 100));
    });
    sliderGroup.addAndMakeVisible (&xSlider);
    xAttachment = new SliderAttachment (processor.parameters, "x", xSlider);

    ySlider.setSliderStyle (Slider::Rotary);
    ySlider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 15);
    ySlider.setGetTextFromValueFunc([](double value) {
        return "." + String(round(value * 100));
    });
    sliderGroup.addAndMakeVisible (&ySlider);
    yAttachment = new SliderAttachment (processor.parameters, "y", ySlider);

    octavesSlider.setSliderStyle (Slider::Rotary);
    octavesSlider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 15);
    sliderGroup.addAndMakeVisible (&octavesSlider);
    octavesAttachment = new SliderAttachment (processor.parameters, "octaves", octavesSlider);

    persistenceSlider.setSliderStyle (Slider::Rotary);
    persistenceSlider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 15);
    persistenceSlider.setGetTextFromValueFunc([](double value) {
        return "." + String(round(value * 100));
    });
    sliderGroup.addAndMakeVisible (&persistenceSlider);
    persistenceAttachment = new SliderAttachment (processor.parameters, "persistence", persistenceSlider);

    xLabel.setText("X", NotificationType::dontSendNotification);
    xLabel.setJustificationType (Justification::centred);
    sliderGroup.addAndMakeVisible (&xLabel);

    yLabel.setText("Y", NotificationType::dontSendNotification);
    yLabel.setJustificationType (Justification::centred);
    sliderGroup.addAndMakeVisible (&yLabel);

    octavesLabel.setText("Octaves", NotificationType::dontSendNotification);
    octavesLabel.setJustificationType (Justification::centred);
    sliderGroup.addAndMakeVisible (&octavesLabel);

    persistenceLabel.setText("Persistence", NotificationType::dontSendNotification);
    persistenceLabel.setJustificationType (Justification::centred);
    sliderGroup.addAndMakeVisible (&persistenceLabel);
}

NoiseEditor::~NoiseEditor()
{
}

void NoiseEditor::resized()
{
    sliderGroup.setBounds (0, 0, getWidth(), getHeight());

    int sliderY = 15;
    int sliderHeight = sliderGroup.getHeight() - 40;
    int sliderWidth = sliderGroup.getWidth() / 4;

    xSlider.setBounds (sliderWidth * 0, sliderY, sliderWidth, sliderHeight);
    ySlider.setBounds (sliderWidth * 1, sliderY, sliderWidth, sliderHeight);
    octavesSlider.setBounds (sliderWidth * 2, sliderY, sliderWidth, sliderHeight);
    persistenceSlider.setBounds (sliderWidth * 3, sliderY, sliderWidth, sliderHeight);

    xLabel.setBounds(sliderWidth * 0, sliderY + sliderHeight + 5, sliderWidth, 10);
    yLabel.setBounds(sliderWidth * 1, sliderY + sliderHeight + 5, sliderWidth, 10);
    octavesLabel.setBounds(sliderWidth * 2, sliderY + sliderHeight + 5, sliderWidth, 10);
    persistenceLabel.setBounds(sliderWidth * 3, sliderY + sliderHeight + 5, sliderWidth, 10);
}
