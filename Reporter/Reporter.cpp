#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include "Employee.h"

double generateSalary(double hours, double hourlyWage) 
{
    return hourlyWage * hours;
}

int main(int argc, char* argv[])
{
    std::string fileNameBin = argv[1];
    std::string fileNameReport = argv[2];

    std::ofstream fileReport(fileNameReport);
    if(!fileReport) {
        std::cout << "Error occured while opening the file " << fileNameReport << std::endl;
        return -1;
    }
    std::ifstream fileEmployees(fileNameBin, std::ios::binary);
    if (!fileEmployees) {
        std::cout << "Error occured while opening the file " << fileNameBin << std::endl;
        return -1;
    }

    fileReport << "Report on file " << fileNameBin << "\n" << std::endl;
    fileReport << "Employee number  Employee name  hours  salary\n";

    double hourlyWage = atof(argv[3]);
    employee Employee;
    while (fileEmployees.read((char*)&Employee, sizeof(employee)))
    {
        fileReport << std::left << std::setw(17) <<  Employee.num << std::setw(15) << Employee.name << std::setw(7) <<
            Employee.hours << std::setw(6) << generateSalary(Employee.hours, hourlyWage) << std::endl;
    }

    fileReport.close();
    fileEmployees.close();
}