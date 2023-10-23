#include <windows.h>
#include <iostream>
#include <algorithm>
#include <vector>

struct Task {
    int taskType; // Номер задачи (1 - сумма, 2 - максимум, 3 - минимум)
    std::vector<int> data; // Данные для выполнения задачи
};

std::vector<int> sharedData(1000000); // Общий ресурс (список чисел от 1 до 1000000)
HANDLE hMutex; // Мьютекс для синхронизации доступа к общим данным

DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    Task* task = static_cast<Task*>(lpParam);

    WaitForSingleObject(hMutex, INFINITE); // Захватываем мьютекс для синхронизации доступа

    if (task->taskType == 1) {
        // Задача для вычисления суммы
        long long sum = 0;
        for (int num : task->data) {
            sum += num;
        }
        std::cout << "Sum: " << sum << std::endl;
    } else if (task->taskType == 2) {
        // Задача для нахождения максимума
        int max = *std::max_element(task->data.begin(), task->data.end());
        std::cout << "Max: " << max << std::endl;
    } else if (task->taskType == 3) {
        // Задача для изменения первого числа на случайное
        if (!task->data.empty()) {
            srand(time(NULL));
            task->data[0] = rand() % 1000; // Заменяем первое число на случайное от 0 до 999
            std::cout << "First number changed to: " << task->data[0] << std::endl;

            std::cout << "First ten numbers: ";
            for (int i = 0; i < 10 && i < task->data.size(); ++i) {
                std::cout << task->data[i] << " ";
            }
            std::cout << std::endl;
        }
    }

    ReleaseMutex(hMutex); // Освобождаем мьютекс после завершения задачи

    return 0;
}

int main() {
    // Инициализация общих данных
    for (int i = 0; i < 1000000; ++i) {
        sharedData[i] = i + 1;
    }

    // Создаем мьютекс
    hMutex = CreateMutex(NULL, FALSE, NULL);

    // Создаем задачи для суммы, минимума и максимума
    Task tasks[3];
    for (int i = 0; i < 3; ++i) {
        tasks[i].taskType = i + 1;
        tasks[i].data = sharedData;
    }

    // Запускаем потоки для выполнения задач
    HANDLE hThreads[3];
    for (int i = 0; i < 3; ++i) {
        hThreads[i] = CreateThread(NULL, 0, ThreadFunction, &tasks[i], 0, NULL);
    }

    // Дожидаемся завершения потоков
    WaitForMultipleObjects(3, hThreads, TRUE, INFINITE);

    // Закрываем дескрипторы потоков
    for (int i = 0; i < 3; ++i) {
        CloseHandle(hThreads[i]);
    }

    // Закрываем дескриптор мьютекса
    CloseHandle(hMutex);

    return 0;
}
