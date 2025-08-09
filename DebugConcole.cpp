#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

int main() {
    HWND consoleWindow = GetConsoleWindow();

    std::cout << "Starting, console visible for 1 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    ShowWindow(consoleWindow, SW_MINIMIZE);

    std::string line;
    while (std::getline(std::cin, line)) {
        std::cout << line << std::endl;
    }

    return 0;
}