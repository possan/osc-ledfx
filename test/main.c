#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "../lib/fx.h"
#include "../lib/player.h"

#define ESC "\x1e"

uint8_t playermemory[123456] = { 0xFF, };
uint8_t ledbuffer[3*60] = { 0x00, };
uint8_t tempbuffer[3*60] = { 0x00, };

int main() {
    uint8_t rgb[100 * 3];
    uint32_t time = 0;

    FxSettings fx;
    FxPlayer player;

    fx_renderer_reset(&fx);
    fx_player_init(&player, &playermemory, 123456);

    fx_set_id_property(&fx, GLOB_NUM_LEDS, 60);
    fx_set_id_property(&fx, GLOB_PIXELORDER, 0);
    fx_set_id_property(&fx, GLOB_OPACITY, 100);
    fx_set_id_property(&fx, GLOB_BASE_SPEED, 1000);

    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_OPACITY, 100);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_OFFSET, 0);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_COLOR_R, 180);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_COLOR_G, 255);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_COLOR_B, 255);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_COLORSPACE, 1);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_SIZE, 40);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_REPEAT, 1000);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_FEATHER_LEFT, 30);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_FEATHER_RIGHT, 30);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_SPEED_MULTIPLIER, 1500);
    fx_set_id_property(&fx, LAYER1_OFFSET + LAYER_BLEND, 0);

    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_OPACITY, 100);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_OFFSET, 0);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_COLOR_R, 0);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_COLOR_G, 0);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_COLOR_B, 0);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_COLORSPACE, 0);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_SIZE, 20);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_REPEAT, 1000);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_FEATHER_LEFT, 0);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_FEATHER_RIGHT, 0);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_SPEED_MULTIPLIER, -2000);
    fx_set_id_property(&fx, LAYER2_OFFSET + LAYER_BLEND, 1);

    fx_set_id_property(&fx, LAYER3_OFFSET + LAYER_OPACITY, 100);
    fx_set_id_property(&fx, LAYER3_OFFSET + LAYER_OFFSET, 0);
    fx_set_id_property(&fx, LAYER3_OFFSET + LAYER_COLOR_R, 0);
    fx_set_id_property(&fx, LAYER3_OFFSET + LAYER_COLOR_G, 0);
    fx_set_id_property(&fx, LAYER3_OFFSET + LAYER_COLOR_B, 255);
    fx_set_id_property(&fx, LAYER3_OFFSET + LAYER_COLORSPACE, 1);
    fx_set_id_property(&fx, LAYER3_OFFSET + LAYER_SIZE, 10);
    fx_set_id_property(&fx, LAYER3_OFFSET + LAYER_REPEAT, 4000);
    fx_set_id_property(&fx, LAYER3_OFFSET + LAYER_FEATHER_LEFT, 15);
    fx_set_id_property(&fx, LAYER3_OFFSET + LAYER_FEATHER_RIGHT, 15);
    fx_set_id_property(&fx, LAYER3_OFFSET + LAYER_SPEED_MULTIPLIER, 3000);
    fx_set_id_property(&fx, LAYER3_OFFSET + LAYER_BLEND, 0);

    uint32_t T = 0;

    // printf("Without animation\n");

    // for(int fr=0; fr<=20; fr++) {
    //     printf("frame %4d (T %4d) [", fr, T);
    //     fx_render(&fx, T, &ledbuffer, 20, &tempbuffer);
    //     for(int p=0; p<20; p++) {
    //         if (p > 0) {
    //             printf(" ");
    //         }
    //         printf("%02X%02X%02X", ledbuffer[p*3+0], ledbuffer[p*3+1], ledbuffer[p*3+2]);
    //     }
    //     printf("]\n");
    //     printf("\n");
    //     printf("\n");
    //     // printf("\n");
    //     T += 100;
    // }

    // printf("With loaded animation\n");

    // Fade down l1 red
    fx_player_add_keyframe(&player, 0, LAYER2_OFFSET + LAYER_COLOR_R, 255, 0);
    fx_player_add_keyframe(&player, 0,  + LAYER_COLOR_R, 0, 0);
    fx_player_add_keyframe(&player, 100, LAYER2_OFFSET + LAYER_REPEAT, 1000, 0);
    fx_player_add_keyframe(&player, 200, LAYER2_OFFSET + LAYER_COLOR_B, 0, 0);
    fx_player_add_keyframe(&player, 300, LAYER2_OFFSET + LAYER_SPEED_MULTIPLIER, 0, 0);
    // fx_player_add_keyframe(&player, 500, LAYER2_OFFSET + LAYER_COLOR_R, 255, 0);
    fx_player_add_keyframe(&player, 500, LAYER2_OFFSET + LAYER_FEATHER_LEFT, 30, 0);
    fx_player_add_keyframe(&player, 500, LAYER2_OFFSET + LAYER_FEATHER_RIGHT, 30, 0);
    // fx_player_add_keyframe(&player, 500, LAYER2_OFFSET + LAYER_SPEED_MULTIPLIER, -5000, 0);
    // fx_player_add_keyframe(&player, 1000, LAYER2_OFFSET + LAYER_SIZE, 20, 0);
    // fx_player_add_keyframe(&player, 2000, LAYER2_OFFSET + LAYER_SIZE, 50, 0);
    fx_player_add_keyframe(&player, 3000, LAYER2_OFFSET + LAYER_SPEED_MULTIPLIER, -5000, 0);
    // fx_player_add_keyframe(&player, 5000, LAYER2_OFFSET + LAYER_COLOR_R, 0, 0);
    fx_player_add_keyframe(&player, 6000, LAYER2_OFFSET + LAYER_FEATHER_LEFT, 10, 0);
    fx_player_add_keyframe(&player, 6000, LAYER2_OFFSET + LAYER_FEATHER_RIGHT, 10, 0);
    fx_player_add_keyframe(&player, 7000, LAYER2_OFFSET + LAYER_COLOR_B, 255, 0);
    // fx_player_add_keyframe(&player, 7000, LAYER2_OFFSET + LAYER_COLOR_R, 0, 0);
    fx_player_add_keyframe(&player, 8000, LAYER1_OFFSET + LAYER_COLOR_R, 720, 0);
    fx_player_add_keyframe(&player, 8000, LAYER2_OFFSET + LAYER_SPEED_MULTIPLIER, 10000, 0);
    fx_player_add_keyframe(&player, 9000, LAYER2_OFFSET + LAYER_REPEAT, 2000, 0);
    // fx_player_add_keyframe(&player, 10000, LAYER2_OFFSET + LAYER_SIZE, 10, 0);

    // fx_player_add_keyframe(&player, 0, LAYER2_OFFSET + LAYER_COLOR_B, 255, 0);
    // fx_player_add_keyframe(&player, 500, LAYER2_OFFSET + LAYER_COLOR_R, 0, 0);
    // fx_player_add_keyframe(&player, 1000, LAYER2_OFFSET + LAYER_COLOR_B, 0, 0);
    // fx_player_add_keyframe(&player, 1300, LAYER1_OFFSET + LAYER_COLOR_R, 0, 0);
    // fx_player_add_keyframe(&player, 1600, LAYER1_OFFSET + LAYER_COLOR_R, 100, 0);
    // fx_player_add_keyframe(&player, 1700, LAYER2_OFFSET + LAYER_COLOR_R, 255, 0);

    T = 0;
    for(int fr=0; fr<=100; fr++) {
        uint32_t dT = 70;
        fx_player_prepare_frame(&player, T, &fx);
        printf("\x1b[0mframe %4d (T %6d) [ ", fr, T);
        fx_renderer_render(&fx, T, &ledbuffer, 60, &tempbuffer, dT);
        for(int p=0; p<60; p++) {
            // if (p > 0) printf(" ");
            int r = ledbuffer[p * 3 + 0];
            int g = ledbuffer[p * 3 + 1];
            int b = ledbuffer[p * 3 + 2];
            r = (r * 5) / 255;
            g = (g * 5) / 255;
            b = (b * 5) / 255;
            int c = 16 + b + (g * 6) + (r * 36);
            printf("\x1b[38;5;%dm🞓", c);
            // printf("%1x%1x%1x", r,g,b);
            // printf("%02X%02X%02X", ledbuffer[p*3+0], ledbuffer[p*3+1], ledbuffer[p*3+2]);
        }
        printf("\x1b[0m");
        printf(" ]\n");
        // printf("\n");
        // printf("\n");
        // printf("\n");
        T += dT;
    }

}
