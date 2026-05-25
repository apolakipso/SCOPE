#include "dsp_tuner.h"

#include "../core/scope_state.h"

static const uint16_t noteFreqO4x10[] PROGMEM = {
    2616, 2772, 2937, 3111, 3296, 3492, 3700, 3920, 4153, 4400, 4662, 4939};
static const char noteNames[] PROGMEM = "C C#D D#E F F#G G#A A#B ";

void frequencyToNote(uint16_t freqX10, char *note, int8_t *octave, int8_t *cents)
{
    uint16_t fx10 = freqX10;
    int8_t oct = 4;

    while (fx10 >= 5232 && oct < 9)
    {
        fx10 = (fx10 + 1) >> 1;
        oct++;
    }
    while (fx10 < 2616 && oct > 0)
    {
        fx10 <<= 1;
        oct--;
    }

    uint8_t bestNote = 0;
    int16_t bestDiff = 32767;
    for (uint8_t i = 0; i < 12; i++)
    {
        uint16_t nf = pgm_read_word(&noteFreqO4x10[i]);
        int16_t diff = (int16_t)fx10 - (int16_t)nf;
        int16_t absDiff = diff < 0 ? -diff : diff;
        if (absDiff < (bestDiff < 0 ? -bestDiff : bestDiff))
        {
            bestDiff = diff;
            bestNote = i;
        }
    }

    if (bestNote == 11)
    {
        int16_t cNext = 5232;
        int16_t diffC = (int16_t)fx10 - (int16_t)cNext;
        if ((diffC < 0 ? -diffC : diffC) < (bestDiff < 0 ? -bestDiff : bestDiff))
        {
            bestNote = 0;
            oct++;
            bestDiff = diffC;
        }
    }

    *octave = oct;

    uint16_t nf = pgm_read_word(&noteFreqO4x10[bestNote]);
    *cents = (int8_t)(((int32_t)bestDiff * 1731L) / (int32_t)nf / 10);
    *cents = constrain(*cents, -50, 50);

    uint8_t idx = bestNote * 2;
    note[0] = pgm_read_byte(&noteNames[idx]);
    note[1] = pgm_read_byte(&noteNames[idx + 1]);
    note[2] = '\0';
    if (note[1] == ' ')
        note[1] = '\0';
}

float detectFrequencyZC()
{
    uint8_t minV = 255, maxV = 0;
    for (uint16_t i = 0; i < 256; i++)
    {
        if (buffer[i] < minV)
            minV = buffer[i];
        if (buffer[i] > maxV)
            maxV = buffer[i];
    }
    uint8_t range = maxV - minV;
    if (range < 30)
        return 0;

    uint8_t center = (minV + maxV) / 2;
    uint8_t hyst = range / 6;
    if (hyst < 5)
        hyst = 5;

    uint8_t crossings = 0;
    uint16_t first = 0, last = 0;
    bool above = buffer[0] > center;

    for (uint16_t i = 1; i < 256; i++)
    {
        if (!above && buffer[i] > center + hyst)
        {
            above = true;
            crossings++;
            if (!first)
                first = i;
            last = i;
        }
        else if (above && buffer[i] < center - hyst)
        {
            above = false;
        }
    }

    if (crossings < 2)
        return 0;
    return sampleRateHz / ((float)(last - first) / (float)(crossings - 1));
}
