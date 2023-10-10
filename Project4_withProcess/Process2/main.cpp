#include <iostream>
#include <vector>
#include <windows.h>

DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    // Код для выполнения внутри потока
    std::vector<int>* numbers = static_cast<std::vector<int>*>(lpParam);

    int max = (*numbers)[0];
    for (int num : *numbers) {
        if (num > max) {
            max = num;
        }
    }

    HANDLE hSemaphore = OpenSemaphore(SYNCHRONIZE, FALSE, "MySemaphore"); // Открываем семафор
    WaitForSingleObject(hSemaphore, INFINITE); // Ожидаем разрешения семафора

    std::cout << "Max: " << max << std::endl;

    ReleaseSemaphore(hSemaphore, 1, NULL); // Уменьшаем счетчик семафора

    return 0;
}

int main() {
    // Измеряем начало выполнения участка кода
    LARGE_INTEGER startTime, endTime, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&startTime);

    const size_t numNumbers = 1000000;
    std::vector<int> numbers(numNumbers);

    for (size_t i = 0; i < numNumbers; ++i) {
        numbers[i] = i + 1;
    }

    // Создаем поток
    HANDLE hThread = CreateThread(NULL, 0, ThreadFunction, &numbers, 0, NULL);

    // Дожидаемся завершения потока
    WaitForSingleObject(hThread, INFINITE);

    // Закрываем дескриптор потока
    CloseHandle(hThread);

    // Измеряем окончание выполнения участка кода
    QueryPerformanceCounter(&endTime);

    // Вычисляем время выполнения в миллисекундах
    double executionTime = static_cast<double>(endTime.QuadPart - startTime.QuadPart) / static_cast<double>(frequency.QuadPart) * 1000.0;
    std::cout << "Execution time: " << executionTime << " ms" << std::endl;

    return 0;
}
