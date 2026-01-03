// The Scenario:
// You are building a system for an E-commerce site. Multiple threads (representing different servers) are constantly:
// Incrementing the "Total Sales" count.
// Adding "Error Logs" to a central list.
// Reading the "System Status" report.

// Requirements:
// Singleton: The MetricManager must be a Singleton.
// Thread-Safe Counters: Use a std::mutex to protect an int totalSales.
// Thread-Safe List: Use a std::shared_mutex (Read-Write Lock) to protect a std::vector<std::string> errorLogs.
// Multiple threads should be able to read the logs at the same time.
// Only one thread can add a log at a time.

// The Test: In your main(), create two threads that both try to update the metrics simultaneously.

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <thread>

class MetricManager{
    int totalSales = 0;
    std::vector<std::string> errorLogs;

    mutable std::mutex salesMtx;
    mutable std::shared_mutex rwLogsMtx;

    MetricManager(){
        std::cout << "Loading metric manager\n";
    }

public:
    MetricManager(const MetricManager&) = delete;
    void operator=(const MetricManager&) = delete;

    static MetricManager& getInstance(){
        static MetricManager instance;
        return instance;
    }

    int getTotalSales() const{
        std::lock_guard<std::mutex> lock(salesMtx);
        return totalSales;
    }

    void getErrorLog() const{
        std::shared_lock lock(rwLogsMtx);
        for(const auto& log : errorLogs){
            std::cout << log << std::endl;
        }
    }

    void incrementSales(){
        std::lock_guard<std::mutex> lock(salesMtx);
        totalSales ++;
    }

    void addErrorLog(const std::string& log){
        std::unique_lock lock(rwLogsMtx);
        errorLogs.push_back(log);
    }

    void readSystemReport() const{
        std::cout << "Total Sales: " << getTotalSales() << std::endl;
        std::cout << "Error logs:\n";
        getErrorLog();
        return;
    }

};

void task(int threadNo){
    MetricManager& mtrcMgr = MetricManager::getInstance();
    std::string str1 = std::to_string(threadNo) + ": error 1";
    std::string str2 = std::to_string(threadNo) + ": error 2";
    mtrcMgr.incrementSales();
    mtrcMgr.incrementSales();
    mtrcMgr.addErrorLog(str1);
    mtrcMgr.addErrorLog(str1);
    std::cout << threadNo << ": ";
    mtrcMgr.readSystemReport();
}

int main() {

    std::thread t1(task, 1);
    std::thread t2(task, 2);


    t1.join();
    t2.join();
    

    return 0;
}