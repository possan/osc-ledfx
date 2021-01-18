#include "fx.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

uint8_t brightness_at_position(FxLayerSettings *layer, FxSettings *fx, float time, int led, int num_leds, uint8_t *temp) {
    int32_t repeat = layer->repeat;
    if (repeat < 1) {
      repeat = 1;
    }

    uint32_t ramp_i =
      1024*1024 +
      (led * 1024) +
      ((time * layer->speed_multiplier / 1000) * num_leds * fx->base_speed / 60000) + // base_speed is in rpm * 1000

      // ((time * layer->speed_multiplier) / 16) +

      (((layer->offset * num_leds * 1024)) / 100000) // offset is in percent * 1000

      ;

    // uint32_t p = (ramp_i >> 10) % (num_leds / layer->repeat);

    ramp_i *= layer->repeat;
    ramp_i /= 1000;
    ramp_i &= 0xFFFFFFF;


    uint16_t f1 = ramp_i & 1023;
    uint16_t f0 = 1023 - f1;

    uint16_t o0 = ramp_i >> 10;
    uint16_t v0 = temp[(o0) % num_leds];
    uint16_t v1 = temp[(o0 + 1) % num_leds];

    uint16_t v = ((v0 * f0) + (v1 * f1)) / 1024;

    return v;

    // return p < layer->size ? 255 : 0;
}

void fx_render_layer(FxLayerSettings *layer, FxSettings *fx, uint32_t time, uint8_t *rgb, int num_leds, uint8_t *temp, int opacity, int32_t deltatime) {

    uint16_t finalopacity = (layer->opacity * opacity);

    layer->_local_time += deltatime * layer->speed_multiplier;

    if (finalopacity < 1) {
        return;
    }

    int32_t rr = 0;
    int32_t gg = 0;
    int32_t bb = 0;

    if (layer->colorspace == 1) {
      int32_t hh = layer->color[0];
      int32_t ss = layer->color[1];
      int32_t vv = layer->color[2];
      hh %= 360;
      if (ss < 0) ss = 0;
      if (ss > 255) ss = 255;
      if (vv < 0) vv = 0;
      if (vv > 255) vv = 255;

      int32_t j = (long)(hh / 60);
      int32_t ff = ((hh % 60) * 255) / 60;
      int32_t p = (vv * (256 - ss)) / 256;
      int32_t q = (vv * (256 - ((ss * ff) / 256))) / 256;
      int32_t t = (vv * (256 - ((ss * (256 - ff)) / 256))) / 256;

      switch(j) {
      case 0:
          rr = vv;
          gg = t;
          bb = p;
          break;
      case 1:
          rr = q;
          gg = vv;
          bb = p;
          break;
      case 2:
          rr = p;
          gg = vv;
          bb = t;
          break;

      case 3:
          rr = p;
          gg = q;
          bb = vv;
          break;
      case 4:
          rr = t;
          gg = p;
          bb = vv;
          break;
      case 5:
      default:
          rr = vv;
          gg = p;
          bb = q;
          break;
      }
    } else {
      rr = layer->color[0];
      gg = layer->color[1];
      bb = layer->color[2];
    }

    rr = (rr * finalopacity) / 10000;
    gg = (gg * finalopacity) / 10000;
    bb = (bb * finalopacity) / 10000;

    if (rr < 1 && gg < 1 && bb < 1) {
        return;
    }

    int o;

    memset(temp, 0, num_leds);

    // sizes are in percent * 1000
    int s1 = (layer->feather_left * num_leds) / 100;
    int s2 = (layer->size * num_leds) / 100;
    int s3 = (layer->feather_right * num_leds) / 100;

    o = 0;
    for(int j=0; j<num_leds; j++) {
      if (j < s1 && s1 > 0) {
        temp[o] = (j * 255) / s1;
      } else if(j < s1 + s2) {
        temp[o] = 255;
      } else if(j < s1 + s2 + s3 && s3 > 0) {
        temp[o] = 255 - ((j - (s1 + s2)) * 255) / s3;
      }
      o ++;
    }

    // printf("blending layer... %d leds, time %d, color %d %d %d: ", num_leds, time, rr, gg, bb);

    int16_t xr, xg, xb, lr, lg, lb;

    o = 0;
    for(int j=0; j<num_leds; j++) {

      int16_t bri = (int16_t) brightness_at_position(layer, fx, layer->_local_time / 1000, j, num_leds, temp);

      // printf("%3d ", bri);

      lr = (rr * bri) >> 8;
      lg = (gg * bri) >> 8;
      lb = (bb * bri) >> 8;

      xr = rgb[o];
      xg = rgb[o + 1];
      xb = rgb[o + 2];

      if (layer->blend == 1) { // subtract
        xr -= lr;
        xg -= lg;
        xb -= lb;
      } else if (layer->blend == 2) { // multiply
        xr = (xr * lr) >> 8;
        xg = (xg * lg) >> 8;
        xb = (xb * lb) >> 8;
      } else { // add
        xr += lr;
        xg += lg;
        xb += lb;
      }

      if (xr < 0) xr = 0;
      if (xg < 0) xg = 0;
      if (xb < 0) xb = 0;

      if (xr > 255) xr = 255;
      if (xg > 255) xg = 255;
      if (xb > 255) xb = 255;

      rgb[o] = xr;
      rgb[o + 1] = xg;
      rgb[o + 2] = xb;

      o += 3;
    }
}

void fx_renderer_reset(FxSettings *fx) {
  fx->_last_time = 0;
  fx->_global_time = 0;
  fx->layer[0]._local_time = 0;
  fx->layer[1]._local_time = 0;
  fx->layer[2]._local_time = 0;
  fx->layer[3]._local_time = 0;
}

void fx_renderer_render(FxSettings *fx, uint32_t time, uint8_t *rgb, int max_leds, uint8_t *temp, uint32_t deltatime) {
    memset(rgb, 0, max_leds * 3);

    uint32_t time2 = time + fx->time_offset;
    // int32_t deltatime = time - fx->_last_time;

    deltatime *= fx->base_speed;
    deltatime /= 1000;

    // printf("rendering... %d leds, time = %d\n", fx->num_leds, time2);
    fx->_last_time = time;

    if (fx->opacity < 1) {
        return;
    }

    fx_render_layer(&fx->layer[0], fx, time2, rgb, fx->num_leds, temp, fx->opacity, deltatime);
    fx_render_layer(&fx->layer[1], fx, time2, rgb, fx->num_leds, temp, fx->opacity, deltatime);
    fx_render_layer(&fx->layer[2], fx, time2, rgb, fx->num_leds, temp, fx->opacity, deltatime);
    // fx_render_layer(&fx->layer[3], fx, time2, rgb, fx->num_leds, temp, fx->opacity);

}

#ifdef WITH_OSC

bool fx_set_osc_property(FxSettings *fx, char *addr, int32_t value) {
  bool debug = true;

  if (strcmp(addr, "/length") == 0) {
    fx->num_leds = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Master num leds: %d\n", fx->num_leds);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/nudge") == 0) {
    fx->time_offset = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Master time offset: %d\n", fx->time_offset);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/opacity") == 0) {
    fx->opacity = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Master opacity: %d\n", fx->opacity);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/pixelorder") == 0) {
    fx->pixel_order = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Pixel order: %d\n", fx->pixel_order);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/basespeed") == 0) {
    fx->base_speed = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Base speed: %d\n", fx->base_speed);
    }
    #endif
    return true;
  }


  if (strcmp(addr, "/layer1/opacity") == 0) {
    fx->layer[0].opacity = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 1 opacity: %d\n", fx->layer[0].opacity);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer1/offset") == 0) {
    fx->layer[0].offset = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 1 offset: %d\n", fx->layer[0].offset);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer1/red") == 0) {
    fx->layer[0].color[0] = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 1 red: %d\n", fx->layer[0].color[0]);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer1/green") == 0) {
    fx->layer[0].color[1] = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 1 green: %d\n", fx->layer[0].color[1]);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer1/blue") == 0) {
    fx->layer[0].color[2] = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 1 blue: %d\n", fx->layer[0].color[2]);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer1/size") == 0) {
    fx->layer[0].size = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 1 size: %d\n", fx->layer[0].size);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer1/repeat") == 0) {
    fx->layer[0].repeat = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 1 repeat: %d\n", fx->layer[0].repeat);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer1/speed") == 0) {
    fx->layer[0].speed_multiplier = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 1 speed multiplier: %d\n", fx->layer[0].speed_multiplier);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer1/feather1") == 0) {
    fx->layer[0].feather_left = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 1 feather_left: %d\n", fx->layer[0].feather_left);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer1/feather2") == 0) {
    fx->layer[0].feather_right = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 1 feather_right: %d\n", fx->layer[0].feather_right);
    }
    #endif
    return true;
  }


  if (strcmp(addr, "/layer2/opacity") == 0) {
    fx->layer[1].opacity = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 2 opacity: %d\n", fx->layer[1].opacity);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer2/offset") == 0) {
    fx->layer[1].offset = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 2 offset: %d\n", fx->layer[1].offset);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer2/red") == 0) {
    fx->layer[1].color[0] = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 2 red: %d\n", fx->layer[1].color[0]);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer2/green") == 0) {
    fx->layer[1].color[1] = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 2 green: %d\n", fx->layer[1].color[1]);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer2/blue") == 0) {
    fx->layer[1].color[2] = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 2 blue: %d\n", fx->layer[1].color[2]);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer2/size") == 0) {
    fx->layer[1].size = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 2 size: %d\n", fx->layer[1].size);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer2/repeat") == 0) {
    fx->layer[1].repeat = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 2 repeat: %d\n", fx->layer[1].repeat);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer2/speed") == 0) {
    fx->layer[1].speed_multiplier = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 2 speed multiplier: %d\n", fx->layer[1].speed_multiplier);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer2/feather1") == 0) {
    fx->layer[1].feather_left = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 2 feather_left: %d\n", fx->layer[1].feather_left);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer2/feather2") == 0) {
    fx->layer[1].feather_right = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 2 feather_right: %d\n", fx->layer[1].feather_right);
    }
    #endif
    return true;
  }


  if (strcmp(addr, "/layer3/opacity") == 0) {
    fx->layer[2].opacity = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 3 opacity: %d\n", fx->layer[2].opacity);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer3/offset") == 0) {
    fx->layer[2].offset = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 3 offset: %d\n", fx->layer[2].offset);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer3/red") == 0) {
    fx->layer[2].color[0] = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 3 red: %d\n", fx->layer[2].color[0]);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer3/green") == 0) {
    fx->layer[2].color[1] = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 3 green: %d\n", fx->layer[2].color[1]);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer3/blue") == 0) {
    fx->layer[2].color[2] = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 3 blue: %d\n", fx->layer[2].color[2]);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer3/size") == 0) {
    fx->layer[2].size = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 3 size: %d\n", fx->layer[2].size);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer3/repeat") == 0) {
    fx->layer[2].repeat = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 3 repeat: %d\n", fx->layer[2].repeat);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer3/speed") == 0) {
    fx->layer[2].speed_multiplier = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 3 speed multiplier: %d\n", fx->layer[2].speed_multiplier);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer3/feather1") == 0) {
    fx->layer[2].feather_left = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 3 feather_left: %d\n", fx->layer[2].feather_left);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer3/feather2") == 0) {
    fx->layer[2].feather_right = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 3 feather_right: %d\n", fx->layer[2].feather_right);
    }
    #endif
    return true;
  }


  if (strcmp(addr, "/layer4/opacity") == 0) {
    fx->layer[3].opacity = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 4 opacity: %d\n", fx->layer[3].opacity);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer4/offset") == 0) {
    fx->layer[3].offset = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 4 offset: %d\n", fx->layer[3].offset);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer4/red") == 0) {
    fx->layer[3].color[0] = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 4 red: %d\n", fx->layer[3].color[0]);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer4/green") == 0) {
    fx->layer[3].color[1] = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 4 green: %d\n", fx->layer[3].color[1]);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer4/blue") == 0) {
    fx->layer[3].color[2] = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 4 blue: %d\n", fx->layer[3].color[2]);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer4/size") == 0) {
    fx->layer[3].size = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 4 size: %d\n", fx->layer[3].size);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer4/repeat") == 0) {
    fx->layer[3].repeat = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 4 repeat: %d\n", fx->layer[3].repeat);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer4/speed") == 0) {
    fx->layer[3].speed_multiplier = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 4 speed multiplier: %d\n", fx->layer[3].speed_multiplier);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer4/feather1") == 0) {
    fx->layer[3].feather_left = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 4 feather_left: %d\n", fx->layer[3].feather_left);
    }
    #endif
    return true;
  }

  if (strcmp(addr, "/layer4/feather2") == 0) {
    fx->layer[3].feather_right = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Layer 4 feather_right: %d\n", fx->layer[3].feather_right);
    }
    #endif
    return true;
  }

  return false;
}

#endif

#ifdef WITH_JSON

void fx_get_layer_config_json(FxLayerSettings *layer, char *destination, uint32_t maxsize) {
  sprintf(destination, "{"
    "\"op\":%d,"
    "\"of\":%d,"
    "\"r\":%d,"
    "\"g\":%d,"
    "\"b\":%d,"
    "\"siz\":%d,"
    "\"rep\":%d,"
    "\"spd\":%d,"
    "\"fe1\":%d,"
    "\"fe2\":%d"
    "}",
    layer->opacity,
    layer->offset,
    layer->color[0],
    layer->color[1],
    layer->color[2],
    layer->size,
    layer->repeat,
    layer->speed_multiplier,
    layer->feather_left,
    layer->feather_right);
}

void fx_get_config_json(FxSettings *fx, char *destination, uint32_t maxsize) {
  char l1buf[500];
  char l2buf[500];
  char l3buf[500];
  char l4buf[500];

  fx_get_layer_config_json(&fx->layer[0], &l1buf, 200);
  fx_get_layer_config_json(&fx->layer[1], &l2buf, 200);
  fx_get_layer_config_json(&fx->layer[2], &l3buf, 200);
  fx_get_layer_config_json(&fx->layer[3], &l4buf, 200);

  // strcmp(addr, "/length") == 0
  // strcmp(addr, "/nudge") == 0
  // strcmp(addr, "/opacity") == 0
  // strcmp(addr, "/pixelorder") == 0

  sprintf(destination, "{\n"
    "\"length\":%d,\"pixelorder\":%d,\"opacity\":%d,\"nudge\":%d,\n,\"basespeed\":%d,\n"
    "\"layers\":[\n%s,\n%s,\n%s,\n%s\n"
    "]}",
    fx->num_leds, fx->pixel_order, fx->opacity, fx->time_offset, fx->base_speed,
    l1buf, l2buf, l3buf, l4buf);
}

#endif


#ifdef WITH_PROPS

bool fx_set_id_property(FxSettings *fx, uint8_t prop, int32_t value) {
  bool debug = true;

  if (prop == GLOB_NUM_LEDS) {
    fx->num_leds = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Master num leds: %d\n", fx->num_leds);
    }
    #endif
    return true;
  }

  if (prop == GLOB_NUDGE) {
    fx->time_offset = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Master time offset: %d\n", fx->time_offset);
    }
    #endif
    return true;
  }

  if (prop == GLOB_OPACITY) {
    fx->opacity = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Master opacity: %d\n", fx->opacity);
    }
    #endif
    return true;
  }

  if (prop == GLOB_PIXELORDER) {
    fx->pixel_order = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Pixel order: %d\n", fx->pixel_order);
    }
    #endif
    return true;
  }

  if (prop == GLOB_BASE_SPEED) {
    fx->base_speed = value;
    #ifdef WITH_DEBUG
    if (debug) {
      printf("Set property: Base speed: %d\n", fx->base_speed);
    }
    #endif
    return true;
  }

  for(int layer=0; layer<4; layer++) {
    int propoffset = LAYER1_OFFSET;
    if (layer == 1) propoffset = LAYER2_OFFSET;
    if (layer == 2) propoffset = LAYER3_OFFSET;
    if (layer == 3) propoffset = LAYER4_OFFSET;

    if (prop == (propoffset + LAYER_OPACITY)) {
      fx->layer[layer].opacity = value;
      #ifdef WITH_DEBUG
      if (debug) {
        printf("Set property: Layer %d opacity: %d\n", layer + 1, fx->layer[layer].opacity);
      }
      #endif
      return true;
    }

    if (prop == (propoffset + LAYER_OFFSET)) {
      fx->layer[layer].offset = value;
      #ifdef WITH_DEBUG
      if (debug) {
        printf("Set property: Layer %d offset: %d\n", layer + 1, fx->layer[0].offset);
      }
      #endif
      return true;
    }

    if (prop == (propoffset + LAYER_COLOR_R)) {
      fx->layer[layer].color[0] = value;
      #ifdef WITH_DEBUG
      if (debug) {
        printf("Set property: Layer %d red/hue: %d\n", layer + 1, fx->layer[layer].color[0]);
      }
      #endif
      return true;
    }

    if (prop == (propoffset + LAYER_COLOR_G)) {
      fx->layer[layer].color[1] = value;
      #ifdef WITH_DEBUG
      if (debug) {
        printf("Set property: Layer %d green/saturation: %d\n", layer + 1, fx->layer[layer].color[1]);
      }
      #endif
      return true;
    }

    if (prop == (propoffset + LAYER_COLOR_B)) {
      fx->layer[layer].color[2] = value;
      #ifdef WITH_DEBUG
      if (debug) {
        printf("Set property: Layer %d blue/value: %d\n", layer + 1, fx->layer[layer].color[2]);
      }
      #endif
      return true;
    }

    if (prop == (propoffset + LAYER_COLORSPACE)) {
      fx->layer[layer].colorspace = value;
      #ifdef WITH_DEBUG
      if (debug) {
        printf("Set property: Layer %d colorspace: %d\n", layer + 1, fx->layer[layer].colorspace);
      }
      #endif
      return true;
    }

    if (prop == (propoffset + LAYER_SIZE)) {
      fx->layer[layer].size = value;
      #ifdef WITH_DEBUG
      if (debug) {
        printf("Set property: Layer %d size: %d\n", layer + 1, fx->layer[layer].size);
      }
      #endif
      return true;
    }

    if (prop == (propoffset + LAYER_REPEAT)) {
      fx->layer[layer].repeat = value;
      #ifdef WITH_DEBUG
      if (debug) {
        printf("Set property: Layer %d repeat: %d\n", layer + 1, fx->layer[layer].repeat);
      }
      #endif
      return true;
    }

    if (prop == (propoffset + LAYER_SPEED_MULTIPLIER)) {
      fx->layer[layer].speed_multiplier = value;
      #ifdef WITH_DEBUG
      if (debug) {
        printf("Set property: Layer %d speed multiplier: %d\n", layer + 1, fx->layer[layer].speed_multiplier);
      }
      #endif
      return true;
    }

    if (prop == (propoffset + LAYER_FEATHER_LEFT)) {
      fx->layer[layer].feather_left = value;
      #ifdef WITH_DEBUG
      if (debug) {
        printf("Set property: Layer %d feather_left: %d\n", layer + 1, fx->layer[layer].feather_left);
      }
      #endif
      return true;
    }

    if (prop == (propoffset + LAYER_FEATHER_RIGHT)) {
      fx->layer[layer].feather_right = value;
      #ifdef WITH_DEBUG
      if (debug) {
        printf("Set property: Layer %d feather_right: %d\n", layer + 1, fx->layer[layer].feather_right);
      }
      #endif
      return true;
    }

    if (prop == (propoffset + LAYER_BLEND)) {
      fx->layer[layer].blend = value;
      #ifdef WITH_DEBUG
      if (debug) {
        printf("Set property: Layer %d blend: %d\n", layer + 1, fx->layer[layer].blend);
      }
      #endif
      return true;
    }
  }

  return false;
}

#endif


