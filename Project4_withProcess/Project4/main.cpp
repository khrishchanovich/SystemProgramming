#include <vector>
#include <windows.h>
#include <iostream>

int main() {
    // Создаем разные процессы для разных задач
    STARTUPINFO si1, si2, si3;
    PROCESS_INFORMATION pi1, pi2, pi3;

    ZeroMemory(&si1, sizeof(si1));
    si1.cb = sizeof(si1);
    ZeroMemory(&pi1, sizeof(pi1));

    ZeroMemory(&si2, sizeof(si2));
    si2.cb = sizeof(si2);
    ZeroMemory(&pi2, sizeof(pi2));

    ZeroMemory(&si3, sizeof(si3));
    si3.cb = sizeof(si3);
    ZeroMemory(&pi3, sizeof(pi3));

    CreateProcess(NULL,   // имя исполняемого файла
                  R"(D:\5 semester\C++\Process1\cmake-build-debug\Process1.exe)", // командная строка
                  NULL,              // дескриптор процесса не наследуется
                  NULL,              // дескриптор потока не наследуется
                  FALSE,             // дескриптор процесса не наследуется
                  0,                 // флаги создания
                  NULL,              // блок переменных окружения
                  NULL,              // текущий каталог
                  &si1,              // информация о запуске
                  &pi1);             // информация о процессе

    CreateProcess(NULL,   // имя исполняемого файла
                  R"(D:\5 semester\C++\Process2\cmake-build-debug\Process2.exe)",     // командная строка
                  NULL,              // дескриптор процесса не наследуется
                  NULL,              // дескриптор потока не наследуется
                  FALSE,             // дескриптор процесса не наследуется
                  0,                 // флаги создания
                  NULL,              // блок переменных окружения
                  NULL,              // текущий каталог
                  &si2,              // информация о запуске
                  &pi2);             // информация о процессе

    CreateProcess(NULL,   // имя исполняемого файла
                  R"(D:\5 semester\C++\Process3\cmake-build-debug\Process3.exe)",     // командная строка
                  NULL,              // дескриптор процесса не наследуется
                  NULL,              // дескриптор потока не наследуется
                  FALSE,             // дескриптор процесса не наследуется
                  0,                 // флаги создания
                  NULL,              // блок переменных окружения
                  NULL,              // текущий каталог
                  &si3,              // информация о запуске
                  &pi3);             // информация о процессе

    // УСТАНОВИТЬ ПРИОРИТЕТ ОТ 1 ДО 3
    if (!SetPriorityClass(pi2.hProcess, ABOVE_NORMAL_PRIORITY_CLASS)) {
        std::cerr << "Failed to set priority." << std::endl;
    }

    // Ждем завершения процессов
    WaitForSingleObject(pi1.hProcess, INFINITE);
    WaitForSingleObject(pi2.hProcess, INFINITE);
    WaitForSingleObject(pi3.hProcess, INFINITE);

    // Закрываем дескрипторы процессов
    CloseHandle(pi1.hProcess);
    CloseHandle(pi2.hProcess);
    CloseHandle(pi3.hProcess);

    return 0;
}
