#include <Windows.h>
#include <iostream>
#include <string>

bool RegisterProgram(const std::string& programName, const std::string& installerPath) {
    HKEY hUninstallKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0, KEY_WRITE, &hUninstallKey) == ERROR_SUCCESS) {
        HKEY hAppKey;
        if (RegCreateKey(hUninstallKey, programName.c_str(), &hAppKey) == ERROR_SUCCESS) {
            RegSetValueEx(hAppKey, "DisplayName", 0, REG_SZ, (const BYTE*)programName.c_str(), programName.size());

            // Set UninstallString to an empty string
            std::string emptyString = "";
            RegSetValueEx(hAppKey, "UninstallString", 0, REG_SZ, (const BYTE*)emptyString.c_str(), emptyString.size());

            RegCloseKey(hAppKey);
            RegCloseKey(hUninstallKey);

            // Now, run the program installer
            SHELLEXECUTEINFO ShExecInfo = {0};
            ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
            ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
            ShExecInfo.lpFile = installerPath.c_str();
            ShExecInfo.nShow = SW_SHOWNORMAL;

            if (ShellExecuteEx(&ShExecInfo) == TRUE) {
                WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
                CloseHandle(ShExecInfo.hProcess);
                return true;
            }
        }
    }
    return false;
}

std::string FindUninstallPath(const std::string& programName) {
    HKEY hUninstallKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0, KEY_READ, &hUninstallKey) == ERROR_SUCCESS) {
        std::string uninstallPath;
        DWORD uninstallPathSize = 1024;
        if (RegGetValue(hUninstallKey, programName.c_str(), "UninstallString", RRF_RT_REG_SZ, NULL, &uninstallPath[0], &uninstallPathSize) == ERROR_SUCCESS) {
            RegCloseKey(hUninstallKey);
            return uninstallPath;
        }
        RegCloseKey(hUninstallKey);
    }
    return "";
}

bool UninstallProgram(const std::string& programName) {
    std::string uninstallPath;

    std::cout << "Enter the uninstall path (e.g., C:\\Path\\To\\Uninstaller.exe): ";
    std::getline(std::cin, uninstallPath);

    if (!uninstallPath.empty()) {
        // Run the uninstaller
        SHELLEXECUTEINFO ShExecInfo = {0};
        ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
        ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        ShExecInfo.lpFile = uninstallPath.c_str();
        ShExecInfo.nShow = SW_SHOWNORMAL;

        if (ShellExecuteEx(&ShExecInfo) == TRUE) {
            WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
            CloseHandle(ShExecInfo.hProcess);
        } else {
            std::cout << "Failed to run the uninstaller." << std::endl;
            return false;
        }

        // Ask the user if they want to delete the program from the registry
        std::cout << "Do you want to delete the program from the registry (yes/no)? ";
        std::string answer;
        std::cin >> answer;

        if (answer == "yes" || answer == "Yes" || answer == "YES") {
            // Update UninstallString in the registry
            HKEY hUninstallKey;
            if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0, KEY_WRITE, &hUninstallKey) == ERROR_SUCCESS) {
                if (RegDeleteKey(hUninstallKey, programName.c_str()) == ERROR_SUCCESS) {
                    RegCloseKey(hUninstallKey);
                    std::cout << "Program uninstalled and removed from the registry." << std::endl;
                    return true;
                }
                RegCloseKey(hUninstallKey);
            }
        } else {
            std::cout << "Program uninstalled but not removed from the registry." << std::endl;
        }
    }
    return false;
}

void ListInstalledPrograms() {
    HKEY hUninstallKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0, KEY_READ, &hUninstallKey) == ERROR_SUCCESS) {
        DWORD subkeyIndex = 0;
        char subkeyName[255];

        while (RegEnumKey(hUninstallKey, subkeyIndex, subkeyName, sizeof(subkeyName)) == ERROR_SUCCESS) {
            HKEY hAppKey;
            if (RegOpenKeyEx(hUninstallKey, subkeyName, 0, KEY_READ, &hAppKey) == ERROR_SUCCESS) {
                DWORD valueSize = 1024;
                char value[1024];
                if (RegQueryValueEx(hAppKey, "DisplayName", NULL, NULL, (LPBYTE)value, &valueSize) == ERROR_SUCCESS) {
                    std::cout << "Installed Program: " << value << std::endl;
                    std::cout << "Registry Key Path: SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" << subkeyName << std::endl;
                }
                RegCloseKey(hAppKey);
            }
            subkeyIndex++;
        }
        RegCloseKey(hUninstallKey);
    }
}

int main() {
    while (true) {
        int choice;
        std::string programName;
        std::string installerPath;

        std::cout << "1. List installed programs\n";
        std::cout << "2. Register a new program\n";
        std::cout << "3. Unregister a program\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (choice == 1) {
            ListInstalledPrograms();
        } else if (choice == 2) {
            std::cout << "Enter the program name: ";
            std::cin.ignore();
            std::getline(std::cin, programName);

            std::cout << "Enter the installer path (e.g., C:\\Path\\To\\Installer.exe): ";
            std::getline(std::cin, installerPath);

            if (RegisterProgram(programName, installerPath)) {
                std::cout << "Program registered successfully!" << std::endl;
            } else {
                std::cout << "Failed to register the program." << std::endl;
            }
        } else if (choice == 3) {
            std::cout << "Enter the program name to uninstall: ";
            std::cin.ignore();
            std::getline(std::cin, programName);

            if (UninstallProgram(programName)) {
                std::cout << "Program uninstalled successfully!" << std::endl;
            } else {
                std::cout << "Failed to uninstall the program." << std::endl;
            }
        } else if (choice == 4) {
            break;
        }
    }

    return 0;
}