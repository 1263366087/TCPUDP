#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>
#include <winsock2.h>

#define SERVER_IP "127.0.0.1"
#define PORT 5000
#define BUFFER_SIZE 1024

struct sockaddr_in clientAddr;
int clientAddrLen = sizeof(clientAddr);

// 函数声明
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

	clientAddr.sin_port = 1;

	// 创建套接字
	SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	// 初始化服务器地址结构
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	// 绑定套接字
	if (bind(serverSocket, ( struct sockaddr * ) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		fprintf(stderr, "Bind failed: %d\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	// 创建接收线程
	std::thread receiveThread(ReceiveThread, serverSocket);

	while (true)
	{
		char message[BUFFER_SIZE];
		memset(message, 0, sizeof(message));

		if (clientAddr.sin_port == 1)
		{
			std::cout << "等待客户端建立连接！" << std::endl;
		}

        while (clientAddr.sin_port == 1)
		{
			NULL;
		}
		std::cout << "请输入要发送给客户端的消息: ";
		// 获取用户输入
		std::cin.clear();
		std::cin.ignore();
		std::cin>>message;

		// 发送数据
		if (sendto(serverSocket, message, strlen(message) + 1, 0, ( struct sockaddr * ) &clientAddr, sizeof(clientAddr)) == SOCKET_ERROR)
		{
			std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
			continue;
		}
	}

	// 关闭套接字
	closesocket(serverSocket);

	// 等待接收线程结束
	receiveThread.join();

	// 清理Winsock
	WSACleanup();

	return 0;
}

// 接收线程的实现
void ReceiveThread(SOCKET serverSocket)
{
	while (true)
	{
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, sizeof(buffer));

		// 接收数据
		int bytesReceived = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, ( struct sockaddr * ) &clientAddr, &clientAddrLen);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
			continue;
		}

		// 打印服务器发送的消息
		std::cout << "客户端(" << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << ")发送: " << buffer << std::endl;
	}
}
