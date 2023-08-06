// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include <ModUtils.h>

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

struct UConfig
{
    DWORD delayMilisec = 0;
} Config;

void InitHooks()
{
    std::string dllpath = "dinput8.original.dll";
    HMODULE target = LoadLibraryA(dllpath.c_str());

    if (target)
    {
        ULog::Get().println("Found \"%s\". Redirecting calls to this DLL", dllpath.c_str());
    }
    else
    {
        dllpath = GetWinAPIString(GetSystemDirectoryA) + "\\dinput8.dll";
        ULog::Get().println("Original library path: %s", dllpath.c_str());
        target = LoadLibraryA(dllpath.c_str());
    }

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

// (unused) keeps tracks of child dlls
std::vector<HMODULE> Modules;

std::vector<std::string> EarlyModFileNames;
std::vector<std::string> ModFileNames;
void ReadModList()
{
    std::ifstream list("mod_loader_list.txt");
    for (std::string line; std::getline(list, line); )
    {
        trim(line);
        if (!line.empty() && line[0] != '#')
        {
            //bool earlyLoad = line[0] == '*';
            bool earlyLoad = false;
            std::string modFileName = earlyLoad ? line.substr(1) : line;
            (earlyLoad ? EarlyModFileNames : ModFileNames).push_back(modFileName);
        }
    }
}

void LoadMod(std::string relPath)
{
    HMODULE hMod = LoadLibraryA(relPath.c_str());
    ULog::Get().println("%s module: %s", IsValid(hMod) ? "Loaded" : "Couldn't load", relPath.c_str());

    if (IsValid(hMod))
    {
        Modules.push_back(hMod);
    }
}

DWORD WINAPI MainThread(LPVOID lpParam)
{
    ReadModList();

    for (std::string mod : EarlyModFileNames)
    {
        LoadMod(mod);
    }

    if (ModFileNames.size())
    {
        //ULog::Get().println("Delaying for %d miliseconds", Config.delayMilisec);
        Sleep(Config.delayMilisec);

        for (std::string mod : ModFileNames)
        {
            LoadMod(mod);
        }
    }

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    ULog::FileName = "mod_loader.log";
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ULog::Get().println("Initializing ModLoader");
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

