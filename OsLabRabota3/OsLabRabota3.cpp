#include <Windows.h>
#include <iostream>
#include <vector>

void marker(void* param);
void printArray(int* arr,int arraySize);


struct Parameters
{
    int* arr;
    int arrSize;
    int ID;
    CRITICAL_SECTION* cs;
    HANDLE& waitEvent;
    HANDLE& TerminateEvent;
    HANDLE& hContinue;
};

int main()
{
    int arraySize;
    std::cout << "Enter the size of array" << std::endl;
    std::cin >> arraySize;

    int* arr = new int[arraySize]{0};

    CRITICAL_SECTION cs;
    InitializeCriticalSection(&cs);

    int markersNum;
    std::cout << "Enter the number of markers" << std::endl;
    std::cin >> markersNum;


    HANDLE hContinue= CreateEvent(NULL, TRUE, FALSE, NULL);
    HANDLE* markersH = new HANDLE[markersNum];
    HANDLE* waitEvents = new HANDLE[markersNum];
    HANDLE* TerminateEvents = new HANDLE[markersNum];
    for (int i = 0; i < markersNum; i++) 
    {
        TerminateEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        waitEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        markersH[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)marker, new Parameters{arr,arraySize,i+1,&cs,waitEvents[i],TerminateEvents[i],hContinue}, 0, NULL);
    }

    WaitForMultipleObjects(markersNum, waitEvents, true, INFINITE);

    std::vector<bool> isTerminated(markersNum,false);
    for(int i = 0 ; i < markersNum;i++)
    {

        std::cout << "**********************************" << std::endl;

        for (int j = 0; j < markersNum; j++){
            if(!isTerminated[j])
                ResetEvent(waitEvents[j]);
        }
        
        PulseEvent(hContinue);

        WaitForMultipleObjects(markersNum, waitEvents, true, INFINITE);


        printArray(arr, arraySize);

        int markerTermID;
        do 
        {
            std::cout << "Enter the ID of the thread to be terminated: " << std::endl;
            std::cin >> markerTermID;
        } while ((markerTermID <= 0 || markerTermID > markersNum) || isTerminated[markerTermID-1]);

        markerTermID--;

        SetEvent(TerminateEvents[markerTermID]);
        WaitForSingleObject(markersH[markerTermID], INFINITE);
        CloseHandle(markersH[markerTermID]);
        isTerminated[markerTermID] = true;

        printArray(arr, arraySize);

        system("pause");

    }

    for (int i = 0; i < markersNum; i++) 
    {
        CloseHandle(waitEvents[i]);
        CloseHandle(TerminateEvents[i]);
    }
    CloseHandle(hContinue);
}

void marker(void* param) {
    Parameters* parameters = (Parameters*)param;
    srand(parameters->ID);
    std::vector<int> indexes;

    HANDLE events[2] = { parameters->hContinue,parameters->TerminateEvent};

    while(true){
        SetEvent(parameters->waitEvent);

        WaitForMultipleObjects(2,events,FALSE,INFINITE);
        if (WaitForSingleObject(parameters->TerminateEvent, 0) == WAIT_OBJECT_0)
            break;


        int randNum;
        while (true)
        {
            randNum = rand() % parameters->arrSize;
            EnterCriticalSection(parameters->cs);
            if (parameters->arr[randNum] != 0) 
            {
                break;
            }
            Sleep(5);
            parameters->arr[randNum] = parameters->ID;
            LeaveCriticalSection(parameters->cs);
            indexes.push_back(randNum);
            Sleep(5);
        }
        std::cout << "marker ID: " << parameters->ID << " marked count: " << indexes.size() << " last index: " << randNum << std::endl;
        LeaveCriticalSection(parameters->cs);

    }

    EnterCriticalSection(parameters->cs);

    for (int index : indexes) {
        parameters->arr[index] = 0;
    }

    LeaveCriticalSection(parameters->cs);

    ExitThread(1);

}

void printArray(int* arr, int arraySize) {
    std::cout << "The array: " << std::endl;
    for (int j = 0; j < arraySize; j++)
    {
        std::cout << arr[j] << " ";
    }
    std::cout << "\n";
}