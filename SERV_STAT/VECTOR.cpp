#include "VECTOR.h"


//std::vector <std::string> URLlist;
//std::vector <std::string> IPlist;
//std::vector <std::string> Timelist;



void LinkStatisticsService::addParentRecord(const std::string& url, const std::string& sourceIP, const std::string& timeInterval) {
        int id = records.size() + 1; // Генерируем id, начиная с 1
        records.emplace_back(id, -1, url, sourceIP, timeInterval, 1);
    }

void LinkStatisticsService::addChildRecord(const std::string& url, const std::string& sourceIP, const std::string& timeInterval, int pid) {
        int id = records.size() + 1; // Генерируем id, начиная с 1
        int count = 1; // Новая запись всегда считается как 1
        records.emplace_back(id, pid, url, sourceIP, timeInterval, count);
    }

std::stringstream LinkStatisticsService::printReport() {
        std::stringstream response; // сюда будет записываться ответ клиенту
        response << "[" << "\n";
        for (size_t i = 0; i < records.size(); ++i) {
            response << "  {" << std::endl;
            response << "    \"Id\": " << records[i].Id << "," << "\n";
            if (records[i].Pid == 0) {
                response << "    \"Pid\": null," << std::endl;
            }
            else {
                response << "    \"Pid\": " << records[i].Pid << "," << std::endl;
            }
            response << "    \"URL\": " << (records[i].URL.empty() ? "null" : "\"" + records[i].URL + "\"") << "," << std::endl;
            response << "    \"SourceIP\": " << (records[i].SourceIP.empty() ? "null" : "\"" + records[i].SourceIP + "\"") << "," << std::endl;
            response << "    \"TimeInterval\": " << (records[i].TimeInterval.empty() ? "null" : "\"" + records[i].TimeInterval + "\"") << "," << std::endl;
            response << "    \"Count\": " << records[i].Count << std::endl;
            response << "  }" << (i < records.size() - 1 ? "," : "") << std::endl;
        }
        response << "]" << std::endl;
        return response;
    }

int LinkStatisticsService::Find2(std::string s, std::string url, std::string clientIP, std::string tim) {
    int flag = 0;
    for (size_t i = 0; i < LinkStatisticsService::records.size(); ++i) {
        if (clientIP == records[i].SourceIP) {
            flag = 1;
            if (tim == records[i].TimeInterval) {
                flag = 2;
                if (url == records[i].URL) {
                    records[i].Count++;
                    flag = 3;
                    break;
                }
            }
        }
    }
    if (!flag) {
        addParentRecord(url, clientIP, tim);// родитель
    }
    else if (flag != 3) {
        addChildRecord(url, clientIP, tim, 1);// дочь
    }
    return 0;
}



void LinkStatisticsService::Statist(std::string s, std::string url, std::string clientIP, std::string tim) {
    int flag = 0; int pid = 1;
    for (size_t i = 0; i < LinkStatisticsService::records.size(); ++i) {
        if (clientIP == records[i].SourceIP) {
            flag = 1;
            pid = records[i].Id;
            if (tim == records[i].TimeInterval) {
                flag = 2;
                pid = records[i].Id;
                if (url == records[i].URL) {
                    records[i].Count++;
                    flag = 3;
                    break;
                }
            }
        }
    }
    if (!flag) {
        addParentRecord(url, clientIP, tim);// родитель
    }
    else if (flag != 3) {
        addChildRecord(url, clientIP, tim, pid);// дочь
    }
}

std::stringstream  LinkStatisticsService::main2() {

    //// Выводим сгенерированный отчет в формате JSON
    return printReport();

    //return 0;
}











