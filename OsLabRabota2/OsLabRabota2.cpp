#include <Windows.h>
#include <iostream>
#include <cmath>

void min_max(void* param);
void average(void* param);

int getValidInteger();
int getArraySize();

struct Parameters
{
    int* arr;
    int arrSize;
    int min, max, arr_average;
};

int main()
{
    Parameters parameters;

    parameters.arrSize = getArraySize();

    parameters.arr = new int[parameters.arrSize];
    for (int i = 0; i < parameters.arrSize; i++)
    {
        std::cout << "Element " << i + 1 << ": ";
        parameters.arr[i] = getValidInteger();
    }

    std::cout << "***********************************" << std::endl;

    HANDLE hThreadMin_max, hThreadAverage;
    hThreadMin_max = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)min_max, &parameters, 0, NULL);
    hThreadAverage = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)average, &parameters, 0, NULL);

    WaitForSingleObject(hThreadMin_max, INFINITE);
    CloseHandle(hThreadMin_max);
    WaitForSingleObject(hThreadAverage, INFINITE);
    CloseHandle(hThreadAverage);


    for (int i = 0; i < parameters.arrSize; i++){
        if (parameters.arr[i] == parameters.min || parameters.arr[i] == parameters.max){
            parameters.arr[i] = parameters.arr_average;
        }
    }

    std::cout << "The resulting array: ";
    for (int i = 0; i < parameters.arrSize; i++)
    {
        std::cout << parameters.arr[i] << " ";
    }
    std::cout << "\n";

    delete[] parameters.arr;

    system("pause");

}

void min_max(void* param)
{
    Parameters* parameters = (Parameters*)param;

    parameters->min = parameters->arr[0];
    parameters->max = parameters->arr[0];

    for (int i = 1; i < parameters->arrSize; i++)
    {
        if (parameters->arr[i] < parameters->min) {
            parameters->min = parameters->arr[i];
        }

        Sleep(7);

        if (parameters->arr[i] > parameters->max) {
            parameters->max = parameters->arr[i];
        }

        Sleep(7);
    }

    std::cout << "minimal number: " << parameters->min << "\nmaximal number: " << parameters->max << std::endl;

    ExitThread(1);

}
void average(void* param)
{
    Parameters* parameters = (Parameters*)param;
    double arrSum = 0;

    for (int i = 0; i < parameters->arrSize; i++)
    {
        arrSum += parameters->arr[i];
        Sleep(12);
    }

    parameters->arr_average = round(arrSum / parameters->arrSize);
    std::cout << "array's average: " << parameters->arr_average << std::endl;

    ExitThread(1);
}

int getArraySize(){
    int arrSize;

    while (true){
        std::cout << "Enter array size: ";
        arrSize = getValidInteger();
        if (arrSize <= 0){
            std::cout << "Please enter the integer that is bigger than 0" << std::endl;
        }
        else{
            break;
        }
    }

    return arrSize;
}
int getValidInteger(){
    double num;
    while (true){
        if (std::cin >> num && num == (int)num){
            return num;
        }
        else{
            std::cout << "Invalid Input! Please input an integer value." << std::endl;
            std::cin.clear();
            while (std::cin.get() != '\n');
        }
    }

}
