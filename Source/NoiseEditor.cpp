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

    torsionSlider.setSliderStyle (Slider::Rotary);
    torsionSlider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 15);
    torsionSlider.setGetTextFromValueFunc([](double value) {
        return String(round(value / 4.0f * 360)) + String(CharPointer_UTF8 ("\xc2\xb0"));
    });
    torsionSlider.setRotaryParameters (0, float_Pi * 2.0f, false);
    sliderGroup.addAndMakeVisible (&torsionSlider);
    torsionAttachment = new SliderAttachment (processor.parameters, "torsion", torsionSlider);

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

    torsionLabel.setText("Torsion", NotificationType::dontSendNotification);
    torsionLabel.setJustificationType (Justification::centred);
    sliderGroup.addAndMakeVisible (&torsionLabel);

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

    torsionSlider.setBounds (sliderWidth * 1, sliderY, sliderWidth, sliderHeight);
    octavesSlider.setBounds (sliderWidth * 2, sliderY, sliderWidth, sliderHeight);
    persistenceSlider.setBounds (sliderWidth * 3, sliderY, sliderWidth, sliderHeight);

    torsionLabel.setBounds(sliderWidth * 1, sliderY + sliderHeight + 5, sliderWidth, 10);
    octavesLabel.setBounds(sliderWidth * 2, sliderY + sliderHeight + 5, sliderWidth, 10);
    persistenceLabel.setBounds(sliderWidth * 3, sliderY + sliderHeight + 5, sliderWidth, 10);
}
