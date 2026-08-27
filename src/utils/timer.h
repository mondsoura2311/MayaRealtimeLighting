#pragma once

#include <chrono>
#include <string>
#include <iostream>

class Timer
{
public:
    Timer(const std::string& name = "Timer")
        : m_name(name), m_start(std::chrono::high_resolution_clock::now())
    {
    }
    
    ~Timer()
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start);
        std::cout << m_name << ": " << duration.count() << "ms" << std::endl;
    }
    
    double elapsed() const
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start);
        return duration.count();
    }
    
    void reset()
    {
        m_start = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::string m_name;
    std::chrono::high_resolution_clock::time_point m_start;
};
