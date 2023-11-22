// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream> 
#include <tlhelp32.h>
#include <vector>
#include <string>

// Process ID
DWORD procId = GetCurrentProcessId();

uintptr_t LocateDirectMemoryAddress(uintptr_t ptr, std::vector<unsigned int> offsets) {
    uintptr_t address = ptr;

    for (unsigned int i = 0; i < offsets.size(); ++i) {
        address = *(uintptr_t*)address;
        address += offsets[i];
    }
    return address;
}

// GetModuleBaseAddress solution from: https://stackoverflow.com/questions/39091948/c-get-the-base-address-of-a-dll-of-a-running-process
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

// Runtime base address of "GameLogic.dll" and "PwnAdventure3.exe"
uintptr_t runtimeBaseAddress = GetModuleBaseAddress(procId, L"GameLogic.dll");
uintptr_t PwnAdventAddr = (uintptr_t)GetModuleHandle(L"PwnAdventure3-Win32-Shipping.exe");

uintptr_t getPositionOffset = 0x16F0;

struct Vector3 {
    float x, y, z;
};

struct Bear {
};

struct Actor {
};

typedef Vector3* (__thiscall* OriginalGetPosition)(Bear* thisActor);
OriginalGetPosition originalGetPosition = nullptr;

Vector3* CallGetPosition(Bear* thisActor) {
    std::cout << "\nGetPosition function initiated";

    // Calculate the absolute address of GetPosition.
    uintptr_t getPositionAddress = runtimeBaseAddress + getPositionOffset;
    originalGetPosition = reinterpret_cast<OriginalGetPosition>(getPositionAddress);
    std::cout << "\nOriginal GetPosition function address: " << std::hex << getPositionAddress;

    // Call the original GetPosition function with the thisActor pointer.
    Vector3* position = originalGetPosition(thisActor);
    std::cout << "\nPosition address: " << position;

    // Return the position pointer to be used outside this function
    return position;
}

// This method defines a threat that will run concurrently with the game
DWORD WINAPI MyThread(HMODULE hModule)
{
    //Console setup. 
    AllocConsole();
    FILE* f = new FILE;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "Injection worked\n";
    std::cout << "Process ID is: " << GetCurrentProcessId() << std::endl;

    // Bear object
    uintptr_t bearPointer1 = LocateDirectMemoryAddress(runtimeBaseAddress + 0x00097D7C, { 0x1c, 0x18c, 0x224, 0x0, 0x18, 0x38c, 0x0 });
    std::cout << "\nBear Pointer: " << bearPointer1;
    Bear* bearObj = reinterpret_cast<Bear*>(bearPointer1);

    CallGetPosition(bearObj);

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
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

