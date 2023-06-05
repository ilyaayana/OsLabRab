#include <iostream>
#include <string>
#include <Windows.h>
#include "EmployeeRequest.h"

void handleRequest(HANDLE hNamedPipe, RequestType type);

void showEmployeeInfo(const Employee);
void sendRequestByPipe(HANDLE hNamedPipe, Request Request);

void finishRequest(HANDLE hNamedPipe);

int getValidNum();

int main()
{
    std::string pipeName = "\\\\.\\pipe\\employeesPipe";
    HANDLE hNamedPipe = CreateFile(
        pipeName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        (LPSECURITY_ATTRIBUTES)NULL,
        OPEN_EXISTING,
        0,
        (HANDLE)NULL
    );
    if (hNamedPipe == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Connection with the named pipe failed." << std::endl
            << "The last error code: " << GetLastError() << std::endl;
        system("pause");
        return 0;
    }

    int choice;
    do {
        std::cout << "\nYou have the following options:\n"
            "1. edit record\n"
            "2. get record\n"
            "3. Stop working\n"
            "Enter your choice: " << std::endl;
        choice = getValidNum();
        switch (choice) {
        case 1: 
            handleRequest(hNamedPipe,EDIT);
            break;
        case 2:
            handleRequest(hNamedPipe,READ);
            break;
        case 3:
            std::cout << "Finishing work" << std::endl;
            break;
        default:
            std::cout << "Invalid choice. Please try again" << std::endl;
            break;
        }
    } while (choice != 3);

    CloseHandle(hNamedPipe);
}

void handleRequest(HANDLE hNamedPipe,RequestType type)
{
    int employeeID;
    std::cout << "Enter ID of the record: ";
    std::cin >> employeeID;

    Request request = { type, employeeID };
    sendRequestByPipe(hNamedPipe,request);

    Employee Employee = getEmployeeInfoFromPipe(hNamedPipe);
    showEmployeeInfo(Employee);

    if (type == EDIT) {
        std::cout << "\nEnter new data: " << std::endl;
        std::cout << "name: ";
        std::cin >> Employee.name;
        std::cout << "hours: ";
        std::cin >> Employee.hours;
        sendEmployeeInfoByPipe(hNamedPipe,Employee);
        std::cout << "Updated info sent!" << std::endl;
    }

    system("pause");
    finishRequest(hNamedPipe);
     
}


void sendRequestByPipe(HANDLE hNamedPipe, Request request) {
    if (!WriteFile(hNamedPipe, (char*)&request, sizeof(request), 0, (LPOVERLAPPED)NULL)) {
        std::cerr << "Data writing to the named pipe failed."
            << "The last error code: " << GetLastError() << std::endl;
        CloseHandle(hNamedPipe);
        system("pause");
        return;
    }
}
void finishRequest(HANDLE hNamedPipe) {
    int code = 1;
    if(!WriteFile(hNamedPipe, (char*)&code, sizeof(int), 0, (LPOVERLAPPED)NULL)) {
        std::cerr << "Data writing to the named pipe failed." 
            << "The last error code: " << GetLastError() << std::endl;
        CloseHandle(hNamedPipe);
        system("pause");
        return;
    }
}

void showEmployeeInfo(const Employee Employee)
{
    std::cout << "**********************************************" << std::endl;
    std::cout << "num: " << Employee.num << std::endl;
    std::cout << "name: " << Employee.name << std::endl;
    std::cout << "hours: " << Employee.hours << std::endl;
}

int getValidNum() {
    double num;
    while (true)
    {
        std::cout << "num: ";
        if (std::cin >> num && num == (int)num)
        {
            return num;
        }
        else
        {
            std::cout << "Invalid Input! Please input an integer value." << std::endl;
            std::cin.clear();
            while (std::cin.get() != '\n');
        }
    }
}