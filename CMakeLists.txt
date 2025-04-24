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
        src/basic_searcher.cpp
        src/heuristic_evaluator.cpp
        src/basic_iterative_searcher.cpp
        src/iterative_pv_searcher.cpp
        src/history_pv_searcher.cpp
        src/history_killer_pv_searcher.cpp

)

# Add any additional libraries or dependencies here if needed