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
		//прием сообщения от сервера
		ZeroMemory(&servBuff, sizeof(servBuff));
		packet_size = recv(ClientSock, servBuff, BUFF_SIZE, 0);
		std::string buf1;

		//if (!packet_size) continue;
		if (packet_size == SOCKET_ERROR) {// обработка ошибки
			cout << "Can't receive message from Server. Error # " << WSAGetLastError() << endl;
			closesocket(ClientSock);
			WSACleanup();
			return "Error";
		}
		else {
			//cout << "Server message: " << endl;
			//for (i = 0; i < BUFF_SIZE; i++) {// выводим массив на печать и вычисляем точнуюдлину 
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
	for (i = 0; i < BUFF_SIZE; i++) {// вычисляем длину точную массива
		if (clientBuff[i] == '\0') break;
	}
	int z = strlen(clientBuff);

	packet_size = send(ClientSock, clientBuff, i + 1, 0);// отправляем массив серверу
	if (packet_size == SOCKET_ERROR) {// обработка ошибки
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
//Если программа отправляет данные в интернет, то в качестве адресата она использует ранее полученный IP-адрес.
//То есть ей нужно знать IP и порт получателя. Но если программа отправляет дочерней проге своей, модулю, плагину,
//то здесь ей нужно знать только порт (и обычно она его точно знает). Так как внутри ПК единый для всех один IP-адрес — 127.0.0.1 (localhost)

// Key variables for all program
	int erStat;										// For checking errors in sockets functions

//IP из формата строки переводится в числовой формат для функций socket. Данные в "ip_to_num"
	in_addr ip_to_num;
//функция, которая переводит обычную строку типа char[], содержащую IPv4 адрес в привычном виде с точками-разделителями
// в структуру типа in_addr
	inet_pton(AF_INET, IP_CL, &ip_to_num);


// Этап 1: Инициализация сокетных интерфейсов Win32API
// структура типа WSADATA, в которую автоматически в момент создания загружаются данные о версии сокетов, используемых ОС
// WinSock initialization
	WSADATA wsData;
// WSAStartup() в случае успеха возвращает 0, 
// а в случае каких-то проблем возвращает код ошибки, который можно расшифровать последующим вызовом функции WSAGetLastError()
	erStat = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (erStat != 0) {// обработка ошибки
		cout << "Error WinSock version initializaion #";
		cout << WSAGetLastError();
		return 0;
	}
	else
		cout << "WinSock initialization is OK" << endl;

/* Этап 2: Создание сокета и его инициализация
 Функция socket() возвращает дескриптор с номером сокета, под которым он зарегистрирован в ОС.
Если же инициализировать сокет по каким-то причинам не удалось – возвращается значение INVALID_SOCKET
Важно!!! после работы приложения обязательно закрыть использовавшиеся сокеты с помощью функции closesocket(SOCKET <имя сокета>)
и деинициализировать сокеты Win32API через вызов метода WSACleanup()
SOCKET socket(int <семейство используемых адресов>, int <тип сокета>, int <тип протокола>)
*/
// Socket initialization
	SOCKET ClientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (ClientSock == INVALID_SOCKET) {// обработка ошибки
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		return 0;
	}
	else
		cout << "Client socket initialization is OK" << endl;

/*Этап 4 (для Клиента). Организация подключения к серверу
Код для Клиента до текущего этапа выглядит даже проще : необходимо исполнение Этапов 0, 1 и 2.
Привязка сокета к конкретному процессу(bind()) не требуется,
т.к.сокет будет привязан к серверному Адресу и Порту через вызов функции connect()(по сути аналог bind() для Клиента).
Собственно, после создания и инициализации сокета на клиентской стороне, нужно вызвать указанную функцию connect().

 int connect(SOCKET <инициализированный сокет>, sockaddr* <указатель на структуру, содержащую IP-адрес и Порт сервера>,
			int <размер структуры sockaddr>)

 Функция возвращает 0 в случае успешного подключения и код ошибки в ином случае.
 Процедура по добавлению данных в структуру sockaddr аналогична тому, как это делалось на Этапе 3 для Сервера при вызове функции bind()
 Принципиально важный момент – в эту структуру для клиента должна заноситься информация о сервере,
 т.е. IPv4-адрес сервера и номер «слушающего» порта на сервере.
*/
// Establishing a connection to Server
	sockaddr_in clientInfo;// специальная структура для преобразования IP адреса, порта и др. в спец. числовой формат нужный функции connect()
	ZeroMemory(&clientInfo, sizeof(clientInfo));// Initializing servInfo structure  Заполняет блок памяти нулями

	clientInfo.sin_family = AF_INET;// AF_INET - IPv4
	clientInfo.sin_addr = ip_to_num;// IP-адрес в числовом виде
	clientInfo.sin_port = htons(PORT);//порт всегда указывается через вызов функции htons(),
											   //которая переупаковывает привычное цифровое значение порта типа unsigned short
											   //в побайтовый порядок понятный для протокола TCP/IP

	erStat = connect(ClientSock, (sockaddr*)&clientInfo, sizeof(clientInfo));
	if (erStat != 0) {// обработка ошибки
		cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		return 0;
	}
	else
		cout << "Connection established SUCCESSFULLY. Ready to send a message to Server" << endl;

	return ClientSock;
}


