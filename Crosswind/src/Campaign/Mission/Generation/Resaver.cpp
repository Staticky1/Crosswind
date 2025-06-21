#include "Resaver.h"
#include <windows.h>
#include <iostream>

bool Resaver::RunResaver(const std::filesystem::path& resaverDir, const std::filesystem::path& missionPath)
{
	
    std::filesystem::path gamePath = Core::Instance().GetGamePath();
    std::filesystem::path dataPath = gamePath / "data";

    // Use generic_wstring to force forward slashes in paths
    std::wstring command = L"/C MissionResaver.exe -d \"" + std::filesystem::path(dataPath).generic_wstring() + L"\" -f \"" + std::filesystem::path(missionPath).generic_wstring() + L"\"";

    SHELLEXECUTEINFOW sei = { 0 };
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.hwnd = nullptr;
    sei.lpVerb = L"open";
    sei.lpFile = L"cmd.exe";
    sei.lpParameters = command.c_str();                    // Run this command in the shell
    std::wstring workingDir = std::filesystem::path(resaverDir).generic_wstring();
    sei.lpDirectory = workingDir.c_str();                  // Set working directory
    sei.nShow = SW_HIDE;                                   // Show console window

    if (!ShellExecuteExW(&sei)) {
        DWORD err = GetLastError();
        std::wcerr << L"ShellExecuteExW failed with error " << err << std::endl;
        return false;
    }

    if (sei.hProcess != nullptr) {
        // Wait until the process completes or timeout in 5mins
        DWORD waitResult = WaitForSingleObject(sei.hProcess, 300000);
        if (waitResult == WAIT_TIMEOUT) {
            std::wcerr << L"Resaver process timed out." << std::endl;
            CloseHandle(sei.hProcess);
            return false;
        }

        DWORD exitCode;
        GetExitCodeProcess(sei.hProcess, &exitCode);
        CloseHandle(sei.hProcess);

        if (exitCode != 0) {
            std::wcerr << L"Resaver process exited with code: " << exitCode << std::endl;
            return false;
        }
    }

    return true;
}

