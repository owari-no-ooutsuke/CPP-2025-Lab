#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#define P_CORRECT 60 //вероятность правильного решения среднего студента (в процентах)
#define N 10 //количество студентов

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

//студент 
class Student : public Person {
public:
	string name; //ФИО студента
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
* с вероятностью P_CORRECT (в процентах) решает правильно, в остальных случаях записывает
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

//результат проверки работы студента преподавателем
class Result {
public:
	string studName; //ФИО студента
	int correctAnsCount; //количество правильных ответов
};

//преподаватель
class Teacher : public Person {
private:
	vector<Answer> queue; //очередь из работ студентов
	vector<Result> res; //результаты проверки
	vector<vector<double>> Solve(string taskFilename); //решения квадратных уравений (правильное)
public:
	void AddToQueue(Answer ans); //добавить работу в очередь
	void ClearQueue(); //очистить очередь
	void Check(string taskFilename); //проверка работ, находящихся в очереди
	int EqCount(string taskFilename); //подсчет количества уравнений в задании 
	void ShowResults(string taskFilename); //вывод результатов проверки
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

/* создание студентов
*
* возвращает массив указателей на базовый класс Student
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