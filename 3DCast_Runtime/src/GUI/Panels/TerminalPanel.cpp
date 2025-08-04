#include "runtimepch.h"
#include "TerminalPanel.h"

#include <3DCast/Misc/Icon.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <thread>
#include <mutex>
#include <atomic>
#include <cstring>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <csignal>
#include <sys/wait.h>
#endif

void Runtime::GUI::TerminalPanel::StartShell()
{
    if (shellRunning)
        return;

#ifdef _WIN32
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&childStdOutRead, &childStdOutWrite, &sa, 0) ||
        !CreatePipe(&childStdInRead, &childStdInWrite, &sa, 0)) {
        output.push_back("[Error] Failed to create pipes");
        return;
    }

    SetHandleInformation(childStdOutRead, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(childStdInWrite, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.hStdError = childStdOutWrite;
    si.hStdOutput = childStdOutWrite;
    si.hStdInput = childStdInRead;
    si.dwFlags |= STARTF_USESTDHANDLES;

    if (!CreateProcess(NULL, const_cast<LPSTR>("cmd.exe"), NULL, NULL, TRUE,
        CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        output.push_back("[Error] Failed to start shell process");
        CloseHandle(childStdOutRead);
        CloseHandle(childStdOutWrite);
        CloseHandle(childStdInRead);
        CloseHandle(childStdInWrite);
        return;
    }

    shellProcessHandle = pi.hProcess;
    CloseHandle(pi.hThread);
    CloseHandle(childStdOutWrite);
    CloseHandle(childStdInRead);

#else
    int stdinPipe[2], stdoutPipe[2];

    if (pipe(stdinPipe) < 0 || pipe(stdoutPipe) < 0) {
        output.emplace_back("[Error] Failed to create pipes");
        return;
    }

    const pid_t pid = fork();
    if (pid < 0) {
        output.emplace_back("[Error] Fork failed");
        return;
    }

    if (pid == 0) {
        close(stdinPipe[1]);
        close(stdoutPipe[0]);

        dup2(stdinPipe[0], STDIN_FILENO);
        dup2(stdoutPipe[1], STDOUT_FILENO);
        dup2(stdoutPipe[1], STDERR_FILENO);

        close(stdinPipe[0]);
        close(stdoutPipe[1]);

        execlp("bash", "bash", NULL);
        exit(EXIT_FAILURE);
    }

    close(stdinPipe[0]);
    close(stdoutPipe[1]);

    childStdInWrite = stdinPipe[1];
    childStdOutRead = stdoutPipe[0];
    shellPid = pid;

    fcntl(childStdOutRead, F_SETFL, fcntl(childStdOutRead, F_GETFL) | O_NONBLOCK);
#endif

    shellRunning = true;
    outputThread = std::thread(&TerminalPanel::ReadShellOutput, this);
}

void Runtime::GUI::TerminalPanel::StopShell()
{
    if (!shellRunning)
        return;

    shellRunning = false;

#ifdef _WIN32
    if (shellProcessHandle != NULL) {
        TerminateProcess(shellProcessHandle, 0);
        CloseHandle(shellProcessHandle);
        shellProcessHandle = NULL;
    }

    if (childStdOutRead != NULL) {
        CloseHandle(childStdOutRead);
        childStdOutRead = NULL;
    }

    if (childStdInWrite != NULL) {
        CloseHandle(childStdInWrite);
        childStdInWrite = NULL;
    }
#else
    if (shellPid > 0) {
        kill(shellPid, SIGTERM);
        waitpid(shellPid, NULL, 0);
        shellPid = -1;
    }

    if (childStdOutRead >= 0) {
        close(childStdOutRead);
        childStdOutRead = -1;
    }

    if (childStdInWrite >= 0) {
        close(childStdInWrite);
        childStdInWrite = -1;
    }
#endif

    if (outputThread.joinable()) {
        outputThread.join();
    }
}

void Runtime::GUI::TerminalPanel::ReadShellOutput()
{
    constexpr size_t bufferSize = 4096;
    char buffer[bufferSize];

    while (shellRunning) {
#ifdef _WIN32
        DWORD bytesRead = 0;
        BOOL success = ReadFile(childStdOutRead, buffer, bufferSize - 1, &bytesRead, NULL);

        if (success && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::lock_guard<std::mutex> lock(outputMutex);
            ProcessOutput(buffer);
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
#else
        ssize_t bytesRead = read(childStdOutRead, buffer, bufferSize - 1);

        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::lock_guard<std::mutex> lock(outputMutex);
            ProcessOutput(buffer);
        }
        else if (bytesRead == 0) {
            // EOF
            break;
        }
        else if (errno != EAGAIN && errno != EWOULDBLOCK) {
            // Error other than "would block"
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
#endif
    }
}

void Runtime::GUI::TerminalPanel::ProcessOutput(const char* text)
{
    std::string current;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (text[i] == '\n') {
            output.push_back(current);
            current.clear();
            scrollToBottom = true;
        }
        else if (text[i] != '\r') {
            current += text[i];
        }
    }

    if (!current.empty()) {
        output.push_back(current);
        scrollToBottom = true;
    }
}

void Runtime::GUI::TerminalPanel::SendCommandToShell(const char* command)
{
    if (!shellRunning) {
        StartShell();
    }

    std::string cmd = command;
    cmd += "\n";

#ifdef _WIN32
    DWORD bytesWritten;
    WriteFile(childStdInWrite, cmd.c_str(), static_cast<DWORD>(cmd.length()), &bytesWritten, NULL);
#else
    write(childStdInWrite, cmd.c_str(), cmd.length());
#endif

    std::lock_guard<std::mutex> lock(outputMutex);
    output.push_back(std::string("> ") + command);
    scrollToBottom = true;
}

void Runtime::GUI::TerminalPanel::Execute3DCastCommand(const char* command)
{
    // TODO: Implement internal commands here
    if (strncmp(command, "clear", 5) == 0) {
        std::lock_guard<std::mutex> lock(outputMutex);
        output.clear();
        return;
    }
    else if (strncmp(command, "exit", 4) == 0 || strncmp(command, "quit", 4) == 0) {
        StopShell();
        output.emplace_back("[Shell terminated]");
        return;
    }

    // If not an internal command, send to shell
    SendCommandToShell(command);
}

void Runtime::GUI::TerminalPanel::OnImGuiRender()
{
    ImGui::Begin(ICON_FA_TERMINAL " Terminal");

    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
    {
        std::lock_guard<std::mutex> lock(outputMutex);
        for (const auto& line : output) {
            ImGui::TextUnformatted(line.c_str());
        }
        if (scrollToBottom) {
            ImGui::SetScrollHereY(1.0f);
            scrollToBottom = false;
        }
    }
    ImGui::EndChild();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));

    const float buttonWidth = ImGui::GetFrameHeight() + 8.0f;
    const float availWidth = ImGui::GetContentRegionAvail().x - (buttonWidth * 2) - 10.0f;

    if (ImGui::Button(ICON_FA_TRASH_CAN, ImVec2(buttonWidth, 0))) {
        std::lock_guard<std::mutex> lock(outputMutex);
        output.clear();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(availWidth);

    const ImVec2 cursorPos = ImGui::GetCursorPos();

    if (refocusInput) {
        ImGui::SetKeyboardFocusHere();
        refocusInput = false;
    }

    const bool input_submitted = ImGui::InputText("##CommandInput", inputBuffer, IM_ARRAYSIZE(inputBuffer),
        ImGuiInputTextFlags_EnterReturnsTrue);

    if (inputBuffer[0] == '\0' && !ImGui::IsItemActive() && !ImGui::IsItemFocused()) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 0.7f));
        ImGui::RenderText(
            ImVec2(cursorPos.x + ImGui::GetStyle().FramePadding.x,
                   cursorPos.y + ImGui::GetStyle().FramePadding.y),
            "Enter command...");
        ImGui::PopStyleColor();
    }

    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_ANGLE_RIGHT, ImVec2(buttonWidth, 0)) || input_submitted) {
        if (inputBuffer[0] != '\0') {
            Execute3DCastCommand(inputBuffer);
            strcpy(inputBuffer, "");
            refocusInput = true;
        }
    }

    ImGui::PopStyleVar();

    ImGui::End();
}

Runtime::GUI::TerminalPanel::~TerminalPanel()
{
    StopShell();
}
