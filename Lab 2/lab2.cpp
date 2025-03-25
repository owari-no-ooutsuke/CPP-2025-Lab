#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#define P_CORRECT 60 //����������� ����������� ������� �������� �������� (� ���������)
#define N 10 //���������� ���������

using std::string;
using std::vector;
using std::ifstream;
using std::cout;

//���������� ��������� ax^2 + bx + c
class Equation {
public:
	double a, b, c;
	bool isValid();
};

/* �������� ������������ ���������
*
* ���������� false, ���� ��� ������������ ��������� �������, � true �� ���� ��������� �������
*/
bool Equation::isValid() {
	if (a == 0 && b == 0 && c == 0) {
		return false;
	}
	else {
		return true;
	}
}

//����� ��������
class Answer {
public:
	string studName; //��� ��������
	vector<vector<double>> solutions; //����� ���������
};

//������� - �������� ������ ������ � ������� ���������� ���������
class Person {
protected:
	Equation ReadEq(ifstream& fin); //������ ��������� �� �����
	vector<double> EqSolution(Equation& eq); //������� ����������� ��������� (����������)
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

//������� 
class Student : public Person {
public:
	string name; //��� ��������
	virtual Answer Solve(string taskFilename) = 0; //������� ���������� ��������� �� �����
	virtual ~Student() {};
};

//������� �������
class Good : public Student {
public:
	Good(string studName) { name = studName; };
	Answer Solve(string taskFilename); //������� ���������� ��������� �� �����
};

//������� ��������� �� ����� ������� ��������� - ������ ������ ���������
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

//������� �������
class Average : public Student {
public:
	Average(string studName) { name = studName; };
	Answer Solve(string taskFilename); //������� ���������� ��������� �� �����
};

/* ������� ��������� �� ����� ������� ���������
*
* � ������������ P_CORRECT (� ���������) ������ ���������, � ��������� ������� ����������
* ���� ������, ������ 0
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

//������ �������
class Bad : public Student {
public:
	Bad(string studName) { name = studName; };
	Answer Solve(string taskFilename); //������� ���������� ��������� �� �����
};

/* ������� ��������� �� ����� ������ ���������
*
* ������ ���� ������, ������ 0
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

//��������� �������� ������ �������� ��������������
class Result {
public:
	string studName; //��� ��������
	int correctAnsCount; //���������� ���������� �������
};

//�������������
class Teacher : public Person {
private:
	vector<Answer> queue; //������� �� ����� ���������
	vector<Result> res; //���������� ��������
	vector<vector<double>> Solve(string taskFilename); //������� ���������� �������� (����������)
public:
	void AddToQueue(Answer ans); //�������� ������ � �������
	void ClearQueue(); //�������� �������
	void Check(string taskFilename); //�������� �����, ����������� � �������
	int EqCount(string taskFilename); //������� ���������� ��������� � ������� 
	void ShowResults(string taskFilename); //����� ����������� ��������
};

vector<vector<double>> Teacher::Solve(string taskFilename) {
	vector<vector<double>> answers;
	ifstream fin(taskFilename);
	Equation eq = ReadEq(fin);
	while (eq.isValid()) {
		vector<double> sol = EqSolution(eq);
		answers.push_back(sol);
		eq = ReadEq(fin);
	}
	fin.close();
	return answers;
}

void Teacher::AddToQueue(Answer ans) {
	queue.push_back(ans);
}

void Teacher::ClearQueue() {
	queue.clear();
}

void Teacher::Check(string taskFilename) {
	res.clear();
	vector<vector<double>> answers = Solve(taskFilename);

	for (int i = 0; i < queue.size(); i++) {
		Result studRes;
		studRes.studName = queue[i].studName;
		int correctCount = 0;
		for (int j = 0; j < answers.size(); j++) {
			if (queue[i].solutions[j] == answers[j]) {
				correctCount++;
			}
		}
		studRes.correctAnsCount = correctCount;
		res.push_back(studRes);
	}
}

int Teacher::EqCount(string taskFilename) {
	ifstream fin(taskFilename);
	int count = 0;
	Equation eq = ReadEq(fin);
	while (eq.isValid()) {
		count++;
		vector<double> sol = EqSolution(eq);
		eq = ReadEq(fin);
	}
	fin.close();
	return count;
}

void Teacher::ShowResults(string taskFilename) {
	int eqCount = EqCount(taskFilename);
	int nameWidth = 0;
	int len;
	for (int i = 0; i < res.size(); i++) {
		len = res[i].studName.length();
		if (len > nameWidth) {
			nameWidth = len;
		}
	}

	cout << std::setw(nameWidth) << "Name" << " | Result" << std::endl;
	for (int i = 0; i < nameWidth + 10; i++) {
		cout << "-";
	}
	for (int i = 0; i < res.size(); i++) {
		cout << std::endl << std::setw(nameWidth) << res[i].studName << " | ";
		cout << res[i].correctAnsCount << " / " << eqCount << std::endl;
		for (int j = 0; j < nameWidth + 10; j++) {
			cout << "-";
		}
	}
}

/* �������� ���������
*
* ���������� ������ ���������� �� ������� ����� Student
*/
vector<Student*> CreateStudents(int count) {
	vector<Student*> students;
	srand(time(NULL));
	for (int i = 0; i < count; i++) {
		string name = "Student" + std::to_string(i);
		int randNum = rand() % 3;
		if (randNum == 0) {
			students.push_back(new Good(name));
		}
		else if (randNum == 1) {
			students.push_back(new Average(name));
		}
		else {
			students.push_back(new Bad(name));
		}
	}
	return students;
}

int main() {
	string task = "eq_coeffs.txt";
	Teacher t;
	vector<Student*> students = CreateStudents(N);
	for (int i = 0; i < N; i++) {
		t.AddToQueue((*students[i]).Solve(task));
	}
	t.Check(task);
	t.ShowResults(task);

	for (int i = 0; i < N; i++) {
		delete students[i];
	}
	return 0;
}