#include <iostream>
#include <fstream>
#include <set>
#include "EmployeeRequest.h"

Employee getValidEmployee(std::set<int>& employeesIDs);
int getValidNum();
void getValidName(char* employeeName);
double getValidHours();

int main(int argc, char* argv[])
{

	std::ofstream file(argv[1]);
	int recordsNumber = atoi(argv[2]);

	std::set<int> employeesIDs;

	Employee Employee;
	for (int i = 1; i <= recordsNumber; i++) {
		std::cout << "Employee " << i << ": " << std::endl;

		Employee = getValidEmployee(employeesIDs);

		file.write((char*)&Employee, sizeof(Employee));

		std::cout << "\n";
	}

	file.close();
}
Employee getValidEmployee(std::set<int>& employeesIDs) {

	Employee Employee;
	Employee.num = getValidNum();
	while (employeesIDs.find(Employee.num) != employeesIDs.end()) {
		std::cout << "Employee with given ID already exists, please enter another ID:" << std::endl;
		Employee.num = getValidNum();
	}
	employeesIDs.insert(Employee.num);
	getValidName(Employee.name);
	Employee.hours = getValidHours();

	return Employee;
}

int getValidNum() {
	double num;
	while (true)
	{
		std::cout << "num: ";
		if (std::cin >> num && num == (int)num) {
			return num;
		}
		else {
			std::cout << "Invalid Input! Please input an integer value." << std::endl;
			std::cin.clear();
			while (std::cin.get() != '\n');
		}
	}
}
void getValidName(char* employeeName)
{
	std::string name;
	int nameLength = 10;
	while (true) {
		std::cout << "name: ";
		std::cin >> name;
		if (name.length() <= nameLength)
		{
			strcpy_s(employeeName, nameLength + 1, name.c_str());
			return;
		}
		else {
			std::cout << "Invalid Input! Please input a value less or equal 10 chars." << std::endl;
		}

	}
}
double getValidHours() {
	double hours;
	while (true)
	{
		std::cout << "hours: ";
		if (std::cin >> hours)
		{
			return hours;
		}
		else
		{
			std::cout << "Invalid Input! Please input a double value." << std::endl;
			std::cin.clear();
			while (std::cin.get() != '\n');
		}
	}
}
