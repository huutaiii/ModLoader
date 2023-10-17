// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include <ModUtils.h>

//#pragma comment (lib, "dinput8.lib")
//#pragma comment (lib, "version.lib")


// Set the VC+ project 'Target Name' to the according file name in all caps
#define TARGET_DINPUT8 0
#define TARGET_VERSION 1
#define TARGET_XINPUT9_1_0 2

#ifndef TARGET_MODULE
    #define TARGET_MODULE TARGET_DINPUT8
#endif

#if TARGET_MODULE == TARGET_DINPUT8
#define TARGET_NAME "dinput8"
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
#endif

#if TARGET_MODULE == TARGET_VERSION
#define TARGET_NAME "version"
extern "C"
{
    void hk_GetFileVersionInfoA();
    void hk_GetFileVersionInfoByHandle();
    void hk_GetFileVersionInfoExA();
    void hk_GetFileVersionInfoExW();
    void hk_GetFileVersionInfoSizeA();
    void hk_GetFileVersionInfoSizeExA();
    void hk_GetFileVersionInfoSizeExW();
    void hk_GetFileVersionInfoSizeW();
    void hk_GetFileVersionInfoW();
    void hk_VerFindFileA();
    void hk_VerFindFileW();
    void hk_VerInstallFileA();
    void hk_VerInstallFileW();
    void hk_VerLanguageNameA();
    void hk_VerLanguageNameW();
    void hk_VerQueryValueA();
    void hk_VerQueryValueW();

    FARPROC Addresses[18];
}
#endif

#if TARGET_MODULE == TARGET_XINPUT9_1_0
extern "C" void hk_DllMain();
#endif

#if TARGET_MODULE == TARGET_XINPUT9_1_0
#define TARGET_NAME "xinput9_1_0"
extern "C"
{
    void hk_XInputGetCapabilities();
    void hk_XInputGetDSoundAudioDeviceGuids();
    void hk_XInputGetState();
    void hk_XInputSetState();

    FARPROC Addresses[5];
}
#endif

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
    HMODULE target;
    std::string dllpath;
    dllpath = TARGET_NAME ".original.dll";
    target = LoadLibraryA(dllpath.c_str());


    if (target)
    {
        ULog::Get().println("Found \"%s\". Redirecting calls to this DLL", dllpath.c_str());
    }
    else
    {
        dllpath = GetWinAPIString(GetSystemDirectoryA) + "\\" TARGET_NAME ".dll";
        ULog::Get().println("Original library path: %s", dllpath.c_str());
        SetLastError(0);
        target = LoadLibraryA(dllpath.c_str());
    }

    ULog::Get().println("Loading original library: %p %d", target, GetLastError());

#if TARGET_MODULE == TARGET_DINPUT8
    if (target && target != INVALID_HANDLE_VALUE)
    {
        Addresses[0] = GetProcAddress(target, "DirectInput8Create");
        Addresses[1] = GetProcAddress(target, "DllCanUnloadNow");
        Addresses[2] = GetProcAddress(target, "DllGetClassObject");
        Addresses[3] = GetProcAddress(target, "DllRegisterServer");
        Addresses[4] = GetProcAddress(target, "DllUnregisterServer");
        Addresses[5] = GetProcAddress(target, "GetdfDIJoystick");
    }
#endif
#if TARGET_MODULE == TARGET_VERSION
    if (target && target != INVALID_HANDLE_VALUE)
    {
        Addresses[ 0] = GetProcAddress(target, "GetFileVersionInfoA");
        Addresses[ 1] = GetProcAddress(target, "GetFileVersionInfoByHandle");
        Addresses[ 2] = GetProcAddress(target, "GetFileVersionInfoExA");
        Addresses[ 3] = GetProcAddress(target, "GetFileVersionInfoExW");
        Addresses[ 4] = GetProcAddress(target, "GetFileVersionInfoSizeA");
        Addresses[ 5] = GetProcAddress(target, "GetFileVersionInfoSizeExA");
        Addresses[ 6] = GetProcAddress(target, "GetFileVersionInfoSizeExW");
        Addresses[ 7] = GetProcAddress(target, "GetFileVersionInfoSizeW");
        Addresses[ 8] = GetProcAddress(target, "GetFileVersionInfoW");
        Addresses[ 9] = GetProcAddress(target, "VerFindFileA");
        Addresses[10] = GetProcAddress(target, "VerFindFileW");
        Addresses[11] = GetProcAddress(target, "VerInstallFileA");
        Addresses[12] = GetProcAddress(target, "VerInstallFileW");
        //Addresses[13] = GetProcAddress(target, "VerLanguageNameA");
        //Addresses[14] = GetProcAddress(target, "VerLanguageNameW");
        //Addresses[13] = reinterpret_cast<FARPROC>(&VerLanguageNameA);
        //Addresses[14] = reinterpret_cast<FARPROC>(&VerLanguageNameW);
        Addresses[15] = GetProcAddress(target, "VerQueryValueA");
        Addresses[16] = GetProcAddress(target, "VerQueryValueW");
    }
#endif
#if TARGET_MODULE == TARGET_XINPUT9_1_0
    if (target && target != INVALID_HANDLE_VALUE)
    {
        Addresses[0] = GetProcAddress(target, "DllMain");
        Addresses[1] = GetProcAddress(target, "XInputGetCapabilities");
        Addresses[2] = GetProcAddress(target, "XInputGetDSoundAudioDeviceGuids");
        Addresses[3] = GetProcAddress(target, "XInputGetState");
        Addresses[4] = GetProcAddress(target, "XInputSetState");
    }
#endif
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

void FindWndProc(HWND hwnd)
{
    WNDPROC proc = (WNDPROC)GetWindowLongPtrA(hwnd, GWLP_WNDPROC);
    ULog::Get().println("wndproc %s %p", GetWinAPIString(GetWindowTextA, hwnd).c_str(), reinterpret_cast<LPVOID>(proc));
}

BOOL CALLBACK EnumWndCallback(HWND hwnd, LPARAM param)
{
    DWORD procID = 0;
    GetWindowThreadProcessId(hwnd, &procID);
    if (procID == GetCurrentProcessId())
    {
        FindWndProc(hwnd);
    }
    return TRUE;
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

#ifdef _DEBUG
    //for (int i = 0; i < 10; ++i)
    //{
    //    Sleep(1000);

    //    EnumWindows(&EnumWndCallback, NULL);
    //}
    //ULog::Get().dprintln("aaaaaaaaaaaaa");
#endif

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

