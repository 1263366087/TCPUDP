#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <thread> // 使用C++11标准的线程库
#include <vector>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define BUFFER_SIZE 1024

// 两个vector数组存放线程和套接字
std::vector<std::thread> clientThreads;
std::vector<SOCKET> clientSocket;

// 函数声明
void ReceiveMessages(SOCKET client_socket);
void SendMessages();

int maicscn()
{
	// 初始化Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStartup初始化失败。" << std::endl;
		return -1;
	}

	// 创建套接字
	SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == INVALID_SOCKET)
	{
		std::cerr << "创建套接字失败。错误代码: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return -1;
	}

	// 初始化服务器地址结构
	sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	// 绑定套接字
	if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == SOCKET_ERROR)
	{
		std::cerr << "绑定套接字失败。错误代码: " << WSAGetLastError() << std::endl;
		closesocket(server_socket);
		WSACleanup();
		return -1;
	}

	// 监听传入连接
	if (listen(server_socket, 5) == SOCKET_ERROR)
	{
		std::cerr << "监听连接失败。错误代码: " << WSAGetLastError() << std::endl;
		closesocket(server_socket);
		WSACleanup();
		return -1;
	}

	std::cout << "服务器启动，等待客户端连接..." << std::endl;

	// 接受连接
	SOCKET client_socket = accept(server_socket, NULL, NULL);

	if (client_socket == INVALID_SOCKET)
	{
		std::cerr << "接受连接失败。错误代码: " << WSAGetLastError() << std::endl;
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}

	// 存客户端连接
	clientSocket.push_back(client_socket);

	// 获取客户端信息
	sockaddr_in client_info;
	int client_info_size = sizeof(client_info);
	getpeername(client_socket, (struct sockaddr *)&client_info, &client_info_size);
	std::cout << "连接到客户端(" << client_socket << ")：" << inet_ntoa(client_info.sin_addr) << ":" << ntohs(client_info.sin_port) << std::endl;

	// 创建发送消息的线程
	clientThreads.emplace_back(SendMessages);
	// 创建接收消息的线程
	clientThreads.emplace_back(ReceiveMessages, client_socket);

	while (true)
	{
		SOCKET client_socket = accept(server_socket, NULL, NULL);
		if (client_socket == INVALID_SOCKET)
		{
			std::cerr << "接受连接失败。错误代码: " << WSAGetLastError() << std::endl;
			closesocket(server_socket);
			WSACleanup();
			break;
		}
		clientSocket.push_back(client_socket);

		// 获取客户端信息
		sockaddr_in client_info;
		int client_info_size = sizeof(client_info);
		getpeername(client_socket, (struct sockaddr *)&client_info, &client_info_size);
		std::cout << "连接到客户端(" << client_socket << ")：" << inet_ntoa(client_info.sin_addr) << ":" << ntohs(client_info.sin_port) << std::endl;

		// 创建接收消息的线程
		clientThreads.emplace_back(ReceiveMessages, client_socket);
	}

	for (auto &client_socket : clientSocket)
	{
		closesocket(client_socket);
	}

	// 等待所有客户端线程结束
	for (auto &thread : clientThreads)
	{
		thread.join();
	}

	// 关闭套接字
	closesocket(server_socket);

	// 清理Winsock
	WSACleanup();

	return 0;
}
void SendMessages()
{
	// 主线程写入数据
	char message[BUFFER_SIZE];
	int recivenum;

	while (true)
	{
		// 循环直到用户输入一个整数
		std::cout << "请输入消息的接收方(一个整数表示对方): ";

		int recivenum;
		std::cin.clear();  // 清除输入缓冲区的错误标志
		std::cin.ignore(); // 忽略当前行的剩余字符
		while (!(std::cin >> recivenum))
		{
			std::cin.clear();  // 清除输入缓冲区的错误标志
			std::cin.ignore(); // 忽略当前行的剩余字符
			std::cerr << "输入错误：请输入一个有效的整数。" << std::endl;
		}

		std::cin.ignore(); // 清除输入缓冲区的换行符

		memset(message, 0, sizeof(message));
		std::cout << "请输入消息: ";
		std::cin >> message;
		std::cin.clear();  // 清除输入缓冲区的错误标志
		std::cin.ignore(); // 忽略当前行的剩余字符
		if (send((SOCKET)recivenum, message, strlen(message) + 1, 0) == SOCKET_ERROR)
		{
			std::cerr << "发送消息失败。错误代码: " << WSAGetLastError() << std::endl;
		}
	}
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
			std::cout << "连接(" << client_socket << ")关闭" << std::endl;
			break;
		}
		else
		{
			std::cout << "Client(" << client_socket << ")"
					  << ": " << buffer << std::endl;
		}
	}
}
