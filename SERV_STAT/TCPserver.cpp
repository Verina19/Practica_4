#include "TCPserver.h"

#define _WIN32_WINNT 0x501

// Необходимо, чтобы линковка происходила с DLL-библиотекой
// Для работы с сокетам
#pragma comment(lib, "Ws2_32.lib")


sockaddr_in clientInfo;
using std::cerr;
const int max_client_buffer_size = 2024;
struct addrinfo* addr = NULL; // структура, хранящая информацию об IP-адресе  слушающего сокета

SOCKET TCPserver(const char* IP_SERV, const char* PORT_NUM) {
SOCKET listen_socket;
    // Этап 1: Инициализация сокетных интерфейсов Win32API
    // структура типа WSADATA, в которую автоматически в момент создания загружаются данные о версии сокетов, используемых ОС

    WSADATA wsaData; // служебная структура для хранение информации
    // о реализации Windows Sockets
    // старт использования библиотеки сокетов процессом
    // (подгружается Ws2_32.dll)
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    // WSAStartup() в случае успеха возвращает 0, 
// а в случае каких-то проблем возвращает код ошибки, который можно расшифровать последующим вызовом функции WSAGetLastError()

    // Если произошла ошибка подгрузки библиотеки
    if (result != 0) {
        cerr << "WSAStartup failed: " << result << "\n";
        return result;
    }


    // Шаблон для инициализации структуры адреса
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_INET; // AF_INET определяет, что будет
    // использоваться сеть для работы с сокетом
    hints.ai_socktype = SOCK_STREAM; // Задаем потоковый тип сокета
    hints.ai_protocol = IPPROTO_TCP; // Используем протокол TCP
    hints.ai_flags = AI_PASSIVE; // Сокет будет биндиться на адрес,
    // чтобы принимать входящие соединения

    // Инициализируем структуру, хранящую адрес сокета - addr
    // Наш HTTP-сервер будет висеть на 8000-м порту локалхоста
    result = getaddrinfo(IP_SERV, PORT_NUM, &hints, &addr);

    // Если инициализация структуры адреса завершилась с ошибкой,
    // выведем сообщением об этом и завершим выполнение программы
    if (result != 0) {
        cerr << "getaddrinfo failed: " << result << "\n";
        WSACleanup(); // выгрузка библиотеки Ws2_32.dll
        return 1;
    }

    /* Этап 2 : Создание сокета и его инициализация
 Функция socket() возвращает дескриптор с номером сокета, под которым он зарегистрирован в ОС.
Если же инициализировать сокет по каким-то причинам не удалось – возвращается значение INVALID_SOCKET
Важно!!! после работы приложения обязательно закрыть использовавшиеся сокеты с помощью функции closesocket(SOCKET <имя сокета>)
и деинициализировать сокеты Win32API через вызов метода WSACleanup()
SOCKET socket(int <семейство используемых адресов>, int <тип сокета>, int <тип протокола>)
*/
// Создание сокета
    listen_socket = socket(addr->ai_family, addr->ai_socktype,
        addr->ai_protocol);
    // Если создание сокета завершилось с ошибкой, выводим сообщение,
    // освобождаем память, выделенную под структуру addr,
    // выгружаем dll-библиотеку и закрываем программу
    if (listen_socket == INVALID_SOCKET) {
        cerr << "Error at socket: " << WSAGetLastError() << "\n";
        freeaddrinfo(addr);
        WSACleanup();
        return 1;
    }

    /* Этап 3: Привязка сокета к паре IP-адрес/Порт
 int bind(SOCKET <имя сокета, к которому необходимо привязать адрес и порт>,
        sockaddr* <указатель на структуру, содержащую детальную информацию по адресу и порту, к которому надо привязать сокет>,
        int <размер структуры, содержащей адрес и порт>)
 Функция bind() возвращает 0, если удалось успешно привязать сокет к адресу и порту, и код ошибки в ином случае
*/
// Привязываем сокет к IP-адресу
    result = bind(listen_socket, addr->ai_addr, (int)addr->ai_addrlen);

    // Если привязать адрес к сокету не удалось, то выводим сообщение
    // об ошибке, освобождаем память, выделенную под структуру addr.
    // и закрываем открытый сокет.
    // Выгружаем DLL-библиотеку из памяти и закрываем программу.
    if (result == SOCKET_ERROR) {
        cerr << "bind failed with error: " << WSAGetLastError() << "\n";
        freeaddrinfo(addr);
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    /*Этап 4 (для сервера): «Прослушивание» привязанного порта для идентификации подключений
    Для того, чтобы реализовать данный этап, нужно вызвать функцию listen()
    int listen(SOCKET <«слушающий» сокет, который мы создавали на предыдущих этапах>,
               int <максимальное количество процессов, разрешенных к подключению>)
    Второй аргумент: максимально возможное число подключений устанавливается через передачу параметр SOMAXCONN(рекомендуется).
    Если нужно установить ограничения на количество подключений – нужно указать SOMAXCONN_HINT(N), где N – кол-во подключений.
    Если будет подключаться больше пользователей, то они будут сброшены.
   */
   // Инициализируем слушающий сокет
    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "listen failed with error: " << WSAGetLastError() << "\n";
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "Сервер запущен. Listening..." << std::endl;

    return listen_socket;
}
// Ожидание подключения клиента
SOCKET Accept(char* clientIP, SOCKET listen_socket) {
    /*Этап 5 (только для Сервера). Подтверждение подключения
После начала прослушивания (вызов функции listen()) следующей функцией должна идти функция accept(),
которую будет искать программа после того, как установится соединение с Клиентом.
Прототип функции accept():
SOCKET accept(SOCKET <"слушающий" сокет на стороне Сервера>,
              sockaddr* <указатель на пустую структуру sockaddr, в которую будет записана информация по подключившемуся Клиенту>,
              int* <указатель на размер структуры типа sockaddr>)
 Функция accept() возвращает номер дескриптора, под которым зарегистрирован сокет в ОС.
 Если произошла ошибка, то возвращается значение INVALID_SOCKET
*/
    int clientInfo_size = sizeof(clientInfo);
    // Принимаем входящие соединения
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
// Закрытие ТСР соединения
void CloseTCP(SOCKET listen_socket) {
    closesocket(listen_socket);
    freeaddrinfo(addr);
    WSACleanup();
}
//Закрытие клиента
void CloseClient(SOCKET client_socket) {
    closesocket(client_socket);
}
// Прием от клиента
int ReceiveTCP(SOCKET client_socket, char* buf) {
    int result = recv(client_socket, buf, max_client_buffer_size, 0);
    if (result == SOCKET_ERROR) {
        // ошибка получения данных
        cerr << "recv failed: " << result << "\n";
        closesocket(client_socket);
        return -1;
    }
    else if (result == 0) {
        // соединение закрыто клиентом
        cerr << "connection closed...\n";
        return -1;
    }
    return result;
}
// Передача клиенту
int SendToTCP(SOCKET client_socket, std::string response) {
    int result = send(client_socket, response.c_str(), response.length(), 0);
    if (result == SOCKET_ERROR) {
        // произошла ошибка при отправке данных
        cerr << "send failed: " << WSAGetLastError() << "\n";
    }
    return result;
}
