#include "stdafx.h"
#include "Profiler.h"

#include <iostream>

Profiler::Profiler(const std::string& scope)
{
    m_scope = scope;
    m_startTime = std::chrono::steady_clock::now();
}

Profiler::~Profiler()
{
    const auto duration = std::chrono::steady_clock::now() - m_startTime;
    std::cout << "Scope: " << m_scope
              << " - duration: " << std::chrono::duration_cast<std::chrono::microseconds>(duration).count()
              << " us." << std::endl;
}
