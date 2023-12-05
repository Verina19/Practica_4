#include "TCPserver.h"

#define _WIN32_WINNT 0x501

// ����������, ����� �������� ����������� � DLL-�����������
// ��� ������ � �������
#pragma comment(lib, "Ws2_32.lib")


sockaddr_in clientInfo;
using std::cerr;
const int max_client_buffer_size = 2024;
struct addrinfo* addr = NULL; // ���������, �������� ���������� �� IP-������  ���������� ������

SOCKET TCPserver(const char* IP_SERV, const char* PORT_NUM) {
SOCKET listen_socket;
    // ���� 1: ������������� �������� ����������� Win32API
    // ��������� ���� WSADATA, � ������� ������������� � ������ �������� ����������� ������ � ������ �������, ������������ ��

    WSADATA wsaData; // ��������� ��������� ��� �������� ����������
    // � ���������� Windows Sockets
    // ����� ������������� ���������� ������� ���������
    // (������������ Ws2_32.dll)
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    // WSAStartup() � ������ ������ ���������� 0, 
// � � ������ �����-�� ������� ���������� ��� ������, ������� ����� ������������ ����������� ������� ������� WSAGetLastError()

    // ���� ��������� ������ ��������� ����������
    if (result != 0) {
        cerr << "WSAStartup failed: " << result << "\n";
        return result;
    }


    // ������ ��� ������������� ��������� ������
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_INET; // AF_INET ����������, ��� �����
    // �������������� ���� ��� ������ � �������
    hints.ai_socktype = SOCK_STREAM; // ������ ��������� ��� ������
    hints.ai_protocol = IPPROTO_TCP; // ���������� �������� TCP
    hints.ai_flags = AI_PASSIVE; // ����� ����� ��������� �� �����,
    // ����� ��������� �������� ����������

    // �������������� ���������, �������� ����� ������ - addr
    // ��� HTTP-������ ����� ������ �� 8000-� ����� ����������
    result = getaddrinfo(IP_SERV, PORT_NUM, &hints, &addr);

    // ���� ������������� ��������� ������ ����������� � �������,
    // ������� ���������� �� ���� � �������� ���������� ���������
    if (result != 0) {
        cerr << "getaddrinfo failed: " << result << "\n";
        WSACleanup(); // �������� ���������� Ws2_32.dll
        return 1;
    }

    /* ���� 2 : �������� ������ � ��� �������������
 ������� socket() ���������� ���������� � ������� ������, ��� ������� �� ��������������� � ��.
���� �� ���������������� ����� �� �����-�� �������� �� ������� � ������������ �������� INVALID_SOCKET
�����!!! ����� ������ ���������� ����������� ������� ���������������� ������ � ������� ������� closesocket(SOCKET <��� ������>)
� ������������������ ������ Win32API ����� ����� ������ WSACleanup()
SOCKET socket(int <��������� ������������ �������>, int <��� ������>, int <��� ���������>)
*/
// �������� ������
    listen_socket = socket(addr->ai_family, addr->ai_socktype,
        addr->ai_protocol);
    // ���� �������� ������ ����������� � �������, ������� ���������,
    // ����������� ������, ���������� ��� ��������� addr,
    // ��������� dll-���������� � ��������� ���������
    if (listen_socket == INVALID_SOCKET) {
        cerr << "Error at socket: " << WSAGetLastError() << "\n";
        freeaddrinfo(addr);
        WSACleanup();
        return 1;
    }

    /* ���� 3: �������� ������ � ���� IP-�����/����
 int bind(SOCKET <��� ������, � �������� ���������� ��������� ����� � ����>,
        sockaddr* <��������� �� ���������, ���������� ��������� ���������� �� ������ � �����, � �������� ���� ��������� �����>,
        int <������ ���������, ���������� ����� � ����>)
 ������� bind() ���������� 0, ���� ������� ������� ��������� ����� � ������ � �����, � ��� ������ � ���� ������
*/
// ����������� ����� � IP-������
    result = bind(listen_socket, addr->ai_addr, (int)addr->ai_addrlen);

    // ���� ��������� ����� � ������ �� �������, �� ������� ���������
    // �� ������, ����������� ������, ���������� ��� ��������� addr.
    // � ��������� �������� �����.
    // ��������� DLL-���������� �� ������ � ��������� ���������.
    if (result == SOCKET_ERROR) {
        cerr << "bind failed with error: " << WSAGetLastError() << "\n";
        freeaddrinfo(addr);
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    /*���� 4 (��� �������): �������������� ������������ ����� ��� ������������� �����������
    ��� ����, ����� ����������� ������ ����, ����� ������� ������� listen()
    int listen(SOCKET <���������� �����, ������� �� ��������� �� ���������� ������>,
               int <������������ ���������� ���������, ����������� � �����������>)
    ������ ��������: ����������� ��������� ����� ����������� ��������������� ����� �������� �������� SOMAXCONN(�������������).
    ���� ����� ���������� ����������� �� ���������� ����������� � ����� ������� SOMAXCONN_HINT(N), ��� N � ���-�� �����������.
    ���� ����� ������������ ������ �������������, �� ��� ����� ��������.
   */
   // �������������� ��������� �����
    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "listen failed with error: " << WSAGetLastError() << "\n";
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "������ �������. Listening..." << std::endl;

    return listen_socket;
}
// �������� ����������� �������
SOCKET Accept(char* clientIP, SOCKET listen_socket) {
    /*���� 5 (������ ��� �������). ������������� �����������
����� ������ ������������� (����� ������� listen()) ��������� �������� ������ ���� ������� accept(),
������� ����� ������ ��������� ����� ����, ��� ����������� ���������� � ��������.
�������� ������� accept():
SOCKET accept(SOCKET <"���������" ����� �� ������� �������>,
              sockaddr* <��������� �� ������ ��������� sockaddr, � ������� ����� �������� ���������� �� ��������������� �������>,
              int* <��������� �� ������ ��������� ���� sockaddr>)
 ������� accept() ���������� ����� �����������, ��� ������� ��������������� ����� � ��.
 ���� ��������� ������, �� ������������ �������� INVALID_SOCKET
*/
    int clientInfo_size = sizeof(clientInfo);
    // ��������� �������� ����������
    SOCKET client_socket = accept(listen_socket, (sockaddr*)&clientInfo, &clientInfo_size);
    if (client_socket == INVALID_SOCKET) {
        cerr << "accept failed: " << WSAGetLastError() << "\n";
        closesocket(listen_socket);
        WSACleanup();
        return -1;
    }
    inet_ntop(AF_INET, &clientInfo.sin_addr, clientIP, INET_ADDRSTRLEN);	// Convert connected client's IP to standard string format
    std::cout << "Client connected with IP address " << clientIP << std::endl;

    return client_socket;
}
// �������� ��� ����������
void CloseTCP(SOCKET listen_socket) {
    closesocket(listen_socket);
    freeaddrinfo(addr);
    WSACleanup();
}
//�������� �������
void CloseClient(SOCKET client_socket) {
    closesocket(client_socket);
}
// ����� �� �������
int ReceiveTCP(SOCKET client_socket, char* buf) {
    int result = recv(client_socket, buf, max_client_buffer_size, 0);
    if (result == SOCKET_ERROR) {
        // ������ ��������� ������
        cerr << "recv failed: " << result << "\n";
        closesocket(client_socket);
        return -1;
    }
    else if (result == 0) {
        // ���������� ������� ��������
        cerr << "connection closed...\n";
        return -1;
    }
    return result;
}
// �������� �������
int SendToTCP(SOCKET client_socket, std::string response) {
    int result = send(client_socket, response.c_str(), response.length(), 0);
    if (result == SOCKET_ERROR) {
        // ��������� ������ ��� �������� ������
        cerr << "send failed: " << WSAGetLastError() << "\n";
    }
    return result;
}
