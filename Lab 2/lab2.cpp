#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>

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

/* проверка корректности уравнения
*
* возвращает false, если все коэффициенты уравнения нулевые, и true во всех остальных случаях
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
	string studName; //ФИО студента
	vector<vector<double>> solutions; //корни уравнений
};

//человек - содержит методы чтения и решения квадратных уравнений
class Person {
protected:
	Equation ReadEq(ifstream& fin); //чтение уравнения из файла
	vector<double> EqSolution(Equation& eq); //решения квадратного уравнения (правильное)
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
