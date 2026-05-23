#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string> 
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")


int main () {
    SetConsoleOutputCP(CP_UTF8);
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0) {
        std::cout << "Ошибка запуска WinSock: " << result << std::endl;
        return 1;
    }

    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (ListenSocket == INVALID_SOCKET) {
        std::cout << "Ошибка создания сокета: " << WSAGetLastError() << std::endl; 
        WSACleanup();
        return 1;
    }

    std::cout << "Сокет успешно создан" << std::endl; 

    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(8080);


    if (bind(ListenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        std::cout << "Ошибка bind: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }


    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Ошибка listen: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Сервер запущен. Ожидание подключений..." << std::endl;

    sockaddr_in clientAddr;
    int clientAddrsize = sizeof(clientAddr);


    while(true) {
        SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&clientAddr, &clientAddrsize);
        if (ClientSocket == INVALID_SOCKET) {
            std::cout << "Ошибка ... : " << WSAGetLastError() << std::endl;
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        std::cout << "Клиент подключился!" << std::endl;


        char buffer[1025];
        int bytesReceived = recv(ClientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << "Сообщение: " << buffer << std::endl;
        } 

        std::string response = 
        "HTTP/1.1 200 OK\r\n"        // Статус: все хорошо
        "Content-Type: text/plain\r\n" // Тип данных: простой текст
        "Content-Length: 18\r\n"     // Длина текста (Hello from server!)
        "Connection: close\r\n"      // Закрываем соединение после ответа
        "\r\n"                       // Пустая строка ОБЯЗАТЕЛЬНА — она отделяет заголовки от текста
        "Hello from server!";

        send(ClientSocket, response.c_str(), response.length(), 0); 

        closesocket(ClientSocket);
    }
}