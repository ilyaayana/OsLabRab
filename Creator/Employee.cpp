#include <string>
#include <fstream>
#include <iostream>
#include <Windows.h>
#include "EmployeeRequest.h"

using namespace std;

Employee getEmployeeByID(const string fileNameEmployees, int employeeID)
{
	ifstream fileEmployees(fileNameEmployees, ios::binary);
	if (!fileEmployees) {
		throw runtime_error("Error occured while opening the file " + fileNameEmployees);
	}


	Employee Employee;
	while (fileEmployees.read((char*)&Employee, sizeof(Employee))){
		if (Employee.num == employeeID) {
			fileEmployees.close();
			return Employee;
		}
	}

	throw invalid_argument("no employees with given ID");
}
void editEmployeeRecord(const string fileNameEmployees, Employee editedEmployee)
{
	ifstream fileEmployees(fileNameEmployees, ios::binary);
	if (!fileEmployees) {
		cout << "Error occured while opening the file " << fileNameEmployees << endl;
		return;
	}

	ofstream tmpFile("temp.bin", std::ios::binary);
	Employee Employee;
	while (fileEmployees.read((char*)&Employee, sizeof(Employee)))
	{
		if (editedEmployee.num == Employee.num) {
			tmpFile.write((char*)&editedEmployee, sizeof(Employee));
		}
		else {
			tmpFile.write((char*)&Employee, sizeof(Employee));
		}
	}

	fileEmployees.close();
	tmpFile.close();

	remove(fileNameEmployees.c_str());
	rename("temp.bin", fileNameEmployees.c_str());

}
void sendEmployeeInfoByPipe(HANDLE hNamedPipe, Employee Employee)
{
	if (!WriteFile(hNamedPipe, (char*)&Employee, sizeof(Employee), 0, (LPOVERLAPPED)NULL)) {
		cerr << "Data writing to the named pipe failed." << endl
			<< "The last error code: " << GetLastError() << endl;
		CloseHandle(hNamedPipe);
		system("pause");
		return;
	}
}
Employee getEmployeeInfoFromPipe(HANDLE hNamedPipe) {
	Employee Employee;
	if (!ReadFile(hNamedPipe, (char*)&Employee, sizeof(Employee), 0, (LPOVERLAPPED)NULL))
	{
		cerr << "Data reading from the named pipe failed." << endl
			<< "The last error code: " << GetLastError() << endl;
		CloseHandle(hNamedPipe);
		system("pause");
		throw runtime_error("");
	}
	return Employee;
}
