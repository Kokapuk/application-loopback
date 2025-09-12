#pragma once

#include <windows.h>
#include <string>

std::string WideToUtf8(const std::wstring &wide);

struct AppInfo
{
    DWORD processId;
    std::string processName;
    std::string windowTitle;
    HWND windowHandle;
    bool isVisible;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

Napi::Value GetVisibleWindows(const Napi::CallbackInfo &info);