// ConsoleApplication2.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "set"
#include"memory"
//#include "stdlib"
#include "random"
#include "utility"
#include "functional"
#include "iostream"
#include "assert.h"
#include <string>


std::set<std::pair<int, int>> SetUnion(std::set<std::pair<int, int>> First, std::set<std::pair<int, int>> Sec);
std::set<std::pair<int, int>> SetIntersect(std::set<std::pair<int, int>> First, std::set<std::pair<int, int>> Sec);
std::set<std::pair<int, int>> SetDiffer(std::set<std::pair<int, int>> First, std::set<std::pair<int, int>> Sec);

class HexLattice
{
private:
	int size;
	std::set<std::pair<int, int>> GoodA;
	std::set<std::pair<int, int>> GoodB;
	std::set<std::pair<int, int>> GoodC;
	std::set<std::pair<int, int>> EveryCel;
	std::set<std::pair<int, int>> NotVisited;
	bool Perc = false;
	int RandState;

	void InitProbSubs()
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distribution(0, 3);
		GoodA.clear();
		GoodB.clear();
		GoodC.clear();
		NotVisited.clear();
		Perc = false;
		for (auto it = EveryCel.begin(); it != EveryCel.end(); ++it) {
			RandState = distribution(gen);
			if (RandState == 0) {
				GoodA.insert(*it);
				GoodB.insert(*it);
				GoodC.insert(*it);
			}
			else if (RandState == 1) {
				GoodA.insert(*it);
			}
			else if (RandState == 2) {
				GoodB.insert(*it);
			}
			else if (RandState == 3) {
				GoodC.insert(*it);
			};
		}
	}

	std::set<std::pair<int, int>> GetNeighbours(int x, int y) {
		return { std::make_pair(x - 1, y - 1),std::make_pair(x - 1, y),std::make_pair(x, y - 1),std::make_pair(x + 1, y),std::make_pair(x, y + 1),std::make_pair(x + 1, y + 1) };
	}

	bool IsInBoard(int x, int y) { //проверяем лежит ли точка (x, y) на границе шестиугольника.
		return (x == 0) || (x == 2 * size - 2) || (y == 0) || (y == 2 * size - 2) || (x - y == size - 1) || (y - x == size - 1);
	}

	bool CheckPercolA() {
		NotVisited = GoodA;
		Perc = false;
		dfs(size - 1, size - 1);
		return Perc;
	}

	bool CheckPercolB() {
		NotVisited = GoodB;
		Perc = false;
		dfs(size - 1, size - 1);
		return Perc;
	}

	bool CheckPercolC() {
		NotVisited = GoodC;
		Perc = false;
		dfs(size - 1, size - 1);
		return Perc;
	}

	void dfs(int x, int y) {
		std::set<std::pair<int, int>> part;
		std::set<std::pair<int, int>> Peers = GetNeighbours(x, y);
		// описываем пересечение множества соседей и множества непосещенных вершин.
		for (auto it = Peers.begin(); it != Peers.end(); ++it) {
			if (NotVisited.find(*it) != NotVisited.end()) {
				part.insert(*it);
				NotVisited.erase(*it);
			}
		}
		for (auto it = part.begin(); it != part.end(); ++it) {
			if (IsInBoard(it->first, it->second)) {
				Perc = true;
			} 
			else {
				if (!Perc) {
					dfs(it->first, it->second);
				}
			};
		}
	}
public:
	explicit HexLattice(int n)
	{

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distribution(0, 3);
		size = n;
		Perc = false;
		RandState = distribution(gen);
		for (int i = 0; i <= size - 1; i++) {
			for (int j = 0; j <= size + i - 1; j++) {
				EveryCel.insert(std::make_pair(i, j));
			}
		}
		for (int i = size; i <= 2 * size - 2; i++) {
			for (int j = i - size + 1; j <= 2 * size - 2; j++) {
				EveryCel.insert(std::make_pair(i, j));
			}
		}
		assert(EveryCel.size() == 3 * size * (size - 1) + 1);
	}
	void MakeGuesses(int NumberOfGesses, int* Triple, int* Double, int* OnlyOne) {
		int Three = 0;
		int Two = 0;
		int One = 0;
		for (int i = 1; i <= NumberOfGesses; ++i) {
			InitProbSubs();
			bool PercA = CheckPercolA();
			bool PercB = CheckPercolB();
			bool PercC = CheckPercolC();
			if (PercA&&PercB&&PercC) {
				Three++;
			}
			else if (((!PercA) && PercB && PercC) || (PercA && (!PercB) && PercC) || (PercA && PercB && (!PercC))) {
				Two++;
			}
			else if (!((!PercA) && (!PercB) && (!PercC))) {
				One++;
				}
		}
		*Triple = Three;
		*Double = Two;
		*OnlyOne = One;
	}

	int SetConfigAndCheck(std::set<std::pair<int, int>> TypeA, std::set<std::pair<int, int>> TypeB) {
		std::set<std::pair<int, int>> All = SetIntersect(TypeA, TypeB);
		GoodA = SetUnion(TypeA, All);
		GoodB = SetUnion(TypeB, All);
		GoodC = SetUnion(SetDiffer(EveryCel, SetUnion(TypeA, TypeB)), All);
		bool ForA = false;
		bool ForB = false;
		bool ForC = false;
		int res = 0;
		ForA = CheckPercolA();
		ForB = CheckPercolB();
		ForC = CheckPercolC();
		if (ForA) {
			res += 4;
		}
		if (ForB) {
			res += 2;
		}
		if (ForC) {
			res += 1;
		}
		return res;
	}
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

void test_configuration(std::string testName, int size, bool expected_result)
{
    std::shared_ptr<HexLattice> lattice = 
        std::make_shared<HexLattice>(size);
	std::set<std::pair<int, int>> One;
	One.clear();
	std::set<std::pair<int, int>> Two;
	Two.clear();
    int ind = lattice->SetConfigAndCheck(One, Two);

    if (expected_result == (ind == 1 || (size == 1 && ind == 0))) {
        reportOk(testName);
    }
    else {
        reportFail(testName);
    }
}

void test_MKS(bool expected_result)
{
	std::shared_ptr<HexLattice> lattice =
		std::make_shared<HexLattice>(2);
	int One = 0;
	int Two = 0;
	int Three = 0;
	lattice->MakeGuesses(200, &Three, &Two, &One);

	if (expected_result == (One + Two + Three == 200)) {
		reportOk("MKS_test");
	}
	else {
		reportFail("MKS_test");
	}
}

void test_k_circle(std::string testName, int size, int len_from_center, bool expected_result) {
	std::shared_ptr<HexLattice> lattice =
		std::make_shared<HexLattice>(size);
	std::set<std::pair<int, int>> One;
	One.clear();
	for (int i = -1 - len_from_center; i <= -1; ++i) {
		One.insert(std::make_pair(size + i, size - 1 - len_from_center));
		One.insert(std::make_pair(size + i, size + i + len_from_center));
		One.insert(std::make_pair(size - 1 - len_from_center, size + i));
		One.insert(std::make_pair(size + i + len_from_center, size + i));
	}
	for (int i = size; i <= size - 1 + len_from_center; ++i) {
		One.insert(std::make_pair(i, size - 1 + len_from_center));
		One.insert(std::make_pair(size - 1 + len_from_center, i));
	}
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
    test_configuration("test size=1", 1, true);
	test_configuration("test size=2", 2, true);
	test_configuration("test size=3", 3, true);
	test_configuration("test size=100", 100, true);
	test_k_circle("test circle size 100", 100, 5, true);
	test_k_circle("test circle size 1", 1, 1, true);
	test_k_circle("test circle size 2", 2, 1, false);
	test_k_circle("test circle size 3", 3, 1, true);
	test_k_circle("test circle size 4", 4, 3, true);
	test_MKS(true);
    const bool allOk = numberOfFailedTests == 0;
    if (allOk) {
        std::cout << "All tests OK: " << numberOfSuccessfullTests << " OK out of " << numberOfTests << "\n";
        return 0;
    }
    else {
        std::cout << "Tests FAILED!!!: " << numberOfSuccessfullTests << " OK, " << numberOfFailedTests << " FAILED " << "out of " << numberOfTests << "\n";
        return 1;
    }
}

///////////////

int main(int argc, char* argv[])
{
    if (argc == 2) {
        if (std::string(argv[1]) == "--test") {
            const int testResult = run_all_tests();
            return testResult;
        }
    }


	int a=0;
	std::cin >> a;
	for (int i = 2; i <= a; ++i) {
		std::shared_ptr<HexLattice> qq = std::make_shared<HexLattice>(i);
		int One, Two, Three;
		qq->MakeGuesses(3000, &Three, &Two, &One);
		std::cout <<"Edge "<<i<<" -> No: "
			<< (3000-Three-Two-One)/3000.0
			<< "; One: "<< One/3000.0
			<< "; Two:  " << Two/3000.0
			<< "; Tree: " << Three/3000.0 << "\n";
		qq = 0;
	}
	std::cout << a;
	return 0;
}



std::set<std::pair<int, int>> SetUnion(std::set<std::pair<int, int>> First, std::set<std::pair<int, int>> Sec) {
	std::set<std::pair<int, int>> res = Sec;
	for (auto it = First.begin(); it != First.end(); ++it) {
		if (Sec.find(*it) == Sec.end()) {
			res.insert(*it);
		}
	}
	return res;
}

std::set<std::pair<int, int>> SetIntersect(std::set<std::pair<int, int>> First, std::set<std::pair<int, int>> Sec) {
	std::set<std::pair<int, int>> res;
	res.clear();
	for (auto it = First.begin(); it != First.end(); ++it) {
		if (Sec.find(*it) != Sec.end()) {
			res.insert(*it);
		}
	}
	return res;
}

std::set<std::pair<int, int>> SetDiffer(std::set<std::pair<int, int>> First, std::set<std::pair<int, int>> Sec) {
	std::set<std::pair<int, int>> res;
	res.clear();
	for (auto it = First.begin(); it != First.end(); ++it) {
		if (Sec.find(*it) == Sec.end()) {
			res.insert(*it);
		}
	}
	return res;
}