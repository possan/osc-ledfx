cmake_minimum_required (VERSION 3.9)

add_executable (fxtest
    main.c
    ../lib/fx.c)

# target_link_libraries(consoleui PRIVATE nlohmann_json::nlohmann_json)
