// TCP�ͻ��� - Windows�汾
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <thread>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define BUFFER_SIZE 1024

// ��������
void ReceiveMessages(SOCKET client_socket);

int main1()
{
	// ��ʼ��Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStartupʧ�ܡ�" << std::endl;
		return 1;
	}

	// �����׽���
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET)
	{
		std::cerr << "�����׽���ʧ�ܡ��������: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	// ��ʼ����������ַ�ṹ
	sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	// ���ӷ�����
	if (connect(client_socket, ( struct sockaddr * ) &server_address, sizeof(server_address)) == SOCKET_ERROR)
	{
		std::cerr << "���ӷ�����ʧ�ܡ��������: " << WSAGetLastError() << std::endl;
		closesocket(client_socket);
		WSACleanup();
		return -1;
	}

	std::cout << "�ɹ����ӵ������������Կ�ʼ�����ˣ�" << std::endl;

	// ����������Ϣ���߳�
	std::thread receiveThread(ReceiveMessages, client_socket);

	// ѭ��д��
	char message[BUFFER_SIZE];
	while (true)
	{
		memset(message, 0, sizeof(message));
		std::cin.clear();  // ������뻺�����Ĵ����־
		std::cin.ignore(); // ���Ե�ǰ�е�ʣ���ַ�

		std::cout << "��������Ϣ: ";
		std::cin >> message;
		if (send(client_socket, message, strlen(message) + 1, 0) == SOCKET_ERROR)
		{
			std::cerr << "������Ϣʧ�ܡ��������: " << WSAGetLastError() << std::endl;
			break;
		}
	}

	// �ر��׽���
	closesocket(client_socket);

	// �ȴ�������Ϣ���߳̽���
	receiveThread.join();

	// ����Winsock
	WSACleanup();

	return 0;
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
			std::cout << "�������ر����ӡ�" << std::endl;
			break;
		}
		else
		{
			std::cout << "Server: " << buffer << std::endl;
		}
	}

	// �ر��׽���
	closesocket(client_socket);
}
