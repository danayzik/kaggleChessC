#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <tuple>
#include "../include/chess.hpp"
#include "../include/game_tables.h"
#include "../include/evaluator.h"
#include <memory>
using namespace chess;
using namespace evaluation;


// Normalize function
std::pair< float, float> normalize(std::vector<float>& data) {
    // Calculate the mean
    float mean = 0.0f;
    for (float value : data) {
        mean += value;
    }
    mean /= data.size();

    // Calculate the standard deviation
    float stddev = 0.0f;
    for (float value : data) {
        stddev += (value - mean) * (value - mean);
    }
    stddev = std::sqrt(stddev / data.size());

    // Normalize the data
    for (float& value : data) {
        value = (value - mean) / stddev;
    }

    // Return normalized data, mean, and standard deviation
    return  {mean, stddev};
}

int main() {
    initTables();
    std::ifstream file("C:\\Users\\danay\\Desktop\\uni\\year4sem1\\workshop\\data.csv");
    std::string line;
    std::vector<std::string> fens;
    std::vector<float> targets;
    std::unique_ptr<Evaluator> evaluator = std::make_unique<HeuristicEvaluator>();
    evaluator->setGameOngoing();
    std::getline(file, line);  // Skip header row

    // Load data
    while (std::getline(file, line)) {
        size_t comma = line.find(',');
        std::string fen = line.substr(0, comma);
        float eval = std::stof(line.substr(comma + 1));
        fens.push_back(fen);
        targets.push_back(eval);
    }

    // Take 80%-90% slice
    size_t N = fens.size();
    size_t start = N * 9 / 10;
    size_t end = N;

    std::vector<float> preds, actuals;

    // Evaluate for the 80%-90% range
    for (size_t i = start; i < end; ++i) {
        Board board(fens[i]);
        float pred = evaluator->getEval(board);
        preds.push_back(pred);
        actuals.push_back(targets[i]);
    }

    // Normalize both sets
    normalize(preds);
    auto [ mean, stddev] = normalize(actuals);

    // Compute MAE
    float sum_abs_err = 0.0f;
    for (size_t i = 0; i < preds.size(); ++i) {
        sum_abs_err += std::abs(preds[i] - actuals[i]);
    }

    float mae_normalized = sum_abs_err / preds.size();
    std::cout << "Normalized MAE: " << mae_normalized << std::endl;

    // Denormalize the MAE
    float mae_denormalized = mae_normalized * stddev + mean;
    std::cout << "Denormalized MAE: " << mae_denormalized << std::endl;

    return 0;
}
