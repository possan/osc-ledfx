#ifndef __FX_H__
#define __FX_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define GLOB_NUM_LEDS 1
#define GLOB_PIXELORDER 2
#define GLOB_NUDGE 3
#define GLOB_OPACITY 4
#define GLOB_BASE_SPEED 5

#define LAYER1_OFFSET 0
#define LAYER2_OFFSET 50
#define LAYER3_OFFSET 100
#define LAYER4_OFFSET 150

#define LAYER_OPACITY 10
#define LAYER_OFFSET 11
#define LAYER_SIZE 12
#define LAYER_REPEAT 13
#define LAYER_FEATHER_LEFT 14
#define LAYER_FEATHER_RIGHT 15
#define LAYER_SPEED_MULTIPLIER 16
#define LAYER_BLEND 17

#define LAYER_COLOR_R 20
#define LAYER_COLOR_G 21
#define LAYER_COLOR_B 22
#define LAYER_COLORSPACE 23






typedef struct FxLayerSettings {
    int32_t opacity; // percent
    int32_t offset; // offset in percent * 1000
    int32_t color[3]; // rgb value, 0-255
    int32_t colorspace; // 0 = RGB, 1 = HSV
    int32_t size; // width in percent
    int32_t feather_left; // width in percent
    int32_t feather_right; // width in percent
    int32_t speed_multiplier; // base speed multiplier * 1000
    int32_t repeat; // repeats, absolute value, 1-10
    int32_t gamma; // in percent 0-200
    int32_t blend; // 0 = add, 1 = subtract
} FxLayerSettings;

typedef struct FxSettings {
    int32_t time_offset; // time offset in milliseconds
    int32_t num_leds; // number of leds
    int32_t opacity; // percent
    int32_t pixel_order; // indexed, 0-5
    int32_t base_speed; // rpm * 1000
    int32_t test_pattern; // indexed, 0-1
    FxLayerSettings layer[4];
} FxSettings;

void fx_render(FxSettings *fx, uint32_t time, uint8_t *rgb, int max_leds, uint8_t *temp);

#ifdef WITH_OSC
bool fx_set_osc_property(FxSettings *fx, char *property, uint32_t value);
#endif
#ifdef WITH_JSON
void fx_get_config_json(FxSettings *fx, char *destination, uint32_t maxsize);
#endif
#ifdef WITH_PROPS
bool fx_set_id_property(FxSettings *fx, uint8_t property, uint32_t value);
#endif
#ifdef	__cplusplus
}
#endif

#endif