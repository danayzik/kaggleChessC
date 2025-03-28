cmake_minimum_required(VERSION 3.10)
project(chessBot)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add the source files to the project
add_executable(chessBot
        main.cpp          # Your main source file
        game_tables.cpp        # Add this line to include tables.cpp where initTables() is defined
        evaluation.cpp    # Add any other .cpp files as needed
        search.cpp
        game_tables.cpp        # If search.cpp is involved, include it too
)

# Add any additional libraries or dependencies here if needed