#pragma once

#include <string>
#include <chrono>

class Profiler
{
public:
    Profiler(const std::string& scope);
    ~Profiler();

private:
    std::string m_scope;
    std::chrono::steady_clock::time_point m_startTime = std::chrono::steady_clock::now();

};