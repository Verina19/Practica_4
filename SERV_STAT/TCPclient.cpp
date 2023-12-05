// Client part for Server-Client chat. Developed by Mr_Dezz
#include "TCPclient.h"



#pragma comment(lib, "ws2_32.lib")

using namespace std;

const short BUFF_SIZE = 1024;						// Maximum size of buffer for exchange info between server and client
char servBuff[BUFF_SIZE], clientBuff[BUFF_SIZE];	// Buffers for sending and receiving data


std::string Receive(SOCKET ClientSock) {
	short packet_size = 0;
	size_t i = 0;

	//while (true) {
		//����� ��������� �� �������
		ZeroMemory(&servBuff, sizeof(servBuff));
		packet_size = recv(ClientSock, servBuff, BUFF_SIZE, 0);
		std::string buf1;

		//if (!packet_size) continue;
		if (packet_size == SOCKET_ERROR) {// ��������� ������
			cout << "Can't receive message from Server. Error # " << WSAGetLastError() << endl;
			closesocket(ClientSock);
			WSACleanup();
			return "Error";
		}
		else {
			//cout << "Server message: " << endl;
			//for (i = 0; i < BUFF_SIZE; i++) {// ������� ������ �� ������ � ��������� ����������� 
			//	if (servBuff[i] == '\0') break;
			//	cout << servBuff[i];
			//}
			//cout << endl;
			buf1.assign(servBuff);
		}

	//}
		return buf1;
}

void Transmit(SOCKET ClientSock, std::string clientBuffS) {
	size_t i = 0;

	short packet_size = 0;			// The size of sending / receiving packet in bytes

	const char* clientBuff = clientBuffS.c_str();
	for (i = 0; i < BUFF_SIZE; i++) {// ��������� ����� ������ �������
		if (clientBuff[i] == '\0') break;
	}
	int z = strlen(clientBuff);

	packet_size = send(ClientSock, clientBuff, i + 1, 0);// ���������� ������ �������
	if (packet_size == SOCKET_ERROR) {// ��������� ������
		cout << "Can't send message to Server. Error # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		return;
	}



}

void Close(SOCKET ClientSock) {
	closesocket(ClientSock);
	WSACleanup();
}

SOCKET TCPclient(const char* IP_CL, const short PORT) {
//���� ��������� ���������� ������ � ��������, �� � �������� �������� ��� ���������� ����� ���������� IP-�����.
//�� ���� �� ����� ����� IP � ���� ����������. �� ���� ��������� ���������� �������� ����� �����, ������, �������,
//�� ����� �� ����� ����� ������ ���� (� ������ ��� ��� ����� �����). ��� ��� ������ �� ������ ��� ���� ���� IP-����� � 127.0.0.1 (localhost)

// Key variables for all program
	int erStat;										// For checking errors in sockets functions

//IP �� ������� ������ ����������� � �������� ������ ��� ������� socket. ������ � "ip_to_num"
	in_addr ip_to_num;
//�������, ������� ��������� ������� ������ ���� char[], ���������� IPv4 ����� � ��������� ���� � �������-�������������
// � ��������� ���� in_addr
	inet_pton(AF_INET, IP_CL, &ip_to_num);


// ���� 1: ������������� �������� ����������� Win32API
// ��������� ���� WSADATA, � ������� ������������� � ������ �������� ����������� ������ � ������ �������, ������������ ��
// WinSock initialization
	WSADATA wsData;
// WSAStartup() � ������ ������ ���������� 0, 
// � � ������ �����-�� ������� ���������� ��� ������, ������� ����� ������������ ����������� ������� ������� WSAGetLastError()
	erStat = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (erStat != 0) {// ��������� ������
		cout << "Error WinSock version initializaion #";
		cout << WSAGetLastError();
		return 0;
	}
	else
		cout << "WinSock initialization is OK" << endl;

/* ���� 2: �������� ������ � ��� �������������
 ������� socket() ���������� ���������� � ������� ������, ��� ������� �� ��������������� � ��.
���� �� ���������������� ����� �� �����-�� �������� �� ������� � ������������ �������� INVALID_SOCKET
�����!!! ����� ������ ���������� ����������� ������� ���������������� ������ � ������� ������� closesocket(SOCKET <��� ������>)
� ������������������ ������ Win32API ����� ����� ������ WSACleanup()
SOCKET socket(int <��������� ������������ �������>, int <��� ������>, int <��� ���������>)
*/
// Socket initialization
	SOCKET ClientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (ClientSock == INVALID_SOCKET) {// ��������� ������
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		return 0;
	}
	else
		cout << "Client socket initialization is OK" << endl;

/*���� 4 (��� �������). ����������� ����������� � �������
��� ��� ������� �� �������� ����� �������� ���� ����� : ���������� ���������� ������ 0, 1 � 2.
�������� ������ � ����������� ��������(bind()) �� ���������,
�.�.����� ����� �������� � ���������� ������ � ����� ����� ����� ������� connect()(�� ���� ������ bind() ��� �������).
����������, ����� �������� � ������������� ������ �� ���������� �������, ����� ������� ��������� ������� connect().

 int connect(SOCKET <������������������ �����>, sockaddr* <��������� �� ���������, ���������� IP-����� � ���� �������>,
			int <������ ��������� sockaddr>)

 ������� ���������� 0 � ������ ��������� ����������� � ��� ������ � ���� ������.
 ��������� �� ���������� ������ � ��������� sockaddr ���������� ����, ��� ��� �������� �� ����� 3 ��� ������� ��� ������ ������� bind()
 ������������� ������ ������ � � ��� ��������� ��� ������� ������ ���������� ���������� � �������,
 �.�. IPv4-����� ������� � ����� ����������� ����� �� �������.
*/
// Establishing a connection to Server
	sockaddr_in clientInfo;// ����������� ��������� ��� �������������� IP ������, ����� � ��. � ����. �������� ������ ������ ������� connect()
	ZeroMemory(&clientInfo, sizeof(clientInfo));// Initializing servInfo structure  ��������� ���� ������ ������

	clientInfo.sin_family = AF_INET;// AF_INET - IPv4
	clientInfo.sin_addr = ip_to_num;// IP-����� � �������� ����
	clientInfo.sin_port = htons(PORT);//���� ������ ����������� ����� ����� ������� htons(),
											   //������� ��������������� ��������� �������� �������� ����� ���� unsigned short
											   //� ���������� ������� �������� ��� ��������� TCP/IP

	erStat = connect(ClientSock, (sockaddr*)&clientInfo, sizeof(clientInfo));
	if (erStat != 0) {// ��������� ������
		cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		return 0;
	}
	else
		cout << "Connection established SUCCESSFULLY. Ready to send a message to Server" << endl;

	return ClientSock;
}


