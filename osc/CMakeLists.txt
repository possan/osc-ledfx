cmake_minimum_required (VERSION 3.9)

add_definitions(-DWITH_OSC)

add_executable (osctest
    main.c
    tinyosc.c
    ../lib/fx.c)

# target_link_libraries(consoleui PRIVATE nlohmann_json::nlohmann_json)
