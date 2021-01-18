#include <stdio.h>
#include <string.h>
#include <emscripten/emscripten.h>
#include "../lib/fx.h"
#include "../lib/player.h"

int main() {
    printf("Hello World\n");
}

uint8_t playermemory[123456] = { 0xFF, };
uint8_t ledbuffer[3*1000] = { 0x00, };
uint8_t tempbuffer[3*1000] = { 0x00, };
FxSettings fx;
FxPlayer player;

EMSCRIPTEN_KEEPALIVE void fx_binding_reset() {
    // printf("fx reset\n");
    fx_renderer_reset(&fx);
    fx_player_init(&player, &playermemory, 123456);
}

        // EMSCRIPTEN_KEEPALIVE void fx_binding_seek(uint32_t time) {
        //     // printf("fx reset\n");
        //     fx_renderer_reset(&fx);
        //     fx_player_init(&player, &playermemory, 123456);
        // }

EMSCRIPTEN_KEEPALIVE void fx_binding_load_animation(uint8_t *animptr, uint32_t len) {
    // printf("fx set animation\n");
}

EMSCRIPTEN_KEEPALIVE void fx_binding_set_property(uint8_t property, uint32_t value) {
    // printf("fx set prop %d = %d\n", property, value);
    fx_set_id_property(&fx, property, value);
}

EMSCRIPTEN_KEEPALIVE void fx_binding_render(uint32_t time, uint32_t deltatime, uint8_t *ledptr, uint32_t ledcount) {
    // printf("fx render %d\n", time, deltatime);
    fx_renderer_render(&fx, time, &ledbuffer, ledcount, &tempbuffer, deltatime);
    memcpy(ledptr, &ledbuffer, ledcount * 3);
}



