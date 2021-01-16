#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "../lib/fx.h"
#include "../lib/player.h"

uint8_t playermemory[123456] = { 0xFF, };
uint8_t ledbuffer[3*60] = { 0x00, };
uint8_t tempbuffer[3*60] = { 0x00, };

int main() {
    uint8_t rgb[100 * 3];
    uint32_t time = 0;

    FxSettings fx;
    FxPlayer player;

    fx_player_init(&player, &playermemory, 123456);

    fx_set_id_property(&fx, GLOB_NUM_LEDS, 20);
    fx_set_id_property(&fx, GLOB_PIXELORDER, 0);
    fx_set_id_property(&fx, GLOB_OPACITY, 100);
    fx_set_id_property(&fx, GLOB_BASE_SPEED, 1000);

    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_OPACITY, 100);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_OFFSET, 0);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_COLOR_R, 255);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_COLOR_G, 0);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_COLOR_B, 0);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_COLORSPACE, 0);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_SIZE, 20);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_REPEAT, 1);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_FEATHER_LEFT, 20);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_FEATHER_RIGHT, 20);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_SPEED_MULTIPLIER, 3000);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_BLEND, 0);

    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_OPACITY, 100);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_OFFSET, 0);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_COLOR_R, 0);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_COLOR_G, 0);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_COLOR_B, 255);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_COLORSPACE, 0);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_SIZE, 30);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_REPEAT, 4);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_FEATHER_LEFT, 10);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_FEATHER_RIGHT, 10);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_SPEED_MULTIPLIER, -2000);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_BLEND, 0);

    uint32_t T = 0;

    printf("Without animation\n");

    for(int fr=0; fr<=20; fr++) {
        printf("frame %4d (T %4d) [", fr, T);
        fx_render(&fx, T, &ledbuffer, 20, &tempbuffer);
        for(int p=0; p<20; p++) {
            if (p > 0) {
                printf(" ");
            }
            printf("%02X%02X%02X", ledbuffer[p*3+0], ledbuffer[p*3+1], ledbuffer[p*3+2]);
        }
        printf("]\n");
        printf("\n");
        printf("\n");
        // printf("\n");
        T += 100;
    }

    printf("With loaded animation\n");

    // Fade down l1 red
    fx_player_add_keyframe(&player, 0, LAYER1_OFFSET + LAYER_COLOR_R, 255, 0);
    fx_player_add_keyframe(&player, 0, LAYER2_OFFSET + LAYER_COLOR_B, 255, 0);
    fx_player_add_keyframe(&player, 500, LAYER2_OFFSET + LAYER_COLOR_R, 0, 0);
    fx_player_add_keyframe(&player, 1000, LAYER2_OFFSET + LAYER_COLOR_B, 0, 0);
    fx_player_add_keyframe(&player, 1300, LAYER1_OFFSET + LAYER_COLOR_R, 0, 0);
    fx_player_add_keyframe(&player, 1600, LAYER1_OFFSET + LAYER_COLOR_R, 100, 0);
    fx_player_add_keyframe(&player, 1700, LAYER2_OFFSET + LAYER_COLOR_R, 255, 0);

    T = 0;
    for(int fr=0; fr<=20; fr++) {
        fx_player_prepare_frame(&player, T, &fx);
        printf("frame %4d (T %4d) [", fr, T);
        fx_render(&fx, T, &ledbuffer, 20, &tempbuffer);
        for(int p=0; p<20; p++) {
            if (p > 0) {
                printf(" ");
            }
            printf("%02X%02X%02X", ledbuffer[p*3+0], ledbuffer[p*3+1], ledbuffer[p*3+2]);
        }
        printf("]\n");
        // printf("\n");
        printf("\n");
        printf("\n");
        T += 100;
    }

}
