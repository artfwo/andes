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

#include "Noise.h"

Noise::Noise()
{
    setSeed (0x740fb143);
}

Noise::~Noise()
{
}

uint32_t Noise::getSeed()
{
    return seed_;
}

void Noise::setSeed (uint32_t seed)
{
    seed_ = seed;
    random.seed(seed);
    float randomMultiplier = 1.0f / random.max();

    for (int z = 0; z < 65536; ++z)
    {
        gradients_00[z] = random() * randomMultiplier * 2.0f - 1.0f;
        gradients_01[z] = random() * randomMultiplier * 2.0f - 1.0f;
        gradients_10[z] = random() * randomMultiplier * 2.0f - 1.0f;
        gradients_11[z] = random() * randomMultiplier * 2.0f - 1.0f;
    }
}

static inline float lerp (float a0, float a1, float w)
{
    return (1.0f - w) * a0 + w * a1;
}

float Noise::gen1 (float x, float y, float z)
{
    float g1, g2, d2;
    float result = 0;

    int z1 = (int) z;
    int z2 = (z1 + 1);

    float dz1 = z - z1; 
    float dz2 = dz1 - 1.0f;

    g1 = lerp(lerp(gradients_00[z1], gradients_01[z1], x),
              lerp(gradients_10[z1], gradients_11[z1], x), y);
    g2 = lerp(lerp(gradients_00[z2], gradients_01[z2], x),
              lerp(gradients_10[z2], gradients_11[z2], x), y);

    d2 = 1.0 - (dz1 * dz1);
    d2 = d2 * d2 * d2 * d2;
    result += d2 * (g1 * dz1);

    d2 = 1.0 - (dz2 * dz2);
    d2 = d2 * d2 * d2 * d2;
    result += d2 * (g2 * dz2);

    return result * 3.33; // temp normalization value
}

float Noise::gen (float x, float y, float z, int octaves, float persistence)
{
    float result = 0;
    float multiplier = 1.0f;

    for (int octave = 0; octave < octaves; ++octave)
    {
        result += gen1 (x, y, z  * (1 << octave)) * multiplier;
        multiplier *= persistence;
    }

    return result;
}
