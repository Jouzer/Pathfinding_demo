#pragma once
#include <utility>
#include <string>
#define ROW 9
#define COL 10

using Pair = std::pair<int, int>;

struct PathData {
    std::string message;
    std::stack<Pair> path;
};

PathData aStarSearch(int grid[][COL], Pair src, Pair dest);

