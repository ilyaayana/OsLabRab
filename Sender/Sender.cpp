#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>

void getAndSendMessage(const std::string fileName,HANDLE& mutex, HANDLE& semaphore);

int main(int argc, char* argv[]){
    HANDLE hMutex = OpenMutex(MUTANT_ALL_ACCESS, FALSE, "messageFileMutex");
    HANDLE hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "messageFileSemaphore");

    int choice;
    do {
        std::cout << "You have the following options:" << std::endl;
        std::cout << "1. Send message" << std::endl;
        std::cout << "2. Stop working" << std::endl;
        std::cout << "Enter your choice: " << std::endl;
        std::cin >> choice;
        switch (choice) {
        case 1:
            std::cin.ignore(32755, '\n');
            std::cin.clear();
            getAndSendMessage(argv[1],hMutex,hSemaphore);
            break;
        case 2:
            std::cout << "Stopping work...";
            break;
        default:
            std::cout << "Invalid choice. Please try again" << std::endl;
            break;
        }
    } while (choice != 2);

    CloseHandle(hMutex);
    CloseHandle(hSemaphore);
}

void getAndSendMessage(const std::string messageFileName, HANDLE& mutex, HANDLE& semaphore) {

    std::string message;
    std::cout << "Please enter a message with less than 20 characters: ";
    std::getline(std::cin, message);

    while (message.length() >= 20) {
        std::cout << "Your message is too long. Please enter a message with less than 20 characters: ";
        std::getline(std::cin, message);
    }
    message += "\n";

    WaitForSingleObject(semaphore, INFINITE);
    WaitForSingleObject(mutex,INFINITE);

    std::ofstream messageFile(messageFileName, std::ios::binary | std::ios_base::app);
    if (!messageFile) {
        std::cout << "Error occured while opening the file " << messageFileName << std::endl;
        return;
    }

    messageFile << message;
    messageFile.close();

    ReleaseMutex(mutex);

}