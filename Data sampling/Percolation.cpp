// Copyright Fedorov Mikhail, 2022
// fedorov.mikhail.s@gmail.com

#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

class ColoredHexagon {
public:
    ColoredHexagon(int size, int liquids);

    void Randomize(std::mt19937 *generator);

    int HexagonSize() const;

    int LiquidsCount() const;

    bool IsInBorder(int x, int y) const;

    std::vector<std::pair<int, int>> GetLiquidNeighbors(int x, int y, int liquid) const;

private:
    int hexagon_size;
    int liquids_count;
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
    ```
    *.*.*
    .\|..
    *-*-*
    ..|\.
    *.*.*
    ```
    */
    std::vector<std::vector<std::vector<int8_t>>> coloring;

    void InitProbSubs();

    bool IsInside(int x, int y) const;
};

/**
 * @brief The function takes a hexagon object and for each of the
 * liquids check whether it percolates between the center and the border
 * of the hex.
 *
 * @param hex The hexagonal lattice object, contains the information on which
 * @return std::vector<bool> if (argc > 1 && !strcmp(argv[1], "--testing")) {
        UnitTests();
        StressTests();
        return 0;
    }For each liquid returns whether this liquid
 * percolate the hexagon or not.
 */
std::vector<bool> CheckPercolate(const ColoredHexagon &hex);

void WriteToFile(std::vector<std::vector<bool>> results, std::string filename);

void UnitTests();

int main(int argc, char **argv) {
    if (argc > 1 && !strcmp(argv[1], "--testing")) {
        UnitTests();
        std::cerr << "Finished running tests\n";
        return 0;
    }
    std::mt19937 generator(1514);
    size_t min_size = 5;
    size_t max_size = 500;
    size_t step = 5;
    std::cout << "Size: from " << min_size << " to " << max_size << " with step " << step << ".\n";
    std::cout << "Enter number of samples: ";
    size_t samples;
    std::cin >> samples;
    std::cout << "Enter number of liquids: ";
    size_t liquids_count;
    std::cin >> liquids_count;

    int64_t total_steps = 0;
    for (auto size = min_size; size <= max_size; size += step) {
        total_steps += samples * size * size;
    }
    int64_t current_step = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto size = min_size; size <= max_size; size += step) {
        std::vector<std::vector<bool>> results;
        std::stringstream filename_stream;
        filename_stream << "./results_csv/results_liquids_" << liquids_count << "_size_" << size
                        << ".csv";
        auto filename = filename_stream.str();
        results.reserve(samples);
        ColoredHexagon hex(size, liquids_count);
        for (auto num = 0; static_cast<size_t>(num) < samples; ++num) {
            auto time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                                    std::chrono::high_resolution_clock::now() - start)
                                    .count();
            auto time_estimated =
                current_step > 0 ? static_cast<double>(time_elapsed * total_steps) / current_step
                                 : 0;
            auto minutes_estimated = static_cast<int>(floor(time_estimated / 60));
            auto seconds_estimated = time_estimated - minutes_estimated * 60;
            std::cerr << "Computations " << std::fixed << std::setprecision(2)
                      << static_cast<double>(current_step) / total_steps * 100 << "%; Time passed "
                      << time_elapsed / 60 << ":" << std::setw(2) << std::setfill('0')
                      << time_elapsed % 60 << " out of " << minutes_estimated << ":" << std::setw(5)
                      << std::setfill('0') << seconds_estimated << " estimated.\r";
            current_step += size * size;
            hex.Randomize(&generator);
            results.push_back(CheckPercolate(hex));
        }
        WriteToFile(results, filename);
    }
    return 0;
}

ColoredHexagon::ColoredHexagon(int size, int liquids) {
    hexagon_size = size;
    liquids_count = liquids;
    assert(liquids > 1);
    coloring.resize(size * 2 - 1);
    for (auto &line : coloring) {
        line.resize(size * 2 - 1);
        for (auto &cell : line) {
            cell.resize(liquids, 0);
            cell[0] = 1;
        }
    }
}

void ColoredHexagon::Randomize(std::mt19937 *generator) {
    std::bernoulli_distribution bernoulli_bool(0.5);
    for (auto &line : coloring) {
        for (auto &cell : line) {
            cell[0] = 1;
            for (int index = 1; index < liquids_count; ++index) {
                cell[index] = static_cast<uint8_t>(bernoulli_bool(*generator));
                cell[0] += cell[index];
            }
            cell[0] %= 2;
        }
    }
}

int ColoredHexagon::LiquidsCount() const {
    return liquids_count;
}

int ColoredHexagon::HexagonSize() const {
    return hexagon_size;
}

std::vector<std::pair<int, int>> ColoredHexagon::GetLiquidNeighbors(int x, int y,
                                                                    int liquid) const {
    std::vector<std::pair<int, int>> list;
    list.reserve(6);
    if (x > 0 && y > 0 && coloring[x - 1][y - 1][liquid] == 1) {
        list.emplace_back(x - 1, y - 1);
    }
    if (y > 0 && x - y < hexagon_size && coloring[x][y - 1][liquid] == 1) {
        list.emplace_back(x, y - 1);
    }
    if (x - y > hexagon_size && x < 2 * hexagon_size - 2 && coloring[x + 1][y][liquid] == 1) {
        list.emplace_back(x + 1, y);
    }
    if (x < 2 * hexagon_size - 2 && y < 2 * hexagon_size - 2 &&
        coloring[x + 1][y + 1][liquid] == 1) {
        list.emplace_back(x + 1, y + 1);
    }
    if (y < 2 * hexagon_size - 2 && y - x < hexagon_size && coloring[x][y + 1][liquid] == 1) {
        list.emplace_back(x, y + 1);
    }
    if (y - x < hexagon_size && x > 0 && coloring[x - 1][y][liquid] == 1) {
        list.emplace_back(x - 1, y);
    }
    return list;
}

bool ColoredHexagon::IsInBorder(int x, int y) const {
    return x == 0 || y == 0 || x == 2 * hexagon_size - 2 || y == 2 * hexagon_size - 2 ||
           x - y == hexagon_size || y - x == hexagon_size;
}

std::vector<bool> CheckPercolate(const ColoredHexagon &hex) {
    auto liquids = hex.LiquidsCount();
    auto size = hex.HexagonSize();
    std::vector<bool> results(liquids, false);
    std::vector<std::vector<std::vector<bool>>> not_visited(
        liquids,
        std::vector<std::vector<bool>>(size * 2 - 1, std::vector<bool>(size * 2 - 1, true)));
    for (auto liquid_number = 0; liquid_number < liquids; ++liquid_number) {
        not_visited[liquid_number][size][size] = false;
    }
    std::vector<std::vector<std::pair<int, int>>> stacks(liquids,
                                                         {std::make_pair(size - 1, size - 1)});
    int checked = 0;
    while (checked < liquids) {
        for (auto liquid_number = 0; liquid_number < static_cast<int>(stacks.size());
             ++liquid_number) {
            auto &stack = stacks[liquid_number];
            if (stack.empty()) {
                continue;
            }
            auto cell = stack.back();
            stack.pop_back();
            for (auto &new_cell : hex.GetLiquidNeighbors(cell.first, cell.second, liquid_number)) {
                if (hex.IsInBorder(new_cell.first, new_cell.second)) {
                    results[liquid_number] = true;
                    stack.clear();
                    break;
                } else if (not_visited[liquid_number][new_cell.first][new_cell.second]) {
                    not_visited[liquid_number][new_cell.first][new_cell.second] = false;
                    stack.push_back(new_cell);
                }
            }
            if (stack.empty()) {
                ++checked;
            }
        }
    }
    return results;
}

void WriteToFile(std::vector<std::vector<bool>> results, std::string filename) {
    std::ofstream output(filename, std::ios::binary);
    for (auto &experiment : results) {
        for (auto liquid : experiment) {
            output << (liquid ? 1 : 0) << ",";
        }
        output << '\n';
    }
}

void UnitTests() {
    ColoredHexagon hex(2, 3);
    auto percol = CheckPercolate(hex);
    assert(percol[0] && !percol[1] && !percol[2]);
}