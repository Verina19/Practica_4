#include "SERV_STAT.h"
#include "TCPserver.h"
#include "TCPclient.h"
#include "VECTOR.h"

LinkStatisticsService statisticsService;

// Пеобразование HEX в Char
char HexToChar(std::string hex) {
    unsigned int x;
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> x;
    return (char)x;
}

// Поиск подстроки и чистка строки (парсинг)
std::string find_url(std::string s, std::string url_s) {
    std::string url_d = "", url = "";
    unsigned int i = s.find(url_s);//ищем подстроку
    if (i != std::string::npos) {//если нашли
        for (unsigned int j = i + url_s.size(); j < s.size(); j++) {//создаем грязную строку
            if (s[j] == '=') {
                j++;
            }
            if (s[j] == '&' || s[j] == ' ') break;
            url_d += s[j];//копируем грязную строку
        }
        for (i = 0; i < url_d.size(); i++) {//очищаем ссылку
            if (url_d[i] == '%') {//если %, то заносим следующие 2 буквы 16-го кода в буфер2
                std::string buf2 = "";
                i++;
                buf2 += url_d[i];
                i++;
                buf2 += url_d[i];
                url += HexToChar(buf2);//отправляем на декодирование
            }
            else {
                url += url_d[i];
            }
        }
    }
    return url;
}


void Client(int client_socket) {

    char buf[2024];
        int t;

    int result = ReceiveTCP(client_socket, buf);

    std::stringstream response; // сюда будет записываться ответ клиенту
    std::stringstream response_body; // тело ответа
    std::stringstream stat;

    if (result > 0) {
        buf[result] = '\0'; // Мы знаем фактический размер полученных данных, поэтому ставим метку конца строки В буфере запроса.
        std::string long_url = "", short_url = "", url = "", Err = "нет";
        std::string buf1 = "", buf2 = "", clientIP = "", tim = "";

        for (int i = 0; i < 4; i++) {
            buf1 += buf[i];
        }
        std::string s = "";
        s.assign(buf);

        if (buf1 == "POST") {
            do {
                if (s.find("/report") == std::string::npos) {//ищем подстроку
                 // проверяем на запрос POST /report
                //репорта нет, значит POST от сервера ссылок, отправляем принятую строку с данными транзитом на базу
                    SOCKET ClientSock = TCPclient(IP_BASE, PORT_BASE);// соединение с базой даных
                    if (!ClientSock) {
                        Err = "Ошибка записи в базу данных";
                        break;
                    }
                    Transmit(ClientSock, s);
                    Close(ClientSock);
                }
                else {// report
                    s = "GET_count";// формируем запрос в базу на кол-во запмсей
                    SOCKET ClientSock = TCPclient(IP_BASE, PORT_BASE);// соединение с базой даных
                    if (!ClientSock) {
                        Err = "Ошибка запроса базы данных";
                        break;
                    }
                    Transmit(ClientSock, s);// запрашиваем count
                    s = Receive(ClientSock);// получаем
                    Close(ClientSock);
                    int count = stoi(s);// в инт

                    for (int i = 0; i < count; i++) {// запрашиваем последовательно все записи

                        s = "GET_data data=" + std::to_string(i);
                        SOCKET ClientSock = TCPclient(IP_BASE, PORT_BASE);// соединение с базой даных
                        if (!ClientSock) {
                            Err = "Ошибка запроса базы данных";
                            break;
                        }
                        Transmit(ClientSock, s);//
                        s = Receive(ClientSock);// получаем запись
                        Close(ClientSock);
                        short_url = find_url(s, "short_url=");// парсим
                        long_url = find_url(s, "long_url=");
                        clientIP = find_url(s, "clientIP=");
                        tim = find_url(s, "time=");
                        url = long_url + "(" + short_url + ")";
                        statisticsService.Statist(s, url, clientIP, tim);// отправляем на обрабортку


                    }
                }

            } while (false);
        }

        response_body = statisticsService.main2();// готовим ответ серверу
                // Формируем весь ответ вместе с заголовками
        response << "HTTP/1.1 200 OK\r\n"
            << "Version: HTTP/1.1\r\n"
            << "Content-Type: text/html; charset=cp1251\r\n"
            << "Content-Length: " << response_body.str().length()
            << "\r\n\r\n"
            << response_body.str();

        // Отправляем ответ клиенту с помощью функции send
        SendToTCP(client_socket, response.str());


    }



    CloseClient(client_socket);

}


int main() {
    //main2();
    //nlohmann::json j = {
    //    {"name", "John"},
    //    {"age", 30},
    //    {"isStudent", true}
    //};
    //std::cout << j.dump(4) << std::endl;
    //std::string name = j["name"];
    //int age = j["age"];
    //j["isStudent"] = false;
    //std::cout << j.dump(4) << std::endl;

    setlocale(LC_ALL, "");
    char clientIP[22];
    int client_socket = INVALID_SOCKET;
    int listen_socket = TCPserver(IP_STAT, PORT_STAT);
    if (listen_socket < 0) return 0;;

    ;
    for (;;) {
        client_socket = Accept(clientIP, listen_socket);

        //клиент подключен
        std::thread thr(Client, client_socket);
        thr.detach();
        //thr.join();

    }
    CloseTCP(listen_socket);
    // Убираем за собой






    return 0;
}
