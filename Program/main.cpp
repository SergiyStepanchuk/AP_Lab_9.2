// Lab_9_2
#include <iostream>
#include <iomanip>
#include <windows.h>
#include <string>
#include <algorithm>
using namespace std;

// Console 

namespace cons {

	COORD GetBufferSize() {
		static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbInfo;
		GetConsoleScreenBufferInfo(handle, &csbInfo);
		return { csbInfo.srWindow.Right - csbInfo.srWindow.Left ,
				csbInfo.srWindow.Bottom - csbInfo.srWindow.Top };
	}

	const COORD size = GetBufferSize();

	void clear() {
		system("cls");
	}

	void gotoxy(const COORD pos) {
		static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleCursorPosition(handle, pos);
	}

	COORD getxy() {
		static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (!GetConsoleScreenBufferInfo(handle, &csbi))
			return { -1, -1 };
		return csbi.dwCursorPosition;
	}

	void clearto(const COORD pos) {
		COORD current_pos = getxy();
		while (current_pos.Y >= pos.Y)
		{
			if (current_pos.Y > pos.Y) {
				gotoxy({ 0, current_pos.Y });
				for (int i = 0; i < size.X; i++)
					cout << ' ';
			}
			else if (current_pos.Y == pos.Y) {
				gotoxy({ pos.X, current_pos.Y });
				for (int i = 0; i <= size.X - pos.X; i++)
					cout << ' ';
			}

			current_pos.Y--;
		}
		gotoxy(pos);
	}

	void change_cusor_visibility(const bool& rst) {
		static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO structCursorInfo;
		GetConsoleCursorInfo(handle, &structCursorInfo);
		structCursorInfo.bVisible = rst;
		SetConsoleCursorInfo(handle, &structCursorInfo);
	}

	template <typename T>
	T input(bool (*check)(T& temp, char* err), const bool& rom, const char* text, ...) {
		COORD start[2] = { getxy() };
		char out[256] = { 0 }, err[256] = { 0 };
		T temp;

		va_list  args;
		va_start(args, text);
		vsprintf_s(out, 255, text, args);
		va_end(args);
		cout << out;
		start[1] = getxy();
		if (check == nullptr)
			check = [](T& temp, char* err) -> bool { return !cin.fail(); };
		do {
			cin.clear();
			cin.ignore();
			if (err[0] != '\0') {
				change_cusor_visibility(false);
				clearto(start[0]);
				cout << err << endl;
				err[0] = '\0';
				cout << out;
				start[1] = getxy();
				change_cusor_visibility(true);
			}
			else clearto(start[1]);
			cin >> temp;
		} while (!check(temp, err));
		if (rom)
			clearto(start[0]);
		return temp;
	}
}

enum StudentProfession : unsigned short {
	StudentProfession_ComputerScience = 0, // Комп’ютерні науки // програмування
	StudentProfession_Informatics, // Інформатика // чисельних методів
	StudentProfession_MathematicsEconomics, // Математика та економіка // педагогіки
	StudentProfession_PhysicsComputerScience, //  Фізика та інформатика // педагогіки
	StudentProfession_WorkTraining // Трудове навчання // педагогіки
};

struct Student {
	string lastname; // прізвище
	unsigned short course; // курс
	StudentProfession profession; // спеціальність

	unsigned short mark_physics; // фізика
	unsigned short mark_math; // математика
	union
	{
		unsigned short programming; // програмування
		unsigned short numerical_methods; // чисельних методів
		unsigned short pedagogy; // педагогіки
	};
};

Student* studentFactory(const int& count) {
	static auto check_course = [](unsigned short& temp, char* err) -> bool {
		if (cin.fail() ||
			temp < 1 ||
			temp > 10)
		{
			sprintf_s(err, 255, "Incorrect cource(%hu)", temp);
			return false;
		}
		return true;
	};

	static auto check_profession = [](unsigned short& temp, char* err) -> bool {
		if (cin.fail() ||
			temp > 4)
		{
			sprintf_s(err, 255, "Incorrect profession(%hu)", temp);
			return false;
		}
		return true;
	};

	static auto check_mark = [](unsigned short& temp, char* err) -> bool {
		if (cin.fail() ||
			temp > 5)
		{
			sprintf_s(err, 255, "Incorrect mark(%hu)", temp);
			return false;
		}
		return true;
	};

	Student* st = new Student[count];
	int prof;
	COORD pos = cons::getxy();
	for (int i = 0; i < count; i++)
	{
		cout << "Student № " << i + 1 << ":" << endl;
		st->lastname = cons::input<string>(nullptr, false, "Lastname: ");
		st->course = cons::input<unsigned short>(check_course, false, "Course  : ");
		cout << "Professions:\n0 - Computer science\n1 - Informatics\n2 - Mathematics and economics\n3 - Physics and computer science\n4 - Work training\n";
		st[i].profession = (StudentProfession)cons::input<unsigned short>(check_profession, false, "Select profession: ");
		switch (st[i].profession)
		{
		case StudentProfession_ComputerScience:
			st[i].programming = cons::input<unsigned short>(check_mark, false, "Mark of programming: ");
			break;
		case StudentProfession_Informatics:
			st[i].programming = cons::input<unsigned short>(check_mark, false, "Mark of numerical methods: ");
			break;
		case StudentProfession_MathematicsEconomics:
		case StudentProfession_PhysicsComputerScience:
		case StudentProfession_WorkTraining:
			st[i].programming = cons::input<unsigned short>(check_mark, false, "Mark of pedagogy: ");
		}
		st[i].mark_physics = cons::input<unsigned short>(check_mark, false, "Mark of physics: ");
		st[i].mark_math = cons::input<unsigned short>(check_mark, false, "Mark of math: ");
		cons::clearto(pos);
	}
	return st;
}

void darwStudents(Student* st, const int& count)
{
	cout << "================================================================================================================" << endl;
	cout << "| № | Lastname | Course | Profession               | Physics | Math | Programming | Numerical methods | Pedagogy" << endl;
	cout << "----------------------------------------------------------------------------------------------------------------" << endl;
	for (int i = 0; i < count; i++)
	{
		cout << "| " << setw(2) << i + 1
			<< "| " << setw(9) << st[i].lastname
			<< "| " << setw(7) << st[i].course;

		switch (st[i].profession)
		{
		case StudentProfession_ComputerScience:
			cout << "| " << setw(28) << "Computer science";
			break;
		case StudentProfession_Informatics:
			cout << "| " << setw(28) << "Informatics";
			break;
		case StudentProfession_MathematicsEconomics:
			cout << "| " << setw(28) << "Mathematics and economics";
		case StudentProfession_PhysicsComputerScience:
			cout << "| " << setw(28) << "Physics and computer science";
		case StudentProfession_WorkTraining:
			cout << "| " << setw(28) << "Work training";
		}
		cout << "| " << setw(8) << st[i].mark_physics
			<< "| " << setw(5) << st[i].mark_math;

		switch (st[i].profession)
		{
		case StudentProfession_ComputerScience:
			cout << "| " << setw(12) << st[i].programming
				<< "| " << setw(18) << " "
				<< "| " << setw(8) << " ";
			break;
		case StudentProfession_Informatics:
			cout << "| " << setw(12) << " "
				<< "| " << setw(18) << st[i].programming
				<< "| " << setw(8) << " ";
			break;
		case StudentProfession_MathematicsEconomics:
		case StudentProfession_PhysicsComputerScience:
		case StudentProfession_WorkTraining:
			cout << "| " << setw(12) << " "
				<< "| " << setw(18) << " "
				<< "| " << setw(8) << st[i].programming;
		}
		cout << endl;
	}
	cout << "================================================================================================================" << endl;
}

int* indexed_sort(Student* st, const int count)
{ 
	int* I = new int[count]; // створили індексний масив
	for (int i = 0; i < count; i++)
		I[i] = i; 
	sort(I, I + count, [&](const int& a1, const int& a2) -> bool {
			int avarage1 = (st[a1].mark_math + st[a1].mark_physics + st[a1].programming) / 3;
			int avarage2 = (st[a2].mark_math + st[a2].mark_physics + st[a2].programming) / 3;
			if (avarage1 > avarage2 ||
				(avarage1 == avarage2 && st[a1].lastname > st[a2].lastname) ||
				(avarage1 == avarage2 && st[a1].lastname == st[a2].lastname && st[a1].course > st[a2].course))
				return true; // міняємо
			return false;
		}
	);
	return I;
}

void print_indexed_sort(Student* st, int* I, const int &count)
{
	cout << "================================================================================================================" << endl;
	cout << "| № | Lastname | Course | Profession               | Physics | Math | Programming | Numerical methods | Pedagogy" << endl;
	cout << "----------------------------------------------------------------------------------------------------------------" << endl;
	for (int i = 0; i < count; i++)
	{
		cout << "| " << setw(2) << I[i] + 1
			<< "| " << setw(9) << st[I[i]].lastname
			<< "| " << setw(7) << st[I[i]].course;

		switch (st[I[i]].profession)
		{
		case StudentProfession_ComputerScience:
			cout << "| " << setw(28) << "Computer science";
			break;
		case StudentProfession_Informatics:
			cout << "| " << setw(28) << "Informatics";
			break;
		case StudentProfession_MathematicsEconomics:
			cout << "| " << setw(28) << "Mathematics and economics";
		case StudentProfession_PhysicsComputerScience:
			cout << "| " << setw(28) << "Physics and computer science";
		case StudentProfession_WorkTraining:
			cout << "| " << setw(28) << "Work training";
		}
		cout << "| " << setw(8) << st[I[i]].mark_physics
			<< "| " << setw(5) << st[I[i]].mark_math;

		switch (st[I[i]].profession)
		{
		case StudentProfession_ComputerScience:
			cout << "| " << setw(12) << st[I[i]].programming
				<< "| " << setw(18) << " "
				<< "| " << setw(8) << " ";
			break;
		case StudentProfession_Informatics:
			cout << "| " << setw(12) << " "
				<< "| " << setw(18) << st[I[i]].programming
				<< "| " << setw(8) << " ";
			break;
		case StudentProfession_MathematicsEconomics:
		case StudentProfession_PhysicsComputerScience:
		case StudentProfession_WorkTraining:
			cout << "| " << setw(12) << " "
				<< "| " << setw(18) << " "
				<< "| " << setw(8) << st[I[i]].programming;
		}
		cout << endl;
	}
	cout << "================================================================================================================" << endl;
}

void sort_students(Student* st, const int& count) {
	sort(st, st + count, [&](const Student& a1, const Student& a2) -> bool {
			int avarage1 = (a1.mark_math + a1.mark_physics + a1.programming) / 3;
			int avarage2 = (a2.mark_math + a2.mark_physics + a2.programming) / 3;
			if (avarage1 > avarage2 ||
				(avarage1 == avarage2 && a1.lastname < a2.lastname) ||
				(avarage1 == avarage2 && a1.lastname == a2.lastname && a1.course > a2.course))
				return true; // міняємо
			return false;
		}
	);
}

/*int	bin_search_student(Student* st, const int& count, const string lastname, const unsigned short& course, const unsigned short& avarage) {
	int L = 0, R = count - 1, m;
	do {
		m = (L + R) / 2;
		if ((st[m].mark_math + st[m].mark_physics + st[m].programming) / 3 < avarage) 
			L = m + 1;
		else if ((st[m].mark_math + st[m].mark_physics + st[m].programming) / 3 == avarage) {
			if (st[m].lastname < lastname)
				L = m + 1;
			else if (st[m].lastname == lastname) {
				if (st[m].course < course)
					L = m + 1;
				else if (st[m].course == course)
					return m;
				else R = m - 1;
			}else R = m - 1;
		}else R = m - 1;
	} while (L <= R);
}*/

void main() {
	SetConsoleCP(1251); // встановлення сторінки win-cp1251 в потік вводу
	SetConsoleOutputCP(1251); // встановлення сторінки win-cp1251 в потік виводу
	cout.setf(ios_base::left);

	unsigned int state = 1, count = 3;
	Student* st;
	bool indexed_cout = false;
	char message[256] = {0};

	count = cons::input<unsigned int>([](unsigned int& temp, char* err) {
			if (cin.fail() ||
				temp < 1 ||
				temp > 20) {
				sprintf_s(err, 255, "Incorrect count(%d)!", temp);
				return false;
			}
			return true;
		}, true, "Input count of students (1 <= count <= 20): ");
	
	st = studentFactory(count);

	do {
		cons::clear();
		//cout << bin_search_student(st, count, "abcd max", 2, 3) << endl;
		if (!indexed_cout)
			darwStudents(st, count);
		else {
			indexed_cout = false;
			int* I = indexed_sort(st, count);
			print_indexed_sort(st, I, count);
			delete[] I;
		}
		if (message[0] != '\0') {
			cout << message << endl;
			message[0] = '\0';
		}
		cout << "[1] Sort by lastname" << endl
			<< "[2] Indexed cout" << endl
			<< "[3] cout" << endl
			<< "[0] Exit" << endl;
		state = cons::input<unsigned int>(nullptr, false, "Select action: ");
		switch (state)
		{
		case 1:
			sort_students(st, count);
			sprintf_s(message, 255, "Succesfully sorted by lastname");
			break;
		case 2:
			indexed_cout = true;
			sprintf_s(message, 255, "Succesfully indexed sort");
			break;
		case 3:
			break;
		default:
			sprintf_s(message, 255, "Incorrect action!");
		}
	} while (state > 0);
}