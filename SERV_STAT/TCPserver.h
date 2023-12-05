#pragma once
#include <iostream>
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

SOCKET TCPserver(const char* IP_SERV, const char* PORT_NUM);
SOCKET Accept(char* clientIP, SOCKET listen_socket);
void CloseTCP(SOCKET listen_socket);
void CloseClient(SOCKET client_socket);
int ReceiveTCP(SOCKET client_socket, char* buf);
int SendToTCP(SOCKET client_socket, std::string response);