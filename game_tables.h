#ifndef GAMETABLES_H
#define GAMETABLES_H

#include <array>
#include <cstdint>

// Values
const std::array<int, 6> mg_value = {82, 337, 365, 477, 1025, 0};
const std::array<int, 6> eg_value = {94, 281, 297, 512, 936, 0};

extern const std::array<int, 64> mg_pawn_table;
extern const std::array<int, 64> eg_pawn_table;
extern const std::array<int, 64> mg_knight_table;
extern const std::array<int, 64> eg_knight_table;
extern const std::array<int, 64> mg_bishop_table;
extern const std::array<int, 64> eg_bishop_table;
extern const std::array<int, 64> mg_rook_table;
extern const std::array<int, 64> eg_rook_table;
extern const std::array<int, 64> mg_queen_table;
extern const std::array<int, 64> eg_queen_table;
extern const std::array<int, 64> mg_king_table;
extern const std::array<int, 64> eg_king_table;

extern const std::array<const int*, 6> mg_pesto_table;
extern const std::array<const int*, 6> eg_pesto_table;

extern std::array<int, 12> gamephaseInc;
extern std::array<std::array<int, 64>, 12> mg_table;
extern std::array<std::array<int, 64>, 12> eg_table;

// Function declarations
void initTables();
constexpr int PCOLOR(int p);
constexpr int FLIP(int sq);

#endif // GAMETABLES_H
