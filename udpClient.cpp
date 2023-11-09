#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <thread> // ʹ��C++11��׼���߳̿�
#include <iostream>
#include <winsock2.h>

#define SERVER_IP "127.0.0.1"
#define PORT 5000
#define BUFFER_SIZE 1024

struct sockaddr_in serverAddr;
int serverAddrLen = sizeof(serverAddr);

// ��������
void SendThread(SOCKET clientSocket);
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

    // �����׽���
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // ��ʼ����������ַ�ṹ
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // ���������̺߳ͽ����߳�
    std::thread sendThread(SendThread, clientSocket);
    std::thread receiveThread(ReceiveThread, clientSocket);

    // �ȴ������̺߳ͽ����߳̽���
    sendThread.join();
    receiveThread.join();

    // �ر��׽���
    closesocket(clientSocket);

    // ����Winsock
    WSACleanup();

    return 0;
}

// �����̵߳�ʵ��
void SendThread(SOCKET clientSocket)
{
    while (true)
    {
        char message[BUFFER_SIZE];
        memset(message, 0, sizeof(message));

        // ��ȡ�û�����
        std::cout << "�����뷢�͸�����������Ϣ: ";
        std::cin.getline(message, sizeof(message));

        // �������ݵ�������
        if (sendto(clientSocket, message, strlen(message) + 1, 0, ( struct sockaddr * ) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
            break;
        }
    }
}

// �����̵߳�ʵ��
void ReceiveThread(SOCKET clientSocket)
{
    while (true)
    {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));

        // ��������
        int bytesReceived = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, ( struct sockaddr * ) &serverAddr, &serverAddrLen);
        if (bytesReceived == SOCKET_ERROR)
        {
            continue;
        }

        // ��ӡ���������͵���Ϣ
        std::cout << "������(" << inet_ntoa(serverAddr.sin_addr) << ":" << ntohs(serverAddr.sin_port) << ")����: " << buffer << std::endl;
    }
}
