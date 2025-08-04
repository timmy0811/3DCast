#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Runtime::GUI {

    class TerminalPanel {
    public:
        TerminalPanel() :
            shellRunning(false),
            refocusInput(false),
    #ifdef _WIN32
            shellProcessHandle(NULL),
            childStdInWrite(NULL),
            childStdOutRead(NULL)
    #else
            shellPid(-1),
            childStdInWrite(-1),
            childStdOutRead(-1)
    #endif
        {
            inputBuffer[0] = '\0';
        }

        ~TerminalPanel();
        void OnImGuiRender();

    private:
        char inputBuffer[256]{};
        std::vector<std::string> output;
        bool scrollToBottom = false;
        bool refocusInput = false;

        std::atomic<bool> shellRunning;
        std::thread outputThread;
        std::mutex outputMutex;

    #ifdef _WIN32
        HANDLE shellProcessHandle;
        HANDLE childStdInWrite;
        HANDLE childStdOutRead;
    #else
        pid_t shellPid;
        int childStdInWrite;
        int childStdOutRead;
    #endif

        void StartShell();
        void StopShell();
        void ReadShellOutput();
        void ProcessOutput(const char* text);
        void SendCommandToShell(const char* command);
        void Execute3DCastCommand(const char* command);
    };
}

