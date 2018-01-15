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
    setSeed (0x568fd34e);
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
        gradients[z] = random() * randomMultiplier * 2.0f - 1.0f;
    }
}

static inline float lerp (float a0, float a1, float w)
{
    return (1.0f - w) * a0 + w * a1;
}

inline float gen1 (float *gradients, float z, float torsion)
{
    const int z1 = static_cast<int>(z);
    const float dz1 = z - z1;
    const float dz2 = dz1 - 1.0f;

    float g1 = gradients[z1] + torsion;
    float g2 = gradients[z1 + 1] + torsion;

    float d1 = 1.0f - (dz1 * dz1);
    d1 = d1 * d1 * d1 * d1;

    float d2 = 1.0f - (dz2 * dz2);
    d2 = d2 * d2 * d2 * d2;

    if (g1 > 1.0f) { g1 = 2.0f - g1; }
    else if (g1 < -1.0f) { g1 = -2.0f - g1; }

    if (g2 > 1.0f) { g2 = 2.0f - g2; }
    else if (g2 < -1.0f) { g2 = -2.0f - g2; }

    return (d1 * (g1 * dz1) + d2 * (g2 * dz2)) * 3.33f; // temp normalization value
}

float Noise::gen (float z, int octaves, float persistence, float torsion, float warping)
{
    float result = 0;
    float multiplier = 1.0f;
    float offset = gen1 (gradients, z, torsion) * warping;

    for (int octave = 0; octave < octaves; ++octave)
    {
        float t = fmodf(torsion, 4);
        float value = gen1 (gradients, z * (1 << octave) + offset, t) * multiplier;
        offset = value * warping;
        result += value;
        multiplier *= persistence;
    }

    return result;
}
