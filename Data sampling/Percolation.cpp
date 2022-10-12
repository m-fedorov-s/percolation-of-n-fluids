// Copyright Fedorov Mikhail, 2022
// fedorov.mikhail.s@gmail.com

#include <iostream>
#include <vector>
#include <memory>
#include <cassert>
#include <cstring>
#include <string>
#include <chrono>
#include <random>

class ColoredHexagon {
public:
    ColoredHexagon(int size, int liquids);
private:
    int hexagon_size;
    std::vector<std::vector<std::vector<bool>>> coloring;
    /*
    Assume the hexagon is represented in the square lattice in the following way:
    *******......
    ********.....
    *********....
    **********...
    ***********..
    ************.
    ******0******
    .************
    ..***********
    ...**********
    ....*********
    .....********
    ......*******
    This is a hexagon with side of 7 cells.
    The neighbors of one cell are:
    *.*.*
    .\|..
    *-*-*
    ..|\.
    *.*.*
    int liquids_count;
};

/**
 * @brief The function takes a hexagon object and for each of the
 * liquids check whether it percolates between the center and the border
 * of the hex.
 * 
 * @param hex The hexagonal lattice object, contains the information on which 
 * @return std::vector<bool> For each liquid returns whether this liquid percolate the hexagon or not.
 */
std::vector<bool> CheckPercolate(const ColoredHexagon& hex);


int main() {
    std::mt19937 generator(1514);
    auto max_len = 1'000'000;
    auto count = 100;
    auto part = count / 10;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto num = 0; num < count; ++num) {
        auto time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - start).count();
        auto time_estimated = num > 0 ? time_elapsed / num * count : 0;
        std::cerr << "Step " << num << "/" << count << "; Time " << time_elapsed << "/" << time_estimated << '\r';
        hex.Randomize(generator);
        result = CheckPercolate(hex);
        WriteToFile(filename);
    }
    return 0;
}