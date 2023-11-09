// TCP服务器
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll

#define SERVER_IP "127.0.0.1"
#define PORT 5000
#define BUFFER_SIZE 1024

int main()
{
	// 初始化Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		fprintf(stderr, "WSAStartup failed.\n");
		return 1;
	}

	// 创建套接字
	SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		fprintf(stderr, "Error creating socket: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// 初始化服务器地址结构
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	while (1)
	{
		char message[BUFFER_SIZE];
		memset(message, 0, sizeof(message));

		// 获取用户输入
		printf("请输入消息: ");
		fgets(message, sizeof(message), stdin);

		// 发送数据
		if (sendto(clientSocket, message, strlen(message) + 1, 0, ( struct sockaddr * ) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			fprintf(stderr, "Send failed: %d\n", WSAGetLastError());
			break;
		}
	}

	// 关闭套接字
	closesocket(clientSocket);

	// 清理Winsock
	WSACleanup();

	return 0;
}
