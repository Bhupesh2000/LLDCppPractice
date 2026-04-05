## std::atomic (C++11)
`std::atomic` makes a variable thread-safe without using a mutex. Operations like `++`, `--`, `load`, `store` happen atomically (no race condition).

Normal increment is not thread-safe:
counter++;   // Not safe in multithreading
It performs: 1) Read 2) Modify 3) Write. Multiple threads → incorrect result.

#include <iostream>
#include <thread>
#include <atomic>
std::atomic<int> counter(0);
void increment(){for(int i=0;i<1000;i++)counter++;}
int main(){
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();
    std::cout<<counter<<std::endl; // Always 2000
}

Use Case - ID generators, counters, flags. For complex shared data use `std::mutex`.

