#include <iostream>
#include <string>
#include <unordered_map>
#include <mutex>
#include <chrono>

struct ClientState{
    std::chrono::steady_clock::time_point windowStartTime = std::chrono::steady_clock::now();
    int requestsCounter = 0;
    std::mutex clientMtx;
};

class RateLimiter{
    const std::chrono::seconds windowSize;
    const int maxRequestsPerWindow;
    std::unordered_map<int, ClientState> clientsStateMap;
    std::mutex mapMtx; // mutable not required as allowRequest is not a const function

public:
    RateLimiter(std::chrono::seconds windowSize, int maxRequestsPerWindow) :
        windowSize(windowSize), 
        maxRequestsPerWindow(maxRequestsPerWindow){}

    bool allowRequest(int clientId);
};

int main() {

    

    return 0;
}