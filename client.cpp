// TCP客户端 - Windows版本
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <thread>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define BUFFER_SIZE 1024

// 函数声明
void ReceiveMessages(SOCKET client_socket);

int main1()
{
	// 初始化Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStartup失败。" << std::endl;
		return 1;
	}

	// 创建套接字
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET)
	{
		std::cerr << "创建套接字失败。错误代码: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	// 初始化服务器地址结构
	sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	// 连接服务器
	if (connect(client_socket, ( struct sockaddr * ) &server_address, sizeof(server_address)) == SOCKET_ERROR)
	{
		std::cerr << "连接服务器失败。错误代码: " << WSAGetLastError() << std::endl;
		closesocket(client_socket);
		WSACleanup();
		return -1;
	}

	std::cout << "成功连接到服务器。可以开始聊天了！" << std::endl;

	// 创建接收消息的线程
	std::thread receiveThread(ReceiveMessages, client_socket);

	// 循环写入
	char message[BUFFER_SIZE];
	while (true)
	{
		memset(message, 0, sizeof(message));
		std::cin.clear();  // 清除输入缓冲区的错误标志
		std::cin.ignore(); // 忽略当前行的剩余字符

		std::cout << "请输入消息: ";
		std::cin >> message;
		if (send(client_socket, message, strlen(message) + 1, 0) == SOCKET_ERROR)
		{
			std::cerr << "发送消息失败。错误代码: " << WSAGetLastError() << std::endl;
			break;
		}
	}

	// 关闭套接字
	closesocket(client_socket);

	// 等待接收消息的线程结束
	receiveThread.join();

	// 清理Winsock
	WSACleanup();

	return 0;
}

// 接收消息的线程函数
void ReceiveMessages(SOCKET client_socket)
{
	char buffer[BUFFER_SIZE];
	while (true)
	{
		memset(buffer, 0, sizeof(buffer));
		int bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "接收消息失败。错误代码: " << WSAGetLastError() << std::endl;
			break;
		}
		else if (bytesReceived == 0)
		{
			std::cout << "服务器关闭连接。" << std::endl;
			break;
		}
		else
		{
			std::cout << "Server: " << buffer << std::endl;
		}
	}

	// 关闭套接字
	closesocket(client_socket);
}
