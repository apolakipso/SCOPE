#include "util_format.h"

#include <string.h>

char *fmtInt(char *buf, int16_t val)
{
    if (val < 0)
    {
        *buf++ = '-';
        val = -val;
    }

    char tmp[6];
    uint8_t n = 0;
    if (val == 0)
    {
        tmp[n++] = '0';
    }
    else
    {
        while (val > 0)
        {
            tmp[n++] = '0' + (val % 10);
            val /= 10;
        }
    }

    for (uint8_t i = n; i > 0; i--)
    {
        *buf++ = tmp[i - 1];
    }
    *buf = '\0';
    return buf;
}

char *fmtDec1(char *buf, int16_t valX10)
{
    if (valX10 < 0)
    {
        *buf++ = '-';
        valX10 = -valX10;
    }
    char *end = fmtInt(buf, valX10 / 10);
    *end++ = '.';
    *end++ = '0' + (valX10 % 10);
    *end = '\0';
    return end;
}

void fmtDec2(char *buf, uint16_t vx100)
{
    char *p = fmtInt(buf, vx100 / 100);
    *p++ = '.';
    uint8_t frac = vx100 % 100;
    *p++ = '0' + frac / 10;
    *p++ = '0' + frac % 10;
    *p++ = '\0';
}

void fmtFreq(char *buf, uint16_t freqX10)
{
    if (freqX10 < 1000)
    {
        fmtDec1(buf, freqX10);
    }
    else
    {
        fmtInt(buf, freqX10 / 10);
    }

    char *p = buf + strlen(buf);
    *p++ = 'H';
    *p++ = 'z';
    *p = '\0';
}
