#pragma once
#include <iostream>
#include <Windows.h>

enum RequestType {
    EDIT,
    READ
};
struct Request {
    RequestType type;
    int employeeID;
};
struct Employee {
    int num;
    char name[10];
    double hours;
};

Employee getEmployeeByID(const std::string fileNameEmployees, int employeeID);
void editEmployeeRecord(const std::string fileNameEmployees, Employee editedEmployee);
void sendEmployeeInfoByPipe(HANDLE hNamedPipe, Employee Employee);
Employee getEmployeeInfoFromPipe(HANDLE hNamedPipe);