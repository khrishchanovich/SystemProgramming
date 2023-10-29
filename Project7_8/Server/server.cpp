#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error initializing Winsock." << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error creating a socket." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345); // Замените на желаемый порт

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error binding the socket to a port." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Error listening to the socket." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "The server is running and listening on port 12345." << std::endl;

    while (true) {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);

        if (clientSocket != INVALID_SOCKET) {
            std::cout << "A client with IP address has connected: " << inet_ntoa(clientAddr.sin_addr) << std::endl;

            char buffer[1024];
            int bytesRead;

            // Получаем команду от клиента (имя файла или действие)
            bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';
                std::string command = buffer;

                if (command == "UPLOAD") {
                    // Клиент хочет отправить файл на сервер
                    // Получаем имя файла
                    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                    if (bytesRead > 0) {
                        buffer[bytesRead] = '\0';
                        std::string fileName = buffer;

                        // Полный путь к папке проекта серверной части, куда будут сохраняться файлы
                        std::string serverPath = "C:\\Users\\Asus\\CLionProjects\\ServerPart\\files\\"; // Замените на желаемый путь
                        serverPath += fileName;

                        std::ofstream fileStream(serverPath, std::ios::out | std::ios::binary);

                        if (!fileStream.is_open()) {
                            std::cerr << "Error opening file for writing: " << fileName << std::endl;
                            closesocket(clientSocket);
                        } else {
                            while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
                                fileStream.write(buffer, bytesRead);
                            }
                            fileStream.close();
                            std::cout << "Received and saved file: " << fileName << std::endl;
                            closesocket(clientSocket);
                        }
                    }
                } else if (command == "DOWNLOAD") {
                    // Клиент хочет скачать файл с сервера
                    // Получаем имя файла для скачивания
                    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                    if (bytesRead > 0) {
                        buffer[bytesRead] = '\0';
                        std::string fileName = buffer;

                        // Полный путь к файлу, который клиент хочет скачать
                        std::string filePathToDownload = "C:\\Users\\Asus\\CLionProjects\\ServerPart\\files\\" + fileName;

                        std::ifstream fileToDownload(filePathToDownload, std::ios::in | std::ios::binary);
                        if (!fileToDownload.is_open()) {
                            std::cerr << "Error opening file for reading: " << filePathToDownload << std::endl;
                            closesocket(clientSocket);
                        } else {
                            while (!fileToDownload.eof()) {
                                fileToDownload.read(buffer, sizeof(buffer));
                                bytesRead = static_cast<int>(fileToDownload.gcount());

                                if (bytesRead > 0) {
                                    send(clientSocket, buffer, bytesRead, 0);
                                }
                            }

                            fileToDownload.close();
                            std::cout << "Sent file to the client: " << fileName << std::endl;
                            closesocket(clientSocket);
                        }
                    }
                }
            }
        }
    }

    // Закрываем серверный сокет
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
