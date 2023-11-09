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

// ��������
void ReceiveThread(SOCKET clientSocket);

int main()
{
	// ��ʼ��Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStartup failed." << std::endl;
		return 1;
	}

	clientAddr.sin_port = 1;

	// �����׽���
	SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	// ��ʼ����������ַ�ṹ
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	// ���׽���
	if (bind(serverSocket, ( struct sockaddr * ) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		fprintf(stderr, "Bind failed: %d\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	// ���������߳�
	std::thread receiveThread(ReceiveThread, serverSocket);

	while (true)
	{
		char message[BUFFER_SIZE];
		memset(message, 0, sizeof(message));

		if (clientAddr.sin_port == 1)
		{
			std::cout << "�ȴ��ͻ��˽������ӣ�" << std::endl;
		}

        while (clientAddr.sin_port == 1)
		{
			NULL;
		}
		std::cout << "������Ҫ���͸��ͻ��˵���Ϣ: ";
		// ��ȡ�û�����
		std::cin.clear();
		std::cin.ignore();
		std::cin>>message;

		// ��������
		if (sendto(serverSocket, message, strlen(message) + 1, 0, ( struct sockaddr * ) &clientAddr, sizeof(clientAddr)) == SOCKET_ERROR)
		{
			std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
			continue;
		}
	}

	// �ر��׽���
	closesocket(serverSocket);

	// �ȴ������߳̽���
	receiveThread.join();

	// ����Winsock
	WSACleanup();

	return 0;
}

// �����̵߳�ʵ��
void ReceiveThread(SOCKET serverSocket)
{
	while (true)
	{
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, sizeof(buffer));

		// ��������
		int bytesReceived = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, ( struct sockaddr * ) &clientAddr, &clientAddrLen);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
			continue;
		}

		// ��ӡ���������͵���Ϣ
		std::cout << "�ͻ���(" << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << ")����: " << buffer << std::endl;
	}
}
