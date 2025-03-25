#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#define P_CORRECT 60 //веро€тность правильного решени€ среднего студента (в процентах)

using std::string;
using std::vector;
using std::ifstream;
using std::cout;

//квадратное уравнение ax^2 + bx + c
class Equation {
public:
	double a, b, c;
	bool isValid();
};

/* проверка корректности уравнени€
*
* возвращает false, если все коэффициенты уравнени€ нулевые, и true во всех остальных случа€х
*/
bool Equation::isValid() {
	if (a == 0 && b == 0 && c == 0) {
		return false;
	}
	else {
		return true;
	}
}

//ответ студента
class Answer {
public:
	string studName; //‘»ќ студента
	vector<vector<double>> solutions; //корни уравнений
};

//человек - содержит методы чтени€ и решени€ квадратных уравнений
class Person {
protected:
	Equation ReadEq(ifstream& fin); //чтение уравнени€ из файла
	vector<double> EqSolution(Equation& eq); //решени€ квадратного уравнени€ (правильное)
};

Equation Person::ReadEq(ifstream& fin) {
	double coeffs[3];
	for (int i = 0; i < 3; i++) {
		fin >> coeffs[i];
		if (fin.fail()) {
			for (int j = 0; j < 3; j++) {
				coeffs[j] = 0;
			}
			break;
		}
	}
	Equation eq = { coeffs[0], coeffs[1], coeffs[2] };
	return eq;
}

vector<double> Person::EqSolution(Equation& eq) {
	vector<double> sol;
	if (eq.a == 0) {
		return sol;
	}

	double d = eq.b * eq.b - 4 * eq.a * eq.c;
	if (d > 0) {
		sol.push_back((-eq.b - pow(d, 0.5)) / (2 * eq.a));
		sol.push_back((-eq.b + pow(d, 0.5)) / (2 * eq.a));
	}

	else if (d == 0) {
		sol.push_back(-eq.b / (2 * eq.a));
	}

	return sol;
}

//студент 
class Student : public Person {
public:
	string name; //‘»ќ студента
	virtual Answer Solve(string taskFilename) = 0; //решение квадратных уравнений из файла
	virtual ~Student() {};
};

//хороший студент
class Good : public Student {
public:
	Good(string studName) { name = studName; };
	Answer Solve(string taskFilename); //решение квадратных уравнений из файла
};

//решение уравнений из файла хорошим студентом - всегда решает правильно
Answer Good::Solve(string taskFilename) {
	Answer ans;
	ans.studName = name;

	ifstream fin(taskFilename);
	Equation eq = ReadEq(fin);
	while (eq.isValid()) {
		vector<double> sol = EqSolution(eq);
		ans.solutions.push_back(sol);
		eq = ReadEq(fin);
	}
	fin.close();
	return ans;
}

//средний студент
class Average : public Student {
public:
	Average(string studName) { name = studName; };
	Answer Solve(string taskFilename); //решение квадратных уравнений из файла
};

/* решение уравнений из файла средним студентом
*
* с веро€тностью P_CORRECT (в процентах) решает правильно, в остальных случа€х записывает
* один корень, равный 0
*/
Answer Average::Solve(string taskFilename) {
	srand(time(NULL));
	Answer ans;
	ans.studName = name;

	ifstream fin(taskFilename);
	Equation eq = ReadEq(fin);
	while (eq.isValid()) {
		int randNum = rand() % 100 + 1;
		vector<double> sol;
		if (randNum <= P_CORRECT) {
			sol = EqSolution(eq);
		}
		else {
			sol.push_back(0);
		}
		ans.solutions.push_back(sol);
		eq = ReadEq(fin);
	}
	fin.close();
	return ans;
}

//плохой студент
class Bad : public Student {
public:
	Bad(string studName) { name = studName; };
	Answer Solve(string taskFilename); //решение квадратных уравнений из файла
};

/* решение уравнений из файла плохим студентом
*
* всегда один корень, равный 0
*/
Answer Bad::Solve(string taskFilename) {
	Answer ans;
	ans.studName = name;

	ifstream fin(taskFilename);
	Equation eq = ReadEq(fin);
	while (eq.isValid()) {
		vector<double> sol = { 0 };
		ans.solutions.push_back(sol);
		eq = ReadEq(fin);
	}
	fin.close();
	return ans;
}