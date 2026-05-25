#include "dsp_generator.h"

const uint16_t genFreqTableX10[] PROGMEM = {
    1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000};

const char genWaveNames[][4] PROGMEM = {"SIN", "TRI", "SAW", "SQR", "DC "};

static const uint8_t sineQ[] PROGMEM = {
    128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 162, 165, 167, 170, 173,
    176, 179, 182, 184, 187, 190, 192, 195, 197, 200, 202, 204, 207, 209, 211, 213,
    215, 217, 219, 221, 223, 224, 226, 228, 229, 231, 232, 233, 234, 236, 237, 238,
    239, 240, 240, 241, 242, 242, 243, 243, 244, 244, 244, 245, 245, 245, 245, 245};

uint8_t sineWave(uint8_t idx)
{
    if (idx < 64)
        return pgm_read_byte(&sineQ[idx]);
    if (idx < 128)
        return pgm_read_byte(&sineQ[127 - idx]);
    if (idx < 192)
        return 255 - pgm_read_byte(&sineQ[idx - 128]);
    return 255 - pgm_read_byte(&sineQ[255 - idx]);
}

uint8_t generateSample(uint8_t waveform, uint8_t idx)
{
    switch (waveform)
    {
    case 1:
        return sineWave(idx);
    case 2:
        return (idx < 128) ? idx * 2 : (255 - idx) * 2;
    case 3:
        return idx;
    case 4:
        return (idx < 128) ? 255 : 0;
    default:
        return 128;
    }
}

uint32_t calcPhaseInc(uint8_t freqIdx, uint32_t sr)
{
    freqIdx = constrain(freqIdx, 1, GEN_NUM_FREQS);
    uint16_t fX10 = pgm_read_word(&genFreqTableX10[freqIdx - 1]);
    return (uint32_t)((uint64_t)fX10 * 429496730ULL / sr);
}
