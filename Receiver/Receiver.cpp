#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>

void readMessage(const std::string messageFileName, HANDLE& mutex, HANDLE& semaphore);

int main() {

    std::string messageFileName;
	int recordsNumber;
    std::cout << "Enter binary file name: ";
    std::cin >> messageFileName;
    std::cout << "Enter the number of records in binary file: ";
    std::cin >> recordsNumber;

    std::ofstream messageFile(messageFileName, std::ios::binary);
	messageFile.close();

	HANDLE semaphore = CreateSemaphore(NULL, recordsNumber, recordsNumber, "messageFileSemaphore");
	if (semaphore == NULL)
	{
        std::cout << "error when creating semaphore: " << GetLastError() << std::endl;
		return -1;
	}
    HANDLE mutex = CreateMutex(NULL, FALSE, "messageFileMutex");
    if (mutex == NULL)
    {
        std::cout << "error when creating mutex: " << GetLastError() << std::endl;
        return -1;
    }

    int sendersAmount;
    std::cout << "Enter the amount of Senders: ";
    std::cin >> sendersAmount;

    HANDLE* senders = new HANDLE[sendersAmount];
    for (int i = 0; i < sendersAmount; i++)
    {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        std::string executable = "Sender.exe " + messageFileName;
        if (!CreateProcess(NULL, (LPSTR)executable.c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
        {
            std::cerr << "Failed to create process " << GetLastError();
            break;
        }
        senders[i] = pi.hProcess;
        CloseHandle(pi.hThread);
    }

    int choice;
    do {
        std::cout << "You have the following options:\n"
        << "1. Read message\n"
        << "2. Stop working\n"
        << "Enter your choice: ";
        std::cin >> choice;
        switch (choice) {
        case 1:
            readMessage(messageFileName,mutex,semaphore);
            break;
        case 2:
            std::cout << "Stopping work." << std::endl;
            break;
        default:
            std::cout << "Invalid choice. Please try again.n";
            break;
        }
    } while (choice != 2);

    for (int i = 0; i < sendersAmount; i++) {
        CloseHandle(senders[i]);
    }
    delete[] senders;
    CloseHandle(semaphore);
    CloseHandle(mutex);
}

void readMessage(const std::string messageFileName, HANDLE& mutex, HANDLE& semaphore) {

    while (ReleaseSemaphore(semaphore, 1, NULL) == FALSE);
    WaitForSingleObject(mutex, INFINITE);

    std::ifstream messageFile(messageFileName, std::ios::binary);
    if (!messageFile) {
        std::cout << "Error occured while opening the file " << messageFileName << " : Error code: " << GetLastError() << std::endl;
        return;
    }
    std::string message;
    std::getline(messageFile,message);

    std::cout << "****************************" << std::endl;
    std::cout << "The message is: " << message << std::endl;
    std::cout << "****************************" << std::endl;

    std::ofstream tmpFile("temp.bin", std::ios::binary);
    while (getline(messageFile,message)) {
        tmpFile << message << "\n";
    }

    messageFile.close();
    tmpFile.close();

    remove(messageFileName.c_str());
    rename("temp.bin", messageFileName.c_str());

    ReleaseMutex(mutex);

}