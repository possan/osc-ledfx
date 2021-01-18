#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <stdint.h>
#include <stdbool.h>
#include "fx.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t time; // time offset in milliseconds
    int32_t newvalue; // number of leds
    uint8_t property;
    uint8_t flags;
    uint8_t dummy1;
    uint8_t dummy2;
} FxKeyframe;

typedef struct {
    uint32_t duration; // time offset in milliseconds
    uint32_t used_keyframes;
    uint32_t allocated_keyframes;
    FxKeyframe *keyframes;
} FxPlayer;


void fx_player_init(FxPlayer *fx, uint8_t *fixedmemory, uint32_t memorysize);
void fx_player_free(FxPlayer *fx);

void fx_player_clear_animation(FxPlayer *fx);
bool fx_player_load_frame(FxPlayer *fx, uint8_t *data, uint32_t length);
bool fx_player_load_animation(FxPlayer *fx, uint8_t *data, uint32_t length);
bool fx_player_add_keyframe(FxPlayer *player, uint32_t time, uint8_t prop, int32_t value, uint8_t flags);

void fx_player_prepare_frame(FxPlayer *fx, uint32_t time, FxSettings *target);

void _fx_player_find_keyframes(FxPlayer *player, uint32_t time, uint32_t seekOrigin, uint8_t prop, FxKeyframe **left, FxKeyframe **right);
void _fx_player_find_and_handle_keyframes(FxPlayer *player, uint32_t time, uint32_t seekOrigin, uint8_t prop, FxSettings *fx);
uint32_t _fx_player_find_seek_origin(FxPlayer *player, uint32_t time);

#ifdef	__cplusplus
}
#endif

#endif
