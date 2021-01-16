cmake_minimum_required (VERSION 3.9)

# add_definitions(-DWITH_PROPS)
# SET(CMAKE_EXECUTABLE_SUFFIX ".wasm")
# emcc hello.c -s WASM=1 -o hello.html

set(JSBINDING_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/jsbinding.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../lib/fx.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../lib/player.c"
)

set(JSBINDING_CFLAGS -DWITH_PROPS)

# add_dependencies(jsbinding_wasm)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/jsbinding.wasm
    COMMAND
      emcc ${JSBINDING_SOURCES} ${JSBINDING_CFLAGS}
        -O2
        -s "NO_EXIT_RUNTIME=1"
        # -s "SIDE_MODULE=1"
        -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
        -s "EXPORTED_FUNCTIONS=['_main','_fx_binding_reset','_fx_binding_load_animation','_fx_binding_set_property','_fx_binding_render','_malloc','_free']"
        -s "WASM=1"
        -o jsbinding.js
    COMMENT "Building webassembly"
    DEPENDS ${JSBINDING_SOURCES}
    VERBATIM
)

configure_file(test.html test.html COPYONLY)

add_custom_target(jsbinding ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/jsbinding.wasm)

# add_custom_target(run ALL DEPENDS hello.txt)

# add_library(jsbinding INTERFACE)

# add_library(foo INTERFACE)
