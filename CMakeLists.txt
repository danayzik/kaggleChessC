cmake_minimum_required(VERSION 3.10)
project(chessBot)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add the source files to the project
add_executable(chessBot
        src/main.cpp
        src/game_tables.cpp
        src/transposition_table.cpp
        src/engine.cpp
        src/move_sorters.cpp
        src/searchers.cpp
        src/heuristic_evaluator.cpp

)

# Add any additional libraries or dependencies here if needed