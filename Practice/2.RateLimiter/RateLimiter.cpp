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

// bool RateLimiter::allowRequest(int clientId){
//     std::lock_guard<std::mutex> guard(mapMtx); // this will lock the map for long time
//     if(clientsStateMap.count(clientId) == 0) clientsStateMap.try_emplace(clientId);
//     ClientState& client = clientsStateMap[clientId];
//     {
//         std::lock_guard<std::mutex> guard(client.clientMtx);
//         auto elapsed = std::chrono::steady_clock::now() - client.windowStartTime;
//         if(std::chrono::duration_cast<std::chrono::seconds> (elapsed) >= windowSize){
//             client.windowStartTime = std::chrono::steady_clock::now();
//             client.requestsCounter = 1;
//             return true;
//         } else{
//             return ++ client.requestsCounter <= maxRequestsPerWindow;
//         }
//     }
// }

bool RateLimiter::allowRequest(int clientId){
    ClientState* client;
    {
        std::lock_guard<std::mutex> guard(mapMtx);
        auto[it, inserted] = clientsStateMap.try_emplace(clientId, ClientState{std::chrono::steady_clock::now(), 0});
        client = &it -> second;
    }

    std::lock_guard<std::mutex> guard(client -> clientMtx);
    auto now = std::chrono::steady_clock::now();
    auto elapsed = now - client -> windowStartTime;
    if(elapsed >= windowSize){ // request at the boundary to be rejected
        client -> windowStartTime = now;
        client -> requestsCounter = 0;
    }

    return ++ client -> requestsCounter <= maxRequestsPerWindow;
}

int main() {

    

    return 0;
}