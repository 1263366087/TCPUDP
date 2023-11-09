#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <thread> // ʹ��C++11��׼���߳̿�
#include <vector>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define BUFFER_SIZE 1024

// ����vector�������̺߳��׽���
std::vector<std::thread> clientThreads;
std::vector<SOCKET> clientSocket;

// ��������
void ReceiveMessages(SOCKET client_socket);
void SendMessages();

int maicscn()
{
	// ��ʼ��Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStartup��ʼ��ʧ�ܡ�" << std::endl;
		return -1;
	}

	// �����׽���
	SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == INVALID_SOCKET)
	{
		std::cerr << "�����׽���ʧ�ܡ��������: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return -1;
	}

	// ��ʼ����������ַ�ṹ
	sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	// ���׽���
	if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == SOCKET_ERROR)
	{
		std::cerr << "���׽���ʧ�ܡ��������: " << WSAGetLastError() << std::endl;
		closesocket(server_socket);
		WSACleanup();
		return -1;
	}

	// ������������
	if (listen(server_socket, 5) == SOCKET_ERROR)
	{
		std::cerr << "��������ʧ�ܡ��������: " << WSAGetLastError() << std::endl;
		closesocket(server_socket);
		WSACleanup();
		return -1;
	}

	std::cout << "�������������ȴ��ͻ�������..." << std::endl;

	// ��������
	SOCKET client_socket = accept(server_socket, NULL, NULL);

	if (client_socket == INVALID_SOCKET)
	{
		std::cerr << "��������ʧ�ܡ��������: " << WSAGetLastError() << std::endl;
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}

	// ��ͻ�������
	clientSocket.push_back(client_socket);

	// ��ȡ�ͻ�����Ϣ
	sockaddr_in client_info;
	int client_info_size = sizeof(client_info);
	getpeername(client_socket, (struct sockaddr *)&client_info, &client_info_size);
	std::cout << "���ӵ��ͻ���(" << client_socket << ")��" << inet_ntoa(client_info.sin_addr) << ":" << ntohs(client_info.sin_port) << std::endl;

	// ����������Ϣ���߳�
	clientThreads.emplace_back(SendMessages);
	// ����������Ϣ���߳�
	clientThreads.emplace_back(ReceiveMessages, client_socket);

	while (true)
	{
		SOCKET client_socket = accept(server_socket, NULL, NULL);
		if (client_socket == INVALID_SOCKET)
		{
			std::cerr << "��������ʧ�ܡ��������: " << WSAGetLastError() << std::endl;
			closesocket(server_socket);
			WSACleanup();
			break;
		}
		clientSocket.push_back(client_socket);

		// ��ȡ�ͻ�����Ϣ
		sockaddr_in client_info;
		int client_info_size = sizeof(client_info);
		getpeername(client_socket, (struct sockaddr *)&client_info, &client_info_size);
		std::cout << "���ӵ��ͻ���(" << client_socket << ")��" << inet_ntoa(client_info.sin_addr) << ":" << ntohs(client_info.sin_port) << std::endl;

		// ����������Ϣ���߳�
		clientThreads.emplace_back(ReceiveMessages, client_socket);
	}

	for (auto &client_socket : clientSocket)
	{
		closesocket(client_socket);
	}

	// �ȴ����пͻ����߳̽���
	for (auto &thread : clientThreads)
	{
		thread.join();
	}

	// �ر��׽���
	closesocket(server_socket);

	// ����Winsock
	WSACleanup();

	return 0;
}
void SendMessages()
{
	// ���߳�д������
	char message[BUFFER_SIZE];
	int recivenum;

	while (true)
	{
		// ѭ��ֱ���û�����һ������
		std::cout << "��������Ϣ�Ľ��շ�(һ��������ʾ�Է�): ";

		int recivenum;
		std::cin.clear();  // ������뻺�����Ĵ����־
		std::cin.ignore(); // ���Ե�ǰ�е�ʣ���ַ�
		while (!(std::cin >> recivenum))
		{
			std::cin.clear();  // ������뻺�����Ĵ����־
			std::cin.ignore(); // ���Ե�ǰ�е�ʣ���ַ�
			std::cerr << "�������������һ����Ч��������" << std::endl;
		}

		std::cin.ignore(); // ������뻺�����Ļ��з�

		memset(message, 0, sizeof(message));
		std::cout << "��������Ϣ: ";
		std::cin >> message;
		std::cin.clear();  // ������뻺�����Ĵ����־
		std::cin.ignore(); // ���Ե�ǰ�е�ʣ���ַ�
		if (send((SOCKET)recivenum, message, strlen(message) + 1, 0) == SOCKET_ERROR)
		{
			std::cerr << "������Ϣʧ�ܡ��������: " << WSAGetLastError() << std::endl;
		}
	}
}

// ������Ϣ���̺߳���
void ReceiveMessages(SOCKET client_socket)
{
	char buffer[BUFFER_SIZE];
	while (true)
	{
		memset(buffer, 0, sizeof(buffer));
		int bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "������Ϣʧ�ܡ��������: " << WSAGetLastError() << std::endl;
			break;
		}
		else if (bytesReceived == 0)
		{
			std::cout << "����(" << client_socket << ")�ر�" << std::endl;
			break;
		}
		else
		{
			std::cout << "Client(" << client_socket << ")"
					  << ": " << buffer << std::endl;
		}
	}
}
