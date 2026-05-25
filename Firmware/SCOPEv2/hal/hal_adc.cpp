#include "hal_adc.h"

#include <avr/interrupt.h>

#include "../core/scope_config.h"
#include "../core/scope_state.h"

ISR(ADC_vect)
{
    if (!adcSampling)
        return;
    if (adcDelayCounter < adcDelayTarget)
    {
        adcDelayCounter++;
        return;
    }
    adcDelayCounter = 0;
    if (adcSampleIndex < adcTargetSamples)
    {
        buffer[adcSampleIndex++] = ADCH;
        if (adcSampleIndex >= adcTargetSamples)
        {
            adcBufferReady = true;
            adcSampling = false;
            ADCSRA &= ~((1 << ADATE) | (1 << ADIE));
        }
    }
}

void startADCSampling(uint16_t numSamples, uint8_t prescaler, uint8_t delaySkip)
{
    cli();
    adcSampleIndex = 0;
    adcTargetSamples = min(numSamples, (uint16_t)ADC_BUFFER_SIZE);
    adcBufferReady = false;
    adcSampling = true;
    adcDelayCounter = 0;
    adcDelayTarget = delaySkip;
    ADMUX = (1 << REFS0) | (1 << ADLAR) | (ANALOG_INPUT_PIN & 0x07);
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE) | (prescaler & 0x07);
    ADCSRB = 0;
    sei();
}

void stopADCSampling()
{
    ADCSRA &= ~((1 << ADIE) | (1 << ADATE));
    adcSampling = false;
}

uint8_t fastAnalogRead()
{
    ADMUX = (1 << REFS0) | (1 << ADLAR) | (ANALOG_INPUT_PIN & 0x07);
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (0x05);
    while (ADCSRA & (1 << ADSC))
    {
    }
    return ADCH;
}
