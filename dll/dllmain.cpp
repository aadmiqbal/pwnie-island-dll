// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream> 
#include <tlhelp32.h>
#include <vector>

DWORD procId = GetCurrentProcessId();

// PatchByte and LocateDirectMemoryAddress Components modified from https://guidedhacking.com/threads/how-to-hack-any-game-tutorial-c-trainer-1-external.10897/

// Set Permissions and Copy Bytes into Memory
void PatchByte(BYTE* dst, BYTE* src, unsigned int size) {
	DWORD oldProtect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldProtect, &oldProtect);
}
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

uintptr_t moduleBase = GetModuleBaseAddress(procId, L"GameLogic.dll");




////////////////////////////////////////////////////////////////canjump///////////////////////
typedef bool(__thiscall* OriginalCanJump)(void* thisPtr);
OriginalCanJump trampolineCanJump = nullptr;

//custom CanJump function
bool __fastcall MyCanJump(void* thisPtr) {
	printf("working");
	return 1;
}

void HookCanJump() {
	HMODULE gameLogicModule = GetModuleHandle(L"GameLogic.dll");
	uintptr_t canJumpAddr = (uintptr_t)gameLogicModule + 0x51680;

	// Change memory protection
	DWORD oldProtect;
	VirtualProtect((LPVOID)canJumpAddr, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

	// Create a trampoline
	trampolineCanJump = (OriginalCanJump)(canJumpAddr + 5);

	// Write JMP to custom function
	*(uint8_t*)canJumpAddr = 0xE9; // JMP opcode
	*(uintptr_t*)(canJumpAddr + 1) = (uintptr_t)&MyCanJump - canJumpAddr - 5;

	// Restore original memory protection
	VirtualProtect((LPVOID)canJumpAddr, 5, oldProtect, &oldProtect);
}

void InitializeHooks() {
	HookCanJump();
	// Any other initialization code can go here
}
////////////////////////////////////////////////////////////////canjump///////////////////////





/* idk what im doing

typedef struct {
	float x;
	float y;
	float z;
} Vector;

void ConstructVector3(Vector* vec, float x, float y, float z) {

	uintptr_t vec3 = (uintptr_t)moduleBase + 0x00014A0;

    __asm {
        mov ecx, vec 
        movss xmm0, x
        movss xmm1, y
        movss xmm2, z 
        call vec3
    }
}
void spawn_actor_with_id(char* actor, unsigned int id, Vector* pos, Vector* rotation) {


	uintptr_t fn_spawn_actor_with_id = (uintptr_t)moduleBase + 0x636c0;
	uintptr_t world = (uintptr_t)moduleBase + 0x00097d7c;

	char* game_world = (char*)world;

	__asm {
		mov eax, rotation
		push eax
		mov eax, pos
		push eax
		mov eax, actor
		push eax
		mov eax, id
		push eax
		mov ecx, game_world
		mov eax, fn_spawn_actor_with_id
		call eax
	}
}

*/

// This method defines a threat that will run concurrently with the game
DWORD WINAPI MyThread(HMODULE hModule)
{
	// The following 3 lines enable a writable console
	// We don't actually need a console here, but it is very useful to print debugging information to. 
	AllocConsole();
	FILE* f = new FILE;
	freopen_s(&f, "CONOUT$", "w", stdout);

	std::cout << "Injection worked\n";
	std::cout << "Process ID is: " << GetCurrentProcessId() << std::endl;
	// We can see by looking at the process ID in process explorer that this code is being run by the process it was injected into. 


	// From cheat engine analysis we know that
	// z_coord is at memory address: [[[["PwnAdventure3-Win32-Shipping.exe"+018FCD60] + 20 ] + 238 ] + 280 ] + 98 
	// This code follows that pointer path
	//
	// NB this may lead trying to dereference null pointers or reading memory you don't have access to.
	// In other module you would be expected to catch and handle these possible errors 
	uintptr_t PwnAventAddr = (uintptr_t)GetModuleHandle(L"PwnAdventure3-Win32-Shipping.exe");
	printf("PwnAventAddr: %p\n", PwnAventAddr);
	uintptr_t firstStep = *(uintptr_t*)(PwnAventAddr + 0x18FCD60);
	printf("PwnAventAddr + 0x18FCD60 = %p has value %p\n", PwnAventAddr + 0x18FCD60, firstStep);
	uintptr_t secondStep = *(uintptr_t*)(firstStep + 0x20);
	printf("firstStep + 0x20 = %p has value %p\n", firstStep + 0x20, secondStep);
	uintptr_t thirdStep = *(uintptr_t*)(secondStep + 0x238);
	printf("secondStep + 0x238 = %p has value %p\n", secondStep + 0x238, thirdStep);
	uintptr_t forthStep = *(uintptr_t*)(thirdStep + 0x280);
	printf("thirdStep + 0x280 = %p has value %p\n", thirdStep + 0x280, forthStep);

	float* z_coord_Address = (float*)(forthStep + 0x98);
	float z_coord = *z_coord_Address;

	// This is the main loop that will run in the background while I play the game
	while (true) {
		// If the player z coordinate (height) changes then print it.
		if (z_coord != *z_coord_Address) {
			z_coord = *z_coord_Address;
			std::cout << " Z co-ord: " << z_coord << std::endl;
		}

		// If the player presses 'F' then add 10000 to the players heigth
		// I.e. this makes 'F' a super jump key that will let me jump through solid objects 
		if (GetAsyncKeyState('F') & 1) {
			std::cout << "   F key pressed";				
		/*	float zAxisValue = *(float*)LocateDirectMemoryAddress(moduleBase + 0x00097D7C, {0x1C, 0x4, 0x4c, 0x38, 0x8, 0x54, 0x98});
			float yAxisValue = *(float*)LocateDirectMemoryAddress(moduleBase + 0x00097D7C, { 0x1C, 0x4, 0x4c, 0x38, 0x8, 0x54, 0x94 });
			float xAxisValue = *(float*)LocateDirectMemoryAddress(moduleBase + 0x00097D7C, { 0x1C, 0x4, 0x4c, 0x38, 0x8, 0x54, 0x90 });
			printf("%f",zAxisValue);
			printf("%f", yAxisValue);
			printf("%f", xAxisValue);


			Vector vector = {xAxisValue,yAxisValue,zAxisValue};
			Vector vector2 = {0.0f, 0.0f, 0.0f};
			uintptr_t bear1 = *(uintptr_t*)(moduleBase + 0x0006011);

			char* bear = (char*)(bear1);
		

			spawn_actor_with_id(bear,99,&vector, &vector2);

			*/
			*z_coord_Address = *z_coord_Address + 10000;
		}


	}
	return 0;
}




// This is the main method that runs when the DLL is injected.
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DisableThreadLibraryCalls(hModule);
		InitializeHooks();
		// We run the cheat code in a seperate thread to stop it interupting the game execution. 
		// Again we dont catch a possible NULL, if we are going down then we can go down in flames. 
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MyThread, hModule, 0, nullptr));
		break; 
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

