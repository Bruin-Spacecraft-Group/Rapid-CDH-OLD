#include <chrono>
#include <iostream>
#include <pthread.h>
#include <thread>

void foo() {}

int main() {
    std::cout << "starting up" << std::endl;

    while (true) {



    // sleep for 50 ms
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
