#pragma once

#include <chrono>
#include <print>

class Clock{
private:
    std::chrono::high_resolution_clock::time_point start{};
    std::chrono::high_resolution_clock::time_point end{};
public:
    void start_clock(){
        start = std::chrono::high_resolution_clock::now();
    }
    void end_clock(){
        end = std::chrono::high_resolution_clock::now();
    }
    void print_time(){
       std::println("{}", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<float>(end - start)) );
    }
};

