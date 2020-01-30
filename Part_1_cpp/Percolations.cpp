
#include <cstddef>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <vector>
#include <memory>
#include <set>
#include <random>


std::set<std::pair<int, int>> make_circle(int size, int len_from_center, int hole_index);
int BiteSum(int number);

class HexLattice {
public:
    explicit HexLattice(int size, int number_of_liquids);

    std::vector<int> MakeGuesses(int count);

    int SetConfigAndCheck(std::set<std::pair<int, int>> TypeA, std::set<std::pair<int, int>> TypeB);

private:
    int size;
    int liquids;
    std::vector<std::vector<std::byte>> Lattice;

    void InitClear();

    void FillRandomly();

    void EstablishZeroLiquid();

    void InitProbSubs();

    std::set<std::pair<int, int>> GetNeighbours(int x, int y);

    bool IsInBoard(int x, int y);

    bool IsInside(int x, int y);

    bool CheckPercol(int LiqNum);

    void dfs(int x, int y);

};


///////////////

int numberOfTests = 0;
int numberOfSuccessfullTests = 0;
int numberOfFailedTests = 0;

void reportOk(std::string testName) 
{
    std::cout << "OK: " << testName << "\n";
    numberOfSuccessfullTests++;
    numberOfTests++;
}

void reportFail(std::string testName) 
{
    std::cout << "FAILED: " << testName << "\n";
    numberOfFailedTests++;
    numberOfTests++;
}

void test_MKS(bool expected_result)
{
    std::shared_ptr<HexLattice> lattice =
        std::make_shared<HexLattice>(2, 3);
    std::vector<int> res;
    res = lattice->MakeGuesses(200);
    int qq = 0;
    int bb = res.size();
    for (int i = 0; i < bb; ++i) {
        qq += res[i];
    }
    if (expected_result == (qq == 200)) {
        reportOk("MBSk_test");
    }
    else {
        reportFail("MBSk_test");
    }
}

void test_k_circle(std::string testName, int size, int len_from_center, bool expected_result) {
    std::shared_ptr<HexLattice> lattice =
        std::make_shared<HexLattice>(size, 3);
    std::set<std::pair<int, int>> One;
    One.clear();
    One = make_circle(size, len_from_center, -1);

    std::set<std::pair<int, int>> Two;
    Two.clear();
    int ind = lattice->SetConfigAndCheck(One, Two);

    if (expected_result == (ind == 0 || (size == 1 && ind == 4))) {
        reportOk(testName);
    }
    else {
        reportFail(testName);
    }
}

int run_all_tests() 
{
    test_k_circle("test circle size 100", 100, 5, true);
    test_k_circle("test circle size 1", 1, 1, true);
    test_k_circle("test circle size 2", 2, 1, false);
    test_k_circle("test circle size 3", 3, 1, true);
    test_k_circle("test circle size 4", 4, 3, true);
    test_MKS(true);
    const bool allOk = numberOfFailedTests == 0;
    if (allOk) {
        std::cout << "All tests OK: " << numberOfSuccessfullTests
                  << " OK out of " << numberOfTests << "\n";
        return 0;
    }
    else {
        std::cout << "Tests FAILED!!!: " << numberOfSuccessfullTests << " OK, "
                  << numberOfFailedTests << " FAILED " << "out of " << numberOfTests << "\n";
        return 1;
    }
}

///////////////

int main()
{
    std::cout << "Hi! Let's set some parametrs to run experiments!\n"
              << "First of all, enter name of the file to save results (by default it's 'data').\n"
              << "Extention .csv will be added automatically:";
    std::string filename;
    std::cin >> filename;
    filename = filename == "" ? "data.csv" : filename + ".scv";
    std::cout << "Please, choose the number of liquids smaller then 30 (by default it is 3):";
    std::string liquids_count_txt;
    std::cin >> liquids_count_txt;
    if (liquids_count_txt == "test") {
        std::cout >> "You asked to run test. Well...\n";
        run_all_tests();
        std::count >> "Press ENTER to exit.";
        std::cin << filename;
        return 1;
    }
    
    int guesses = 0;
    int size = 0;
    std::cout << "Size: from 5 to 500 with step 5";
    //std::cin >> size;
    std::cout << "\nNumber of guesses: ";
    std::cin >> guesses;
    std::ofstream result_file;
    std::shared_ptr<HexLattice> qq = std::make_shared<HexLattice>(100, 2);
    std::vector<int> res;
    res = qq->MakeGuesses(2000);
    float prot = float(res[1]) / 2 + float(res[2]);
    float m = 0;
    float d = 0;
    m = (prot) / 2000;
    d = (prot*(1 - m)*(1 - m) + (2000 - prot)*m*m) / 2000;
    std::cout << m << "\n" << d;
    //for (int p = 10; p < 501; p += 5) {
    //    result_file.open("D:\\Misha\\Универ\\2018-19\\к курсово\\ress.txt", std::ios::app);
    //    //result_file << "\n\n" << "-----------" << p << "--------------" << "\n\n";
    //    //std::cout << "\n\n" << "-----------" << p << "--------------" << "\n\n";
    //    std::vector<int> res;
    //    /*for (int i = 2; i <= 30; ++i) {
    //        std::shared_ptr<HexLattice> qq = std::make_shared<HexLattice>(p, i);
    //        res = qq->MakeGuesses(guesses);
    //        std::cout << "\n" << i << "\t";
    //        result_file << "\n" << i << "\t";
    //        for (int j = 0; j <= i; ++j) {
    //            std::cout << (double)res[j] / guesses << "\t";
    //            result_file << (double)res[j] / guesses << "\t";
    //        }
    //    }*/
    //    const int i = 3;
    //    std::shared_ptr<HexLattice> qq = std::make_shared<HexLattice>(p, i);
    //    res = qq->MakeGuesses(guesses);
    //    std::cout << "\n" << i << "\t" << p << "\t";
    //    result_file << "\n" << i << "\t" << p << "\t";
    //    std::cout << (double)res[i] / guesses << "\t";
    //    result_file << (double)res[i] / guesses << "\t";
    //    result_file.close();

    //}
    std::cout << size;
    result_file.close();
    return 0;
}




std::set<std::pair<int, int>> make_circle(int size, int len_from_center, int hole_index){
    std::set<std::pair<int, int>> One;
    One.clear();
    int count = 0;
    for (int i = - 1 - len_from_center; i <= -1; ++i) {
        if (count != hole_index) {
            One.insert(std::make_pair(size + i, size - 1 - len_from_center));
        }
        count++;
        if (count != hole_index) {
            One.insert(std::make_pair(size + i, size + i + len_from_center));
        }
        count++;
    }
    for (int i = - len_from_center; i < -1; ++i) {
        if (count != hole_index) {
            One.insert(std::make_pair(size - 1 - len_from_center, size + i));
        }
        count++;
        if (count != hole_index) {
            One.insert(std::make_pair(size + i + len_from_center, size + i));
        }
        count++;
    }
    for (int i = size; i <= size - 1 + len_from_center; ++i) {
        if (count != hole_index) {
            One.insert(std::make_pair(i, size - 1 + len_from_center));
        }
        count++;
    }
    for (int i = size - 1; i <= size + len_from_center - 2; ++i) {
        if (count != hole_index) {
            One.insert(std::make_pair(size - 1 + len_from_center, i));
        }
        count++;
    }
    return One;
}

int BiteSum(int number) {
    int result = 0;
    while (number != 0) {
        result += number & 1;
        number = number >> 1;
    }
    result = result & 1;
    return result;
}

bool HexLattice::IsInBoard(int x, int y) { //проверяем лежит ли точка (x, y) на границе шестиугольника.
    return (x == 0) || (x == 2 * size - 2) || (y == 0) || (y == 2 * size - 2) || (x - y == size - 1) || (y - x == size - 1);
}

bool HexLattice::IsInside(int x, int y) { //проверяем лежит ли точка (x, y) в допустимом диапазоне шестиугольника.
        return (x >= 0) && (x <= 2 * size - 2) && (y >= 0) && (y <= 2 * size - 2) && (x - y <= size - 1) && (y - x <= size - 1);
    }