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

#include "AndesVoice.h"
#include "AndesSound.h"

AndesVoice::AndesVoice (AndesAudioProcessor& processor)
    : currentPhase (0), phaseDelta (0), level (0), tailOff (0),
      processor(processor)
{
}

bool AndesVoice::canPlaySound (SynthesiserSound* sound)
{
    return dynamic_cast<AndesSound*> (sound) != nullptr;
}

void AndesVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition)
{
    currentPhase = 0.0;
    level = velocity;
    tailOff = 0.0;

    double cyclesPerSecond = MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    double cyclesPerSample = cyclesPerSecond / getSampleRate();

    phaseDelta = cyclesPerSample * 2.0;
}

void AndesVoice::stopNote (float velocity, bool allowTailOff)
{
    if (allowTailOff)
    {
        // start a tail-off by setting this flag. The render callback will pick up on
        // this and do a fade out, calling clearCurrentNote() when it's finished.

        if (tailOff == 0.0) // we only need to begin a tail-off if it's not already doing so - the
                            // stopNote method could be called more than once.
            tailOff = 1.0;
    }
    else
    {
        // we're being told to stop playing immediately, so reset everything..

        clearCurrentNote();
        phaseDelta = 0.0;
    }
}

void AndesVoice::pitchWheelMoved (int newPitchWheelValue)
{
}

void AndesVoice::controllerMoved (int controllerNumber, int newControllerValue)
{
}

void AndesVoice::renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (phaseDelta != 0.0)
    {
        if (tailOff > 0)
        {
            while (--numSamples >= 0)
            {
                float phase = fmod (currentPhase, 2);
                const float currentSample = processor.noise.gen (*processor.parameters.getRawParameterValue ("x"),
                                                                 *processor.parameters.getRawParameterValue ("y"),
                                                                 phase,
                                                                 (int) *processor.parameters.getRawParameterValue ("octaves"),
                                                                 *processor.parameters.getRawParameterValue ("persistence")) * level * tailOff;

                for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample (i, startSample, currentSample);

                currentPhase += phaseDelta;
                ++startSample;

                tailOff *= 0.99;

                if (tailOff <= 0.005)
                {
                    // tells the synth that this voice has stopped
                    clearCurrentNote();

                    phaseDelta = 0.0;
                    break;
                }
            }
        }
        else
        {
            while (--numSamples >= 0)
            {
                float phase = fmod (currentPhase, 2);
                const float currentSample = processor.noise.gen (*processor.parameters.getRawParameterValue ("x"),
                                                                 *processor.parameters.getRawParameterValue ("y"),
                                                                 phase,
                                                                 (int) *processor.parameters.getRawParameterValue ("octaves"),
                                                                 *processor.parameters.getRawParameterValue ("persistence")) * level;

                for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample (i, startSample, currentSample);

                currentPhase += phaseDelta;
                ++startSample;
            }
        }
    }
}
