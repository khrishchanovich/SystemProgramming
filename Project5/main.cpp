#include <Windows.h>
#include <iostream>
#include <string>

// Регистрируем новую программу в реестре с установкой пути к инсталятору
bool SetProgramRegistry(const std::string& programName, const std::string& installerPath) {
    HKEY hParentKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\MyProject", 0, KEY_WRITE, &hParentKey) == ERROR_SUCCESS) {
        HKEY hAppKey;
        if (RegCreateKey(hParentKey, programName.c_str(), &hAppKey) == ERROR_SUCCESS) {
            RegSetValueEx(hAppKey, "DisplayName", 0, REG_SZ, (const BYTE*)programName.c_str(), programName.size());

            RegSetValueEx(hAppKey, "InstallString", 1, REG_SZ, (const BYTE*)installerPath.c_str(), installerPath.size());

            RegCloseKey(hAppKey);
            RegCloseKey(hParentKey);
            return true;
        }
    }
    return false;
}

// Запускаем инсталятор
bool RunInstaller(const std::string& installerPath) {
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
    return false;
}

// Устаналиваем путь к деинсталятору
bool SetUninstallRegistry(const std::string& programName, const std::string& uninstallerPath) {
    HKEY hParentKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\MyProject", 0, KEY_WRITE, &hParentKey) == ERROR_SUCCESS) {
        HKEY hAppKey;
        if (RegCreateKey(hParentKey, programName.c_str(), &hAppKey) == ERROR_SUCCESS) {

            RegSetValueEx(hAppKey, "UninstallString", 2, REG_SZ, (const BYTE*)uninstallerPath.c_str(), uninstallerPath.size());

            RegCloseKey(hAppKey);
            RegCloseKey(hParentKey);

            return true;
        }
    }
    return false;
}

// Запускаем деинсталятор
bool RunUninstaller(const std::string& programName) {
    HKEY hParentKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\MyProject", 0, KEY_READ, &hParentKey) == ERROR_SUCCESS) {
        HKEY hAppKey;
        if (RegOpenKeyEx(hParentKey, programName.c_str(), 0, KEY_READ, &hAppKey) == ERROR_SUCCESS) {
            DWORD dataValueSize = 1024;
            char uninstallerPath[1024];
            if (RegQueryValueEx(hAppKey, "UninstallString", 0, NULL, (LPBYTE)uninstallerPath, &dataValueSize) == ERROR_SUCCESS) {
                SHELLEXECUTEINFO ShExecInfo = {0};
                ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
                ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
                ShExecInfo.lpFile = uninstallerPath;
                ShExecInfo.nShow = SW_SHOWNORMAL;

                if (ShellExecuteEx(&ShExecInfo) == TRUE) {
                    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
                    CloseHandle(ShExecInfo.hProcess);
                    return true;
                } else {
                    std::cout << "Failed to run the uninstaller.\n***\n" << std::endl;
                    return false;
                }
            }
            RegCloseKey(hAppKey);
        }
        RegCloseKey(hParentKey);
    }
    return false;
}

// Список подключей в ключе реестра
void ListInstalledPrograms() {
    HKEY hParentKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\MyProject", 0, KEY_READ, &hParentKey) == ERROR_SUCCESS) {
        DWORD subkeyIndex = 0;
        char subkeyName[255];

        while (RegEnumKey(hParentKey, subkeyIndex, subkeyName, sizeof(subkeyName)) == ERROR_SUCCESS) {
            HKEY hAppKey;
            if (RegOpenKeyEx(hParentKey, subkeyName, 0, KEY_READ, &hAppKey) == ERROR_SUCCESS) {
                std::cout << "Program: " << subkeyName << std::endl;
                RegCloseKey(hAppKey);
            }
            subkeyIndex++;
        }
        RegCloseKey(hParentKey);
    }
}

int main() {
    while (true) {
        int choice;
        std::string programName;
        std::string installerPath;
        std::string uninstallerPath;

        std::cout << "1. List installed programs\n";
        std::cout << "2. Register new program and set InstallerPath\n";
        std::cout << "3. Set UninstallerPath\n";
        std::cout << "4. Install program\n";
        std::cout << "5. Uninstall program\n";
        std::cout << "6. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (choice == 1) {
            ListInstalledPrograms();
        } else if (choice == 2) {
            std::cout << "Enter the program name: ";
            std::getline(std::cin, programName);

            std::cout << "Enter the installer path (e.g., C:\\Path\\To\\Installer.exe): ";
            std::getline(std::cin, installerPath);

            if (SetProgramRegistry(programName, installerPath)) {
                std::cout << "Program information added to the registry." << std::endl;
            } else {
                std::cout << "Failed to add program information to the registry." << std::endl;
            }
        } else if (choice == 3) {
            std::cout << "Enter the program name to set UninstallString ";
            std::getline(std::cin, programName);

            std::cout << "Enter the uninstaller path (e.g., C:\\Path\\To\\Uninstaller.exe): ";
            std::getline(std::cin, uninstallerPath);

            if (SetUninstallRegistry(programName, uninstallerPath)) {
                std::cout << "Uninstall information added to the registry." << std::endl;
            } else {
                std::cout << "Failed to add uninstall information to the registry." << std::endl;
            }
        } else if (choice == 4) {
            std::cout << "Enter the program name to install: ";
            std::getline(std::cin, programName);

            HKEY hInstallKey;
            if (RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\MyProject", 0, KEY_READ, &hInstallKey) == ERROR_SUCCESS) {
                HKEY hAppKey;
                if (RegOpenKeyEx(hInstallKey, programName.c_str(), 0, KEY_READ, &hAppKey) == ERROR_SUCCESS) {
                    DWORD valueSize = 1024;
                    char installPath[1024];
                    if (RegQueryValueEx(hAppKey, "InstallString", 0, NULL, (LPBYTE)installPath, &valueSize) == ERROR_SUCCESS) {
                        if (RunInstaller(installPath)) {
                            std::cout << "Program installed successfully!" << std::endl;
                        } else {
                            std::cout << "Failed to run the installer." << std::endl;
                        }
                    }
                    RegCloseKey(hAppKey);
                }
                RegCloseKey(hInstallKey);
            }
        } else if (choice == 5) {
            std::cout << "Enter the program name to uninstall: ";
            std::getline(std::cin, programName);

            HKEY hUninstallKey;
            if (RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\MyProject", 0, KEY_READ, &hUninstallKey) == ERROR_SUCCESS) {
                HKEY hAppKey;
                if (RegOpenKeyEx(hUninstallKey, programName.c_str(), 0, KEY_READ, &hAppKey) == ERROR_SUCCESS) {
                    DWORD valueSize = 1024;
                    char uninstallPath[1024];
                    if (RegQueryValueEx(hAppKey, "UninstallString", 0, NULL, (LPBYTE)uninstallPath, &valueSize) == ERROR_SUCCESS) {
                        if (RunUninstaller(programName)) {
                            std::cout << "Program uninstalled successfully!" << std::endl;
                        } else {
                            std::cout << "Failed to run the uninstaller." << std::endl;
                        }
                    }
                    RegCloseKey(hAppKey);
                }
                RegCloseKey(hUninstallKey);
            }
        } else if (choice == 6) {
            break;
        }
    }

    return 0;
}
