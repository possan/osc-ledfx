# Led strip pattern generator

Four simple animated layers, each with controllable parameters. Listens to changes over OSC.

## OSC Protocol

### `/length {float}`
number of leds

### `/nudge {float}`
time offset in milliseconds

### `/sync`
restart all timers

### `/layerN/opacity {float}`
opacity, percent

### `/layerN/offset {float}`
start position, milliseconds

### `/layerN/(red|green|blue) {float}`
color, 0-255

### `/layerN/radius {float}`
solid fill, in leds

### `/layerN/feather_left {float}`
fade on the left, in leds

### `/layerN/feather_right {float}`
fade on the right, in leds

### `/layerN/speed {float}`
movement leds/second

### `/layerN/repeat {float}`
repeat times


## Demo programs

Build with CMake and run example

```
# On Mac:
mkdir build
cd build
cmake -G "Unix Makefiles" ..
make
./osc/osctest
```

Open PureData controller and send OSC, enjoy fancy vt100 color preview.
