#include <windows.h>
#include <iostream>
#include <string>
#include "Debug.h"

static HANDLE hPipeWrite = NULL;
static PROCESS_INFORMATION piProcInfo;


void Debug::Init()
{
    SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hPipeRead = NULL;

    // Create a pipe for child stdin
    if (!CreatePipe(&hPipeRead, &hPipeWrite, &saAttr, 0)) {
        std::cerr << "Failed to create pipe\n";
        return;
    }

    SetHandleInformation(hPipeWrite, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA siStartInfo = {0};
    siStartInfo.cb = sizeof(siStartInfo);
    siStartInfo.hStdInput = hPipeRead;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Launch your DebugConsole.exe process
    if (!CreateProcessA(
        "DebugConsole.exe", // path to helper console app
        NULL, NULL, NULL, TRUE, CREATE_NEW_CONSOLE,
        NULL, NULL, &siStartInfo, &piProcInfo))
    {
        std::cerr << "Failed to start DebugConsole\n";
        CloseHandle(hPipeRead);
        CloseHandle(hPipeWrite);
        return;
    }
    CloseHandle(hPipeRead);
}

void Debug::Log(const std::string& msg)
{
    if (piProcInfo.hProcess) {
        DWORD exitCode;
        if (GetExitCodeProcess(piProcInfo.hProcess, &exitCode) && exitCode != STILL_ACTIVE) {
            std::cerr << "Debug console process already exited.\n";
            return; // Don't write if the process exited
        }
    }

    if (hPipeWrite) {
        std::string out = msg + "\r\n";
        DWORD written = 0;
        WriteFile(hPipeWrite, out.c_str(), (DWORD)out.size(), &written, NULL);
    }
}