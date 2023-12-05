#pragma once
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include "windows.h"
#include <thread>

//Key constants
SOCKET TCPclient(const char* IP_CL, const short PORT);
std::string Receive(SOCKET ClientSock);
void Transmit(SOCKET ClientSock, std::string clientBuff);
void Close(SOCKET ClientSock);
