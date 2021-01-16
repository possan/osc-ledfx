#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

#include "../lib/fx.h"
#include "tinyosc.h"

#define PORT 9000
#define BUFFERSIZE 2000

uint32_t lastResetTime = 0;
FxSettings fx;
uint8_t rgb[1000 * 3];
uint8_t temp[1000];

uint32_t millis() {
  struct timespec spec;
  uint32_t ms;
  clock_gettime(CLOCK_REALTIME, &spec);
  ms = (spec.tv_nsec / 1000000) + (spec.tv_sec * 1000);
  return ms;
}

void render() {
  char bri[6] = { " .+:#@"};
  uint32_t T = millis() - lastResetTime;
  fx_render(&fx, T, &rgb, 1000, &temp);

  printf("FRAME: ");
  for(int j=0; j<fx.num_leds; j++) {
    // int b = ((rgb[j*3+0] + rgb[j*3+1] + rgb[j*3+2]) * 6) / 768;
    int rr = (rgb[j*3+0] * 6) / 256;
    int gg = (rgb[j*3+1] * 6) / 256;
    int bb = (rgb[j*3+2] * 6) / 256;
    int cc = 16 + bb + 6 * gg + 36 * rr;
    // printf("\033[38;5;%dm%c", bri[b], 20 + (rand() % 100));
    printf("\033[38;5;%dm#", cc);
  }
  printf("\033[0m\n");
}

void handleOSC(tosc_message *osc) {
  const char *addr = tosc_getAddress(osc);
  tosc_getFormat(osc);

  if (strcmp(addr, "/sync") == 0) {
    lastResetTime = millis();
    return;
  }

  float v = tosc_getNextFloat(osc);
  if (!fx_set_osc_property(&fx, addr, v)) {
    printf("Unhandled OSC: ");
    tosc_printMessage(osc);
  }
}

void parseOSC(uint8_t *buffer, int len) {
  if (tosc_isBundle((char *)buffer)) {
    tosc_bundle bundle;
    tosc_parseBundle(&bundle, (char *)buffer, len);
    const uint64_t timetag = tosc_getTimetag(&bundle);
    tosc_message osc;
    while (tosc_getNextMessage(&bundle, &osc)) {
      handleOSC(&osc);
    }
  } else {
    tosc_message osc;
    tosc_parseMessage(&osc, (char *)buffer, len);
    handleOSC(&osc);
  }
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main() {

  int i, fd, len, n, rv;
  float T, aa;
  uint32_t a, x;
  int sockfd;
  fd_set readfds;
  uint8_t buffer[BUFFERSIZE];
  struct sockaddr_in servaddr, cliaddr;
  struct timeval tv;
  socklen_t clilen;

  printf("Hello!\n");

  memset(&fx, 0, sizeof(FxSettings));
  fx.num_leds = 40;
  fx.opacity = 100;

  fx.layer[0].offset = 0;
  fx.layer[0].opacity = 100;
  fx.layer[0].color[0] = 255;
  // fx.layer[0].radius = 3;
  fx.layer[0].repeat = 1;
  fx.layer[0].feather_left = 3;
  fx.layer[0].feather_right = 10;
  fx.layer[0].opacity = 100;
  // fx.layer[0].speed = 1000;

  // fx.layer[1].offset = 33;
  // fx.layer[1].opacity = 33;
  // fx.layer[1].color[0] = 255;
  // fx.layer[1].color[1] = 255;
  // fx.layer[1].color[2] = 255;
  // fx.layer[1].radius = 1;
  // fx.layer[1].repeat = 1;
  // fx.layer[1].speed = -1000;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
    printf("socket creation failed\n");
    return 1;
  }

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    error("ERROR opening socket");
  }

  bzero((char *) &servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(PORT);

  if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
    error("ERROR on binding");
  }

  listen(sockfd, 5);

  printf("Starting to listen to port %d\n", PORT);

  lastResetTime = millis();

  memset(buffer, 0, BUFFERSIZE);
  while (1) {
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    tv.tv_sec = 0;
    tv.tv_usec = 10000;

    rv = select(sockfd + 1, &readfds, NULL, NULL, &tv);

    if (rv == 1) {
      clilen = sizeof(cliaddr);
      n = recvfrom(sockfd, (unsigned char *)buffer, BUFFERSIZE, 0, ( struct sockaddr *) &cliaddr, &clilen);
      parseOSC(buffer, n);
    }

    render();
  }
  close(sockfd);
  close(fd);

  return 0;

}
