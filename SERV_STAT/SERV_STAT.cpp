#include "SERV_STAT.h"
#include "TCPserver.h"
#include "TCPclient.h"

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
    //s.assign(buf);
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
       // Мы знаем фактический размер полученных данных, поэтому ставим метку конца строки
       // В буфере запроса.
        buf[result] = '\0';
        std::string long_url = "", short_url = "", Err = "нет";
        std::string buf1 = "", buf2 = "", clientIP = "", tim = "";
        char ttt[5];

        for (int i = 0; i < 4; i++) {
            buf1 += buf[i];
        }
        std::string s = "";
        s.assign(buf);

        if (buf1 == "POST") {
            do {
                short_url = find_url(s, "report");
                if (short_url == "") {// server short
                    short_url = find_url(s, "short_url=");
                    long_url = find_url(s, "long_url=");
                    clientIP = find_url(s, "clientIP=");
                    t = stoi(find_url(s, "time="));
                    SOCKET ClientSock = TCPclient(IP_BASE, PORT_BASE);// соединение с базой даных
                    if (!ClientSock) {
                        Err = "Ошибка записи в базу данных";
                        break;
                    }
                    Transmit(ClientSock, s);
                    Close(ClientSock);
                }
                else {
                    // yandex



                }

            } while (false);
        }
   // SendToTCP(client_socket, buf1);


    }



    CloseClient(client_socket);

}


int main() {
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
