#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <nlohmann/json.hpp>

class ParentRecord {
public:
    int Id;
    std::string URL;
    std::string SourceIP;
    std::string TimeInterval;

    ParentRecord(int id, const std::string& url, const std::string& sourceIP, const std::string& timeInterval)
        : Id(id), URL(url), SourceIP(sourceIP), TimeInterval(timeInterval) {
    }
};

class ChildRecord : public ParentRecord {
public:
    int Pid;
    int Count;

    ChildRecord(int id, int pid, const std::string& url, const std::string& sourceIP, const std::string& timeInterval, int count)
        : ParentRecord(id, url, sourceIP, timeInterval), Pid(pid), Count(count) {
    }
};

class LinkStatisticsService {
private:
    std::map<std::string, int> urlToIdMap;
    std::vector<ChildRecord> records;
    int Find2(std::string s, std::string url, std::string clientIP, std::string tim);
    void addParentRecord(const std::string& url, const std::string& sourceIP, const std::string& timeInterval);
    void addChildRecord(const std::string& url, const std::string& sourceIP, const std::string& timeInterval, int pid);

public:


    std::stringstream printReport();
    void Statist(std::string s, std::string url, std::string clientIP, std::string tim);
    std::stringstream main2();


};


//std::stringstream statisticsService.printReport();