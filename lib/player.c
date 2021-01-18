#include "fx.h"
#include "player.h"
#include <stdio.h>
#include <string.h>
#include <math.h>


void fx_player_init(FxPlayer *player, uint8_t *fixedmemory, uint32_t memorysize) {
    player->duration = 0;
    player->keyframes = (FxKeyframe *)fixedmemory;
    player->allocated_keyframes = floor(memorysize / sizeof(FxKeyframe));
    memset(fixedmemory, 0, memorysize);
    player->used_keyframes = 0;
}

void fx_player_free(FxPlayer *player) {
    player->keyframes = NULL;
    player->duration = 0;
    player->used_keyframes = 0;
    player->allocated_keyframes = 0;
}

void fx_player_clear_animation(FxPlayer *player) {
    player->duration = 0;
    player->used_keyframes = 0;
}

bool fx_player_load_frame(FxPlayer *player, uint8_t *data, uint32_t length) {
    uint32_t nkf = floor(length / sizeof(FxKeyframe));
    memset(player->keyframes, 0, sizeof(FxKeyframe) * player->allocated_keyframes);
    return false;
}

bool fx_player_add_keyframe(FxPlayer *player, uint32_t time, uint8_t prop, int32_t value, uint8_t flags) {
    if (player->used_keyframes >= player->allocated_keyframes) {
        #ifdef WITH_DEBUG
        printf("Out of memory.\n");
        #endif
        return false;
    }

    FxKeyframe *nextkf = (FxKeyframe *)(player->keyframes + player->used_keyframes);
    nextkf->time = time;
    nextkf->property = prop;
    nextkf->newvalue = value;
    nextkf->flags = flags;

    player->used_keyframes ++;
    #ifdef WITH_DEBUG
    printf("Added keyframe %d/%d ...\n", player->used_keyframes, player->allocated_keyframes);
    #endif

    return false;
}

bool fx_player_load_animation(FxPlayer *player, uint8_t *data, uint32_t length) {
    uint32_t nkf = floor(length / sizeof(FxKeyframe));
    memset(player->keyframes, 0, sizeof(FxKeyframe) * player->allocated_keyframes);
    memcpy(player->keyframes, data, sizeof(FxKeyframe) * nkf);
    player->used_keyframes = nkf;
    player->duration = 0;
    FxKeyframe *p = player->keyframes;
    while (p != NULL) {
        if (p->time > player->duration) {
            player->duration = p->time;
        }
        p ++;
    }
    return true;
}

uint32_t _fx_player_find_seek_origin(FxPlayer *player, uint32_t time) {
    uint32_t last_key_index = 0;
    uint32_t last_time = 0;

    // if (player->used_keyframes > 1) {
    for(uint32_t f=0; f<player->used_keyframes; f++) {
        FxKeyframe *kf = player->keyframes + f;

        if (kf->time > time) {
            break;
        }

        if (kf->time > last_time) {
            last_time = kf->time;
            last_key_index = f;
        }
    }
    // }

    return last_key_index;
}

void _fx_player_find_keyframes(FxPlayer *player, uint32_t time, uint32_t seekOrigin, uint8_t prop, FxKeyframe **left, FxKeyframe **right) {
    *left = NULL;
    *right = NULL;

    if (seekOrigin >= player->used_keyframes) {
        return;
    }

    for(int32_t f=seekOrigin; f>=0; f--) {
        FxKeyframe *kf = player->keyframes + f;

        if (kf->property != prop) {
            continue;
        }

        #ifdef WITH_DEBUG
        printf("Seeking back, found kf p=%d, f=%d, t=%d\n", prop, f, kf->time);
        #endif
        *left = kf;
        break;
    }

    for(uint32_t f=seekOrigin + 1; f<player->used_keyframes; f++) {
        FxKeyframe *kf = player->keyframes + f;

        if (kf->property != prop) {
            continue;
        }

        #ifdef WITH_DEBUG
        printf("Seeking forward found kf p=%d, f=%d, t=%d\n", prop, f, kf->time);
        #endif
        *right = kf;
        break;
    }

    // seek left
    // if (seekOrigin < player->used_keyframes) {
    //     for(int32_t f=seekOrigin; f>=0; f--) {
    //         // printf("Seeking back for last prop=%d, f=%d\n", prop, f);
    //         if (f < 0) {
    //             continue;
    //         }

    //         FxKeyframe *kf = player->keyframes + f;
    //         // printf("  t=%d p=%d v=%d\n", kf->time, kf->property, kf->newvalue);

    //         if (kf->property != prop) {
    //             continue;
    //         }

    //         *left = kf;
    //         // printf("Using ^ as left\n");
    //         break;
    //     }
    // }

    // // seek right
    // if (seekOrigin < player->used_keyframes) {
    //     for(uint32_t f=seekOrigin + 1; f<player->used_keyframes; f++) {
    //         // printf("Seeking forward for next prop=%d, f=%d\n", prop, f);
    //         FxKeyframe *kf = player->keyframes + f;
    //         // printf("  t=%d p=%d v=%d\n", kf->time, kf->property, kf->newvalue);

    //         if (kf->property != prop) {
    //             continue;
    //         }

    //         if (kf == *left) {
    //             continue;
    //         }

    //         *right = kf;
    //         // printf("Using ^ as right\n");
    //         break;
    //     }
    // }
}

void _fx_player_find_and_handle_keyframes(FxPlayer *player, uint32_t time, uint32_t seekOrigin, uint8_t prop, FxSettings *fx) {
    FxKeyframe *left = NULL;
    FxKeyframe *right = NULL;

    #ifdef WITH_DEBUG
    printf("_fx_player_find_and_handle_keyframes (t=%d, o=%d, p=%d)\n", time, seekOrigin, prop);
    #endif

    _fx_player_find_keyframes(player, time, seekOrigin, prop, &left, &right);

    // brute force

    // uint32_t lefttime = 0;
    // uint32_t leftindex = 0;
    // for(uint32_t f=0; f<player->used_keyframes; f++) {
    //     FxKeyframe *kf = player->keyframes + f;
    //     printf("  check fwd kf #%d t=%d p=%d v=%d\n", f, kf->time, kf->property, kf->newvalue);

    //     if (kf->property != prop) {
    //         continue;
    //     }

    //     if (kf->time > time) {
    //         continue;
    //     }

    //     printf("  possible fwd kf #%d t=%d p=%d v=%d\n", f, kf->time, kf->property, kf->newvalue);

    //     left = kf;
    //     lefttime = kf->time;
    //     leftindex = f;
    // }

    // if (player->used_keyframes > 0) {
    //     for(int32_t f=player->used_keyframes - 1; f>=0; f--) {
    //         FxKeyframe *kf = player->keyframes + f;
    //         printf("  check rev kf #%d t=%d p=%d v=%d\n", f, kf->time, kf->property, kf->newvalue);

    //         if (kf->property != prop) {
    //             continue;
    //         }

    //         if (kf->time < time) {
    //             continue;
    //         }

    //         if (kf == left) {
    //             continue;
    //         }

    //         printf("  possible rev kf #%d t=%d p=%d v=%d\n", f, kf->time, kf->property, kf->newvalue);

    //         right = kf;
    //     }
    // }

    // if (lastindex < player->used_keyframes - 1) {
    //     right = player->keyframes + lastindex + 1
    // }
    // _fx_player_find_keyframes(player, time, seekOrigin, prop, &left, &right);

    if (left == NULL && right == NULL) {
        #ifdef WITH_DEBUG
        printf("No keyframes.\n");
        #endif
        return;
    }

    if (left == NULL && right != NULL) {
        #ifdef WITH_DEBUG
        printf("Use right. (t=%d, p=%d, v=%d)\n", right->time, right->property, right->newvalue);
        #endif
        fx_set_id_property(fx, prop, right->newvalue);
        return;
    }

    if (left != NULL && right == NULL) {
        #ifdef WITH_DEBUG
        printf("Use left. (t=%d, p=%d, v=%d)\n", left->time, left->property, left->newvalue);
        #endif
        fx_set_id_property(fx, prop, left->newvalue);
        return;
    }

    // interpolate
    #ifdef WITH_DEBUG
    printf("Interpolate keyframes.\n");
    printf("  left. (t=%d, p=%d, v=%d)\n", left->time, left->property, left->newvalue);
    printf("  right. (t=%d, p=%d, v=%d)\n", right->time, right->property, right->newvalue);
    #endif

    int32_t deltatime = right->time - left->time;
    if (deltatime < 1) {
        return;
    }

    int32_t deltaoffset = time - left->time;
    #ifdef WITH_DEBUG
    printf("  delta offset %d\n", deltaoffset);
    #endif
    if (deltaoffset < 0) deltaoffset = 0;
    if (deltaoffset > deltatime) deltaoffset = deltatime;

    int32_t deltafrac = (deltaoffset * 1000) / deltatime;
    #ifdef WITH_DEBUG
    printf("  delta frac %f\n", deltafrac);
    #endif

	// float angle = a * D3DX_PI;
    float a = (float)deltafrac * 3.1425f / 1000.0f;
	deltafrac = (int)(500.0f - 500.0f * cos(a));

    int32_t invdeltafrac = 1000 - deltafrac;

    int32_t interp = ( (right->newvalue * deltafrac) / 1000 ) + ( (left->newvalue * invdeltafrac) / 1000 );
    #ifdef WITH_DEBUG
    printf("  interp %d\n", interp);
    #endif

    fx_set_id_property(fx, prop, interp);
}

void fx_player_prepare_frame(FxPlayer *player, uint32_t time, FxSettings *target) {
    #ifdef WITH_DEBUG
    printf("\nfx_player_prepare_frame, t=%d\n", time);
    #endif
    // printf("Checking %d keyframes..\n", player->used_keyframes);

    // Find last/next keyframes for each property
    uint32_t o = _fx_player_find_seek_origin(player, time);
    #ifdef WITH_DEBUG
    printf("Offset %d\n", o);
    #endif

    // First go through all globals
    _fx_player_find_and_handle_keyframes(player, time, o, GLOB_BASE_SPEED, target);
    _fx_player_find_and_handle_keyframes(player, time, o, GLOB_OPACITY, target);
    _fx_player_find_and_handle_keyframes(player, time, o, GLOB_NUDGE, target);

    // FxKeyframe *t;
    for(int l=0; l<3; l++) {
        uint8_t propbase = LAYER1_OFFSET;
        if (l == 1) propbase = LAYER2_OFFSET;
        if (l == 2) propbase = LAYER3_OFFSET;

        _fx_player_find_and_handle_keyframes(player, time, o, propbase + LAYER_OPACITY, target);
        _fx_player_find_and_handle_keyframes(player, time, o, propbase + LAYER_OFFSET, target);

        _fx_player_find_and_handle_keyframes(player, time, o, propbase + LAYER_COLOR_R, target);
        _fx_player_find_and_handle_keyframes(player, time, o, propbase + LAYER_COLOR_G, target);
        _fx_player_find_and_handle_keyframes(player, time, o, propbase + LAYER_COLOR_B, target);
        _fx_player_find_and_handle_keyframes(player, time, o, propbase + LAYER_COLORSPACE, target);

        _fx_player_find_and_handle_keyframes(player, time, o, propbase + LAYER_SIZE, target);
        _fx_player_find_and_handle_keyframes(player, time, o, propbase + LAYER_REPEAT, target);
        _fx_player_find_and_handle_keyframes(player, time, o, propbase + LAYER_FEATHER_LEFT, target);
        _fx_player_find_and_handle_keyframes(player, time, o, propbase + LAYER_FEATHER_RIGHT, target);
        _fx_player_find_and_handle_keyframes(player, time, o, propbase + LAYER_SPEED_MULTIPLIER, target);
        _fx_player_find_and_handle_keyframes(player, time, o, propbase + LAYER_BLEND, target);
    }

    // Then go though all layers and their properties
    #ifdef WITH_DEBUG
    printf("\n");
    #endif
}
