#include <windows.h>
#include <psapi.h>
#include <string>
#include <vector>
#include <napi.h>

#include "window-list.h"

#pragma comment(lib, "psapi.lib")

std::string WideToUtf8(const std::wstring &wide)
{
    if (wide.empty())
        return std::string();

    int size = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string result(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, &result[0], size, nullptr, nullptr);
    return result;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    std::vector<AppInfo> *apps = reinterpret_cast<std::vector<AppInfo> *>(lParam);

    wchar_t windowTitle[256];
    GetWindowTextW(hwnd, windowTitle, sizeof(windowTitle) / sizeof(wchar_t));

    if (wcslen(windowTitle) == 0)
    {
        return TRUE;
    }

    std::string windowTitleUtf8 = WideToUtf8(windowTitle);

    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess == NULL)
    {
        return TRUE;
    }

    wchar_t processName[MAX_PATH];
    DWORD size = sizeof(processName) / sizeof(wchar_t);
    if (!QueryFullProcessImageNameW(hProcess, 0, processName, &size))
    {
        CloseHandle(hProcess);
        return TRUE;
    }

    std::wstring fullPath(processName);
    size_t lastSlash = fullPath.find_last_of(L"\\");
    std::wstring fileName = (lastSlash != std::wstring::npos) ? fullPath.substr(lastSlash + 1) : fullPath;
    std::string fileNameUtf8 = WideToUtf8(fileName);

    bool isVisible = IsWindowVisible(hwnd);

    AppInfo app;
    app.processId = processId;
    app.processName = fileNameUtf8;
    app.windowTitle = windowTitleUtf8;
    app.windowHandle = hwnd;
    app.isVisible = isVisible;

    apps->push_back(app);

    CloseHandle(hProcess);
    return TRUE;
}

Napi::Value GetVisibleWindows(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    std::vector<AppInfo> apps;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&apps));

    std::vector<AppInfo> filteredApps;
    for (const auto &app : apps)
    {
        if (!app.windowTitle.empty() && app.isVisible)
        {
            filteredApps.push_back(app);
        }
    }

    Napi::Array result = Napi::Array::New(env, filteredApps.size());
    for (size_t i = 0; i < filteredApps.size(); i++)
    {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("processId", Napi::Number::New(env, filteredApps[i].processId));
        obj.Set("processName", Napi::String::New(env, filteredApps[i].processName));
        obj.Set("windowTitle", Napi::String::New(env, filteredApps[i].windowTitle));
        result[i] = obj;
    }

    return result;
}