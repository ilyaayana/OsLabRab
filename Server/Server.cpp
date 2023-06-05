#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <map>
#include <Windows.h>
#include "EmployeeRequest.h"


void launchCreator(const std::string fileNameEmployees,const int recordsNumber);
void launchProcess(const std::string Args);
void showBinaryFileInfo(const std::string fileNameEmployees);

void clientRequestThread(void* params);
void waitForRequestToFinish(HANDLE hNamedPipe);

struct Parameters
{
	std::map<int, int>& readersCount;
	int clientCount;
	std::string fileNameEmployees;
};



int main()
{
	std::string fileNameEmployees;
	int recordsNumber;
	std::cout << "Enter binary file name: ";
	std::cin >> fileNameEmployees;
	std::cout << "Enter the number of records: ";
	std::cin >> recordsNumber;

	launchCreator(fileNameEmployees, recordsNumber);

	showBinaryFileInfo(fileNameEmployees);


	std::map<int, int> readersCount;
	for (int i = 1; i <= recordsNumber;i++) 
	{
		readersCount[recordsNumber + 1] = 0;
		std::string semName = "EmployeeFileSemaphoreRecord" + i;
		std::string mutName = "EmployeeFileMutexRecord" + i;
		if (!CreateSemaphore(NULL, 1, 1, semName.c_str())) {
			std::cout << "Error when creating semaphore: Error code " << GetLastError() << std::endl;
			return -1;
		}
		if(!CreateMutex(NULL, false, mutName.c_str())){
			std::cout << "Error when creating mutex: Error code " << GetLastError() << std::endl;
			return -1;
		}
	}

	int clientCount;
	std::cout << "Enter the number of Clients: ";
	std::cin >> clientCount;

	HANDLE* processes = new HANDLE[clientCount];
	for (int i = 0; i < clientCount; i++)
	{
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)clientRequestThread, new Parameters{readersCount,clientCount,fileNameEmployees}, 0, NULL);

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		std::string commandLine = "Client.exe";

		if (!CreateProcess(NULL, (LPSTR)commandLine.c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
		{
			std::cerr << "Failed to create process:Error code " << GetLastError();
			return -1;
		}

		processes[i] = pi.hProcess;
		CloseHandle(pi.hThread);
	}
	
	WaitForMultipleObjects(clientCount, processes, TRUE, INFINITE);
	showBinaryFileInfo(fileNameEmployees);

	for (int i = 0; i < clientCount; i++){
		CloseHandle(processes[i]);
	}

}

void clientRequestThread(void* params){
	Parameters* parameters = (Parameters*)params;

	HANDLE hNamedPipe = CreateNamedPipe(
		"\\\\.\\pipe\\employeesPipe",
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_WAIT,
		parameters->clientCount,
		0,
		0,
		INFINITE,
		(LPSECURITY_ATTRIBUTES)NULL
	);
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Creation of the named pipe failed."
			<< "The last error code: " << GetLastError() << std::endl;
		return;
	}

	if (!ConnectNamedPipe(hNamedPipe,(LPOVERLAPPED)NULL)){
		std::cerr << "The connection failed."
			<< "The last error code: " << GetLastError() << std::endl;
		CloseHandle(hNamedPipe);
		return;
	}

	while (true) {
		Request request;
		if (!ReadFile(hNamedPipe, &request, sizeof(Request), 0, (LPOVERLAPPED)NULL)){
			if (GetLastError() != 109) {
				std::cerr << "Data reading from the named pipe failed."
					<< "The last error code: " << GetLastError() << std::endl;
			}
			CloseHandle(hNamedPipe);
			return;
		}

		std::string semName = "EmployeeFileSemaphoreRecord" + request.employeeID;
		std::string mutName = "EmployeeFileMutexRecord" + request.employeeID;

		HANDLE semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, semName.c_str());
		if (semaphore == NULL) {
			std::cout << "Error when opening semaphore: Error code " << GetLastError();
			CloseHandle(hNamedPipe);
			return;
		}
		HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, false, mutName.c_str());
		if (mutex == NULL) {
			std::cout << "Error when opening semaphore: Error code " << GetLastError();
			CloseHandle(hNamedPipe);
			return;
		}

		if (request.type == READ) {

			WaitForSingleObject(mutex, INFINITE);
			parameters->readersCount[request.employeeID]++;
			if (parameters->readersCount[request.employeeID] == 1) {
				WaitForSingleObject(semaphore,INFINITE);
			}
			ReleaseMutex(mutex);

			Employee Employee = getEmployeeByID(parameters->fileNameEmployees, request.employeeID);
			sendEmployeeInfoByPipe(hNamedPipe, Employee);
			
			waitForRequestToFinish(hNamedPipe);

			WaitForSingleObject(mutex, INFINITE);
			parameters->readersCount[request.employeeID]--;
			if (parameters->readersCount[request.employeeID] == 0) {
				ReleaseSemaphore(semaphore,1,0);
			}
			ReleaseMutex(mutex);


		}
		else {

			WaitForSingleObject(semaphore, INFINITE);

			Employee Employee = getEmployeeByID(parameters->fileNameEmployees, request.employeeID);
			sendEmployeeInfoByPipe(hNamedPipe, Employee);

			std::cout << "Waiting for editted record" << request.employeeID << std::endl;
			Employee = getEmployeeInfoFromPipe(hNamedPipe);
			editEmployeeRecord(parameters->fileNameEmployees, Employee);

			waitForRequestToFinish(hNamedPipe);

			ReleaseSemaphore(semaphore, 1, 0);
		}
	}

}

void waitForRequestToFinish(HANDLE hNamedPipe) {
	int code = 1;
	if (!ReadFile(hNamedPipe, (char*)&code, sizeof(int), 0, (LPOVERLAPPED)NULL)) {
		std::cerr << "Data writing to the named pipe failed." << std::endl
			<< "The last error code: " << GetLastError() << std::endl;
		CloseHandle(hNamedPipe);
		system("pause");
		return;
	}
}

void launchCreator(const std::string fileNameEmployees, const int recordsNumber)
{

	std::string Args = "Creator.exe " + fileNameEmployees + " " + std::to_string(recordsNumber);
	launchProcess(Args);
}
void launchProcess(const std::string Args)
{
	LPSTR comArgs = const_cast<char*>(Args.c_str());

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	CreateProcess(nullptr, comArgs, nullptr, nullptr, FALSE,
		CREATE_NEW_CONSOLE, nullptr, nullptr, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}
void showBinaryFileInfo(const std::string fileNameEmployees)
{
	std::ifstream fileEmployees(fileNameEmployees, std::ios::binary);

	if (!fileEmployees) {
		std::cout << "Error occured while opening the file " << fileNameEmployees << std::endl;
		return;
	}

	std::cout << "**********************************************" << std::endl;

	Employee Employee;
	while (fileEmployees.read((char*)&Employee, sizeof(Employee)))
	{
		std::cout << std::left << std::setw(17) << Employee.num << std::setw(15) << Employee.name << std::setw(7) <<
			Employee.hours << std::setw(6) << std::endl;
	}

	std::cout << "**********************************************" << std::endl;

	fileEmployees.close();
}

