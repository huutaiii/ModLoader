// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include <ModUtils.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

//#pragma comment (lib, "dinput8.lib")

extern "C"
{
    __declspec(dllexport) void hk_DirectInput8Create();
    __declspec(dllexport) void hk_DllCanUnloadNow();
    __declspec(dllexport) void hk_DllGetClassObject();
    __declspec(dllexport) void hk_DllRegisterServer();
    __declspec(dllexport) void hk_DllUnregisterServer();
    __declspec(dllexport) void hk_GetdfDIJoystick();

    FARPROC Addresses[6];
}

// trim from start (in place)
static inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string& s) {
    rtrim(s);
    ltrim(s);
}

bool IsValid(HANDLE h) { return h != NULL && h != INVALID_HANDLE_VALUE; }

void InitHooks()
{
    std::string dllpath = GetWinAPIString(GetSystemDirectoryA) + "\\dinput8.dll";
    ULog::Get().println("Original library path: %s", dllpath.c_str());
    HMODULE target = LoadLibraryA(dllpath.c_str());

    ULog::Get().println("Loading original library: %p %d", target, GetLastError());

    if (target && target != INVALID_HANDLE_VALUE)
    {
        Addresses[0] = GetProcAddress(target, "DirectInput8Create");
        Addresses[1] = GetProcAddress(target, "DllCanUnloadNow");
        Addresses[2] = GetProcAddress(target, "DllGetClassObject");
        Addresses[3] = GetProcAddress(target, "DllRegisterServer");
        Addresses[4] = GetProcAddress(target, "DllUnregisterServer");
        Addresses[5] = GetProcAddress(target, "GetdfDIJoystick");
    }
}

std::vector<HMODULE> Modules;

void LoadMods()
{
    std::ifstream list("mod_loader_list.txt");
    for (std::string line; std::getline(list, line); )
    {
        trim(line);
        if (!line.empty() && line[0] != '#')
        {
            HMODULE hMod = LoadLibraryA(line.c_str());
            ULog::Get().println("%s module: %s", IsValid(hMod) ? "Loaded" : "Couldn't load", line.c_str());

            if (IsValid(hMod))
            {
                Modules.push_back(hMod);
            }
        }
    }
}

DWORD WINAPI MainThread(LPVOID lpParam)
{
    Sleep(5000);
    LoadMods();
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ULog::FileName = "mod_loader.log";

        InitHooks();
        CreateThread(0, 0, &MainThread, 0, 0, 0);
        break;
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}

