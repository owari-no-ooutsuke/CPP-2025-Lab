#include <iostream>
#include <cmath>
#include <fstream>

using namespace std;

//��������� ax^2 + bx + c
typedef struct {
	double a;
	double b;
	double c;
} quadEq;

//����� ���������
typedef struct {
	int rootCount; //���������� ������
	double* roots; //������ ������
} eqSolution;

/* ������ ��������� �� �����
* 
* ������ ��������� �����������, ���� ����� � ����� �������� ����� ������������ ���������
* ���������� �������� �/��� ��������� ������
* � ������ ������������ ������ ������������ ��������� � �������� ��������������
*/
quadEq ReadEquation(string const filename) {
	ifstream fin(filename);
	double coeffs[3] = { 0 };
	for (int i = 0; i < 3; i++) {
		fin >> coeffs[i];
		if (fin.fail()) {
			cout << "Invalid data!";
			for (int j = 0; j < i; j++) {
				coeffs[j] = 0;
			}
			break;
		}
	}
	fin.close();

	quadEq eq = { coeffs[0], coeffs[1], coeffs[2] };
	return eq;
}

//���������� �������������
double Discriminant(quadEq const &eq) {
	return eq.b * eq.b - 4 * eq.a * eq.c;
}

//���������� ������
eqSolution Solution(quadEq const &eq) {
	eqSolution sol;
	if (eq.a == 0) {
		sol.rootCount = -1;
		sol.roots = nullptr;
		return sol;
	}

	double d = Discriminant(eq);
	if (d > 0) {
		sol.rootCount = 2;
		sol.roots = (double*)malloc(sizeof(double) * 2);
		sol.roots[0] = (-eq.b - pow(d, 0.5)) / (2 * eq.a);
		sol.roots[1] = (-eq.b + pow(d, 0.5)) / (2 * eq.a);
	}

	else if (d == 0) {
		sol.rootCount = 1;
		sol.roots = (double*)malloc(sizeof(double));
		sol.roots[0] = -eq.b / (2 * eq.a);
	}

	else {
		sol.rootCount = 0;
		sol.roots = nullptr;
	}

	return sol;
}

//������ ������ � ����
void WriteSolution(eqSolution const& sol, string const filename) {
	ofstream fout(filename);
	if (sol.rootCount == -1) {
		fout << "Invalid quadratic equation";
	}
	else if (sol.rootCount == 0) {
		fout << "No real roots";
	}
	else {
		for (int i = 0; i < sol.rootCount; i++) {
			fout << sol.roots[i] << " ";
		}
	}
	fout.close();
}

int main() {
	//��� ������ 
	string inputFilename = "eq_coeffs.txt";
	ofstream fout(inputFilename);
	fout << "1 1 10";
	fout.close();

	quadEq eq = ReadEquation(inputFilename);
	eqSolution sol = Solution(eq);
	WriteSolution(sol, "output_no_roots.txt");

	//���� ������
	fout.open(inputFilename);
	fout << "1 4 4";
	fout.close();

	eq = ReadEquation(inputFilename);
	sol = Solution(eq);
	WriteSolution(sol, "output_one_root.txt");

	//��� �����
	fout.open(inputFilename);
	fout << "1 -1 -2";
	fout.close();

	eq = ReadEquation(inputFilename);
	sol = Solution(eq);
	WriteSolution(sol, "output_two_roots.txt");

	//������������ ����
	fout.open(inputFilename);
	fout << "1,,, -1 -2";
	fout.close();

	eq = ReadEquation(inputFilename);
	sol = Solution(eq);
	WriteSolution(sol, "output_invalid_coeffs.txt");
	return 0;
}