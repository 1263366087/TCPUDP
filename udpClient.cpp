#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <thread> // 使用C++11标准的线程库
#include <iostream>
#include <winsock2.h>

#define SERVER_IP "127.0.0.1"
#define PORT 5000
#define BUFFER_SIZE 1024

struct sockaddr_in serverAddr;
int serverAddrLen = sizeof(serverAddr);

// 函数声明
void SendThread(SOCKET clientSocket);
void ReceiveThread(SOCKET clientSocket);

int main()
{
    // 初始化Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    // 创建套接字
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // 初始化服务器地址结构
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // 创建发送线程和接收线程
    std::thread sendThread(SendThread, clientSocket);
    std::thread receiveThread(ReceiveThread, clientSocket);

    // 等待发送线程和接收线程结束
    sendThread.join();
    receiveThread.join();

    // 关闭套接字
    closesocket(clientSocket);

    // 清理Winsock
    WSACleanup();

    return 0;
}

// 发送线程的实现
void SendThread(SOCKET clientSocket)
{
    while (true)
    {
        char message[BUFFER_SIZE];
        memset(message, 0, sizeof(message));

        // 获取用户输入
        std::cout << "请输入发送给服务器的消息: ";
        std::cin.getline(message, sizeof(message));

        // 发送数据到服务器
        if (sendto(clientSocket, message, strlen(message) + 1, 0, ( struct sockaddr * ) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
            break;
        }
    }
}

// 接收线程的实现
void ReceiveThread(SOCKET clientSocket)
{
    while (true)
    {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));

        // 接收数据
        int bytesReceived = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, ( struct sockaddr * ) &serverAddr, &serverAddrLen);
        if (bytesReceived == SOCKET_ERROR)
        {
            continue;
        }

        // 打印服务器发送的消息
        std::cout << "服务器(" << inet_ntoa(serverAddr.sin_addr) << ":" << ntohs(serverAddr.sin_port) << ")发送: " << buffer << std::endl;
    }
}
