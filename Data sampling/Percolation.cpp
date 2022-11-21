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
#include <thread>
#include <utility>
#include <vector>

class ColoredHexagon {
public:
    ColoredHexagon(int size, int liquids);

    void Randomize(std::mt19937 *generator);

    int HexagonSize() const;

    int LiquidsCount() const;

    bool IsInBorder(int x, int y) const;

    const std::vector<std::vector<std::vector<int8_t>>> &Coloring() const;

    void SetZeroLiquid(const std::vector<std::vector<bool>> &liquid_map);

    void PrintLiquid(int liquid_index) const;

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
    std::cout << "This program will run experiments for several sizes of a hexagon, for several "
                 "number of liquids.\n"
              << "For each size and number of liquids we will sample a fixed number of colorings "
                 "of the hexagonal.\n"
              << "And for each coloring we will check percolation of each liquid.\n";
    std::cout << "The sizes of the hexagon will be from min_size to max_size with a fixed step.\n"
              << "I.e. sizes would be min_size; min_size + step; min_size + 2 * step ...\n"
              << "Please, enter the minimal size of the hexagon (recommended at least 5):";
    size_t min_size;
    std::cin >> min_size;
    std::cout << "Please, enter the maximal size of the hexagon: ";
    size_t max_size;
    std::cin >> max_size;
    std::cout << "Please, enter the step of the sizes (recommended 5): ";
    size_t step;
    std::cin >> step;
    std::cout << "Size: from " << min_size << " to " << max_size << " with step " << step << ".\n";
    std::cout << "The number of liquids will be from min_liquids to max_liquids with step 1\n";
    std::cout << "Please, enter the minimal number of liquids: ";
    size_t liquids_count_min;
    std::cin >> liquids_count_min;
    std::cout << "Please, enter the maximal number of liquids: ";
    size_t liquids_count_max;
    std::cin >> liquids_count_max;
    assert(liquids_count_max >= liquids_count_min);
    std::cout << "Liquids number will be from " << liquids_count_min << " to " << liquids_count_max
              << " inclusively.\n";
    std::cout << "Please, enter the number of samples: ";
    size_t samples;
    std::cin >> samples;
    std::cout << "The results will be saved in folder results_csv in a separate files.\n";

    int64_t total_steps = 0;
    for (auto size = min_size; size <= max_size; size += step) {
        for (auto liquids = liquids_count_min; liquids <= liquids_count_max; ++liquids) {
            total_steps += liquids * size * size;
        }
    }
    int64_t current_step = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto liquids_count = liquids_count_min; liquids_count <= liquids_count_max;
         ++liquids_count) {
        for (auto size = min_size; size <= max_size; size += step) {
            std::stringstream filename_stream;
            filename_stream << "./results_csv/results_liquids_" << liquids_count << "_size_" << size
                            << ".csv";
            auto filename = filename_stream.str();

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
            current_step += liquids_count * size * size;

            std::vector<std::vector<bool>> results(samples, std::vector<bool>());
            // launch several threads to speed up process
            auto processor_count =
                std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() : 2;
            std::vector<std::thread> threads;
            threads.reserve(processor_count);
            auto lambda = [&](int shift) {
                std::mt19937 generator(1514 + shift);
                ColoredHexagon hex(size, liquids_count);
                for (auto index = shift; index < static_cast<int>(samples);
                     index += processor_count) {
                    hex.Randomize(&generator);
                    auto one_result = CheckPercolate(hex);
                    results[index] = one_result;
                }
            };
            for (auto thread_index = 0; thread_index < static_cast<int>(processor_count);
                 ++thread_index) {
                threads.emplace_back(lambda, thread_index);
            }

            for (auto &thread : threads) {
                thread.join();
            }
            WriteToFile(results, filename);
        }
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
    if (x - y < hexagon_size && x < 2 * hexagon_size - 2 && coloring[x + 1][y][liquid] == 1) {
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
           x - y == hexagon_size - 1 || y - x == hexagon_size - 1;
}

const std::vector<std::vector<std::vector<int8_t>>> &ColoredHexagon::Coloring() const {
    return coloring;
}

void ColoredHexagon::SetZeroLiquid(const std::vector<std::vector<bool>> &liquid_map) {
    for (auto i = 0; i < hexagon_size * 2 - 1; ++i) {
        for (auto j = 0; j < hexagon_size * 2 - 1; ++j) {
            coloring[i][j][0] = liquid_map[i][j];
        }
    }
}

void ColoredHexagon::PrintLiquid(int liquid_index) const {
    if (liquid_index >= liquids_count) {
        return;
    }
    for (auto &line : coloring) {
        for (auto &cell : line) {
            std::cerr << (cell[liquid_index] == 1 ? "x" : ".");
        }
        std::cerr << "\n";
    }
}

std::vector<bool> CheckPercolate(const ColoredHexagon &hex) {
    auto liquids = hex.LiquidsCount();
    auto size = hex.HexagonSize();
    std::vector<bool> results(liquids, false);
    std::vector<std::vector<std::vector<bool>>> not_visited(
        liquids,
        std::vector<std::vector<bool>>(size * 2 - 1, std::vector<bool>(size * 2 - 1, true)));
    for (auto liquid_number = 0; liquid_number < liquids; ++liquid_number) {
        not_visited[liquid_number][size - 1][size - 1] = false;
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

void TestRandomizing(int size, int liquids) {
    std::mt19937 generator(1514);
    ColoredHexagon hex(size, liquids);
    hex.Randomize(&generator);
    for (auto &line : hex.Coloring()) {
        for (auto &cell : line) {
            auto sum = 0;
            for (auto i : cell) {
                sum += i;
            }
            assert(sum % 2 == 1);
        }
    }
}

void MakeCircle(std::vector<std::vector<bool>> *map, int size, int radius) {
    for (auto i = 0; i < radius; ++i) {
        (*map)[size - 1 - i][size - 1 - (radius - 1)] = false;
        (*map)[size - 1 + i][size - 1 + (radius - 1)] = false;
        (*map)[size - 1 - (radius - 1)][size - 1 - i] = false;
        (*map)[size - 1 + (radius - 1)][size - 1 + i] = false;
        (*map)[size - 1 + i][size - 1 - (radius - 1) + i] = false;
        (*map)[size - 1 - (radius - 1) + i][size - 1 + i] = false;
    }
}

void TestCircle(int size, int radius) {
    ColoredHexagon hex(size, 2);
    std::vector<std::vector<bool>> coloring(size * 2 - 1, std::vector<bool>(size * 2 - 1, true));
    MakeCircle(&coloring, size, radius);
    coloring[size - 1][size - 1] = false;
    hex.SetZeroLiquid(coloring);
    auto percol = CheckPercolate(hex);
    assert(!percol[0] || radius == 1);
}

void TestCircleWithHole(int size, int radius) {
    ColoredHexagon hex(size, 2);
    std::vector<std::vector<bool>> coloring(size * 2 - 1, std::vector<bool>(size * 2 - 1, true));
    MakeCircle(&coloring, size, radius);
    coloring[size - 1][size - 1] = false;
    coloring[size - 1][size - 1 - (radius - 1)] = true;
    hex.SetZeroLiquid(coloring);
    auto percol = CheckPercolate(hex);
    assert(percol[0]);
}

void TestBorder() {
    ColoredHexagon hex2(2, 3);
    assert(hex2.IsInBorder(0, 0));
    assert(hex2.IsInBorder(1, 0));
    assert(hex2.IsInBorder(2, 1));
    assert(hex2.IsInBorder(2, 2));
    assert(hex2.IsInBorder(1, 2));
    assert(hex2.IsInBorder(0, 1));
    ColoredHexagon hex3(3, 3);
    assert(hex3.IsInBorder(0, 0));
    assert(hex3.IsInBorder(1, 0));
    assert(hex3.IsInBorder(2, 0));
    assert(hex3.IsInBorder(3, 1));
    assert(hex3.IsInBorder(4, 2));
    assert(hex3.IsInBorder(4, 3));
    assert(hex3.IsInBorder(4, 4));
    assert(hex3.IsInBorder(3, 4));
    assert(hex3.IsInBorder(2, 4));
    assert(hex3.IsInBorder(1, 3));
    assert(hex3.IsInBorder(0, 2));
    assert(hex3.IsInBorder(0, 1));
}

void TestGoOneDirection(int size, std::string direction) {
    ColoredHexagon hex(size, 2);
    std::vector<std::vector<bool>> coloring(size * 2 - 1, std::vector<bool>(size * 2 - 1, false));
    int x_shift = 0;
    int y_shift = 0;
    if (direction == "up") {
        x_shift = 0;
        y_shift = -1;
    } else if (direction == "down") {
        x_shift = 0;
        y_shift = 1;
    } else if (direction == "left") {
        x_shift = -1;
        y_shift = 0;
    } else if (direction == "right") {
        x_shift = 1;
        y_shift = 0;
    } else if (direction == "left-up") {
        x_shift = -1;
        y_shift = -1;
    } else if (direction == "right-down") {
        x_shift = 1;
        y_shift = 1;
    } else {
        std::cerr << "Wrong parameters: " << direction;
    }
    for (auto i = 0; i < size; ++i) {
        coloring[size - 1 + i * x_shift][size - 1 + i * y_shift] = true;
    }
    hex.SetZeroLiquid(coloring);
    auto percol = CheckPercolate(hex);
    if (!percol[0]) {
        std::cerr << "Failed direction: " << direction << '\n';
        hex.PrintLiquid(0);
    }
    assert(percol[0]);
}

void UnitTests() {
    ColoredHexagon hex(2, 3);
    auto percol = CheckPercolate(hex);
    assert(percol[0] && !percol[1] && !percol[2]);
    std::mt19937 generator(1514);
    for (auto i = 0; i < 100; ++i) {
        hex.Randomize(&generator);
        percol = CheckPercolate(hex);
        assert(percol[0] || percol[1] || percol[2]);
    }
    TestBorder();
    TestGoOneDirection(3, "up");
    TestGoOneDirection(30, "up");
    TestGoOneDirection(3, "down");
    TestGoOneDirection(30, "down");
    TestGoOneDirection(3, "left");
    TestGoOneDirection(30, "left");
    TestGoOneDirection(3, "right");
    TestGoOneDirection(30, "right");
    TestGoOneDirection(3, "left-up");
    TestGoOneDirection(30, "left-up");
    TestGoOneDirection(3, "right-down");
    TestGoOneDirection(30, "right-down");
    TestRandomizing(10, 3);
    TestRandomizing(100, 3);
    TestRandomizing(10, 5);
    TestRandomizing(100, 5);
    TestRandomizing(10, 10);
    TestRandomizing(100, 10);
    TestCircle(5, 2);
    TestCircle(5, 4);
    TestCircle(1000, 20);
    TestCircle(1000, 200);
    TestCircle(1000, 999);
    TestCircle(1000, 1000);
    TestCircleWithHole(500, 2);
    TestCircleWithHole(500, 20);
    TestCircleWithHole(500, 200);
    TestCircleWithHole(500, 499);
    TestCircleWithHole(500, 500);
}