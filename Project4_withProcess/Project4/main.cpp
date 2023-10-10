#include <vector>
#include <windows.h>
#include <iostream>

int main() {
    // ������� ������ �������� ��� ������ �����
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

    CreateProcess(NULL,   // ��� ������������ �����
                  R"(D:\5 semester\C++\Process1\cmake-build-debug\Process1.exe)", // ��������� ������
                  NULL,              // ���������� �������� �� �����������
                  NULL,              // ���������� ������ �� �����������
                  FALSE,             // ���������� �������� �� �����������
                  0,                 // ����� ��������
                  NULL,              // ���� ���������� ���������
                  NULL,              // ������� �������
                  &si1,              // ���������� � �������
                  &pi1);             // ���������� � ��������

    CreateProcess(NULL,   // ��� ������������ �����
                  R"(D:\5 semester\C++\Process2\cmake-build-debug\Process2.exe)",     // ��������� ������
                  NULL,              // ���������� �������� �� �����������
                  NULL,              // ���������� ������ �� �����������
                  FALSE,             // ���������� �������� �� �����������
                  0,                 // ����� ��������
                  NULL,              // ���� ���������� ���������
                  NULL,              // ������� �������
                  &si2,              // ���������� � �������
                  &pi2);             // ���������� � ��������

    CreateProcess(NULL,   // ��� ������������ �����
                  R"(D:\5 semester\C++\Process3\cmake-build-debug\Process3.exe)",     // ��������� ������
                  NULL,              // ���������� �������� �� �����������
                  NULL,              // ���������� ������ �� �����������
                  FALSE,             // ���������� �������� �� �����������
                  0,                 // ����� ��������
                  NULL,              // ���� ���������� ���������
                  NULL,              // ������� �������
                  &si3,              // ���������� � �������
                  &pi3);             // ���������� � ��������

    // ���������� ��������� �� 1 �� 3
    if (!SetPriorityClass(pi2.hProcess, ABOVE_NORMAL_PRIORITY_CLASS)) {
        std::cerr << "Failed to set priority." << std::endl;
    }

    // ���� ���������� ���������
    WaitForSingleObject(pi1.hProcess, INFINITE);
    WaitForSingleObject(pi2.hProcess, INFINITE);
    WaitForSingleObject(pi3.hProcess, INFINITE);

    // ��������� ����������� ���������
    CloseHandle(pi1.hProcess);
    CloseHandle(pi2.hProcess);
    CloseHandle(pi3.hProcess);

    return 0;
}
