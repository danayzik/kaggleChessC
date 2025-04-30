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
        src/sorters.cpp
        src/heuristic_evaluator.cpp
        src/history_killer_pv_searcher.cpp
        src/history_killer_pv_TT_searcher.cpp
        src/negaMaxSearcher.cpp
        src/negaMaxQSearcher.cpp

)

