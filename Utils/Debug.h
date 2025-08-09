#pragma once
#include <string>

class Debug {
public:
    static void Init();
    static void Log(const std::string& msg);
};