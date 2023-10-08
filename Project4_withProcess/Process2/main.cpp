#include <iostream>
#include <vector>
#include <windows.h>

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

    int max = numbers[0];
    for (int num : numbers) {
        if (num > max) {
            max = num;
        }
    }

    HANDLE hSemaphore = OpenSemaphore(SYNCHRONIZE, FALSE, "MySemaphore"); // Открываем семафор
    WaitForSingleObject(hSemaphore, INFINITE); // Ожидаем разрешения семафора

    std::cout << "Max: " << max << std::endl;

    ReleaseSemaphore(hSemaphore, 1, NULL); // Уменьшаем счетчик семафора

    // Измеряем окончание выполнения участка кода
    QueryPerformanceCounter(&endTime);

    // Вычисляем время выполнения в миллисекундах
    double executionTime = static_cast<double>(endTime.QuadPart - startTime.QuadPart) / static_cast<double>(frequency.QuadPart) * 1000.0;
    std::cout << "Execution time: " << executionTime << " ms" << std::endl;

    return 0;
}
