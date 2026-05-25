#include "scope_state.h"

ScopeState gState = {
    nullptr,
    false,
    nullptr,
    0,
    128,
    false,
    false,
    0,
    0,
    0.0f,
    2,
    0.0f,
    0,
    0,
    false,
    0,
    0,
    false,
    false,
    0,
    0,
    128,
    25000UL,
    40,
    {0},
    {{0, 0, 0}},
    MODE_LFO,
    MODE_LFO,
    0,
    1,
    2,
    1,
    false,
    false,
    false,
    false,
    0,
    false,
    0,
    -999.0f,
    -999.0f,
    false,
    1,
    5,
    1,
    0};

Adafruit_SSD1306 *&display = gState.display;
bool &isHWv25 = gState.isHWv25;
Encoder *&encoder = gState.encoder;

volatile uint16_t &adcSampleIndex = gState.adcSampleIndex;
volatile uint16_t &adcTargetSamples = gState.adcTargetSamples;
volatile bool &adcBufferReady = gState.adcBufferReady;
volatile bool &adcSampling = gState.adcSampling;
volatile uint8_t &adcDelayCounter = gState.adcDelayCounter;
volatile uint8_t &adcDelayTarget = gState.adcDelayTarget;

float &smoothedFrequency = gState.smoothedFrequency;
uint8_t &tunerSampleRate = gState.tunerSampleRate;
float &sampleRateHz = gState.sampleRateHz;
uint8_t &waveState = gState.waveState;
uint8_t &tunerState = gState.tunerState;

bool &isLGT8F = gState.isLGT8F;

uint32_t &genPhase = gState.genPhase;
uint32_t &genPhaseInc = gState.genPhaseInc;
bool &genAvailable = gState.genAvailable;
bool &dacIsI2C = gState.dacIsI2C;
uint8_t &dacI2CAddr = gState.dacI2CAddr;
int8_t &calOffset = gState.calOffset;
uint8_t &calGain = gState.calGain;
uint32_t &genSampleRate = gState.genSampleRate;
uint16_t &genSamplePeriodUs = gState.genSamplePeriodUs;

uint8_t (&buffer)[ADC_BUFFER_SIZE] = gState.buffer;
ModeSettings (&modeSettings)[NUM_MODES] = gState.modeSettings;

uint8_t &mode = gState.mode;
uint8_t &old_mode = gState.old_mode;
uint8_t &param_select = gState.param_select;
uint8_t &param = gState.param;
uint8_t &param1 = gState.param1;
uint8_t &param2 = gState.param2;
bool &trig = gState.trig;
bool &old_trig = gState.old_trig;
bool &SW = gState.SW;
bool &old_SW = gState.old_SW;
unsigned long &hideTimer = gState.hideTimer;
bool &hide = gState.hide;
int &rfrs = gState.rfrs;
float &oldPosition = gState.oldPosition;
float &newPosition = gState.newPosition;
bool &configMenuActive = gState.configMenuActive;
byte &configMenuOption = gState.configMenuOption;
unsigned int &menuTimer = gState.menuTimer;
int &encoderDirection = gState.encoderDirection;
uint8_t &oledRotation = gState.oledRotation;
