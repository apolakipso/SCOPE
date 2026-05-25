#include "core/scope_state.cpp"

#include "hal/hal_adc.cpp"
#include "hal/hal_dac.cpp"
#include "hal/hal_display.cpp"
#include "hal/hal_hw.cpp"

#include "dsp/dsp_generator.cpp"
#include "dsp/dsp_tuner.cpp"
#include "dsp/util_format.cpp"

#include "app/storage_settings.cpp"
#include "app/app_mode_manager.cpp"

#include "ui/ui_config_menu.cpp"
#include "ui/ui_controls.cpp"
#include "ui/ui_parameter_bar.cpp"

#include "mode/mode_lfo.cpp"
#include "mode/mode_wave.cpp"
#include "mode/mode_tuner.cpp"
#include "mode/mode_gen.cpp"