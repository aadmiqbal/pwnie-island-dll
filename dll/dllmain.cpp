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
// Placeholder for the runtime base address of "GameLogic.dll"
uintptr_t runtimeBaseAddress = GetModuleBaseAddress(procId, L"GameLogic.dll");

uintptr_t offset = 0x551a0; // Correct offset for Player::Chat

// Calculate the actual runtime address of the Chat function
uintptr_t chatFuncAddr = runtimeBaseAddress + offset;

typedef void(__thiscall* OriginalChatFunc)(void* thisPlayer, const char* text);
OriginalChatFunc originalChat = nullptr;

// Declare a pointer-to-member function type
typedef void(__thiscall* ChatFuncType)(const char* text);

// Custom function to intercept and modify the Chat function behavior
void __fastcall MyCustomChat(void* thisPlayer, ChatFuncType func, const char* originalText) {
	printf(originalText);

	// Compare originalText to the string "adam" using strcmp
	if (strcmp(originalText, "spaceinvaders") == 0) {
		printf("Space Invaders started");
		HookCanJump();
	}
}


void HookChatFunction() {
	std::cout << "HookChatFunction: Starting." << std::endl;

	// Calculate the actual runtime address of the Chat function.
	uintptr_t actualFunctionAddress = runtimeBaseAddress + offset;
	std::cout << "HookChatFunction: actualFunctionAddress calculated as " << std::hex << actualFunctionAddress << std::endl;

	// Change memory protection to execute-read-write.
	DWORD oldProtect;
	VirtualProtect((LPVOID)actualFunctionAddress, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
	std::cout << "HookChatFunction: Memory protection changed to PAGE_EXECUTE_READWRITE." << std::endl;

	// Calculate the relative jump distance from the Chat function to your custom function.
	uintptr_t relativeAddress = ((uintptr_t)&MyCustomChat - actualFunctionAddress - 5);

	// Write the jump instruction and the relative address to the Chat function.
	*(uint8_t*)actualFunctionAddress = 0xE9; // JMP opcode
	*(uintptr_t*)(actualFunctionAddress + 1) = relativeAddress;
	std::cout << "HookChatFunction: JMP instruction written." << std::endl;

	// Restore the original memory protection.
	VirtualProtect((LPVOID)actualFunctionAddress, 5, oldProtect, &oldProtect);
	std::cout << "HookChatFunction: Original memory protection restored." << std::endl;

	// Save the original Chat function address for calling it later.
	originalChat = (OriginalChatFunc)(actualFunctionAddress + 5);
	std::cout << "HookChatFunction: Original Chat function address saved." << std::endl;
}


void InitializeHooks() {
	HookChatFunction();
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

	//Mana hack start
	uintptr_t PwnAventAddr = (uintptr_t)GetModuleHandle(L"PwnAdventure3-Win32-Shipping.exe");
	printf("PwnAventAddr: %p\n", PwnAventAddr);

	// Offset for the mana pointer
	uintptr_t manaPointerOffset = 0x18FCD60;

	// Adding offsets one by one
	uintptr_t firstPointer = *(uintptr_t*)(PwnAventAddr + manaPointerOffset);
	printf("PwnAventAddr + 0x18FCD60 = %p has value %p\n", PwnAventAddr + manaPointerOffset, firstPointer);

	uintptr_t secondPointer = *(uintptr_t*)(firstPointer + 0x20);
	printf("firstPointer + 0x20 = %p has value %p\n", firstPointer + 0x20, secondPointer);

	uintptr_t thirdPointer = *(uintptr_t*)(secondPointer + 0x284);
	printf("secondPointer + 0x284 = %p has value %p\n", secondPointer + 0x284, thirdPointer);

	uintptr_t fourthPointer = *(uintptr_t*)(thirdPointer + 0x3E0);
	printf("thirdPointer + 0x3E0 = %p has value %p\n", thirdPointer + 0x3E0, fourthPointer);

	int* manaValue = (int*)(fourthPointer + 0xBC);
	printf("fourthPointer + 0xBC = %p has value %d\n", fourthPointer + 0xBC, *manaValue);

	std::cout << "\nMana: " << *manaValue << "\n\n";

	//Z coordinate hack - Tom
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
			//std::cout << " Z co-ord: " << z_coord << std::endl;
		}

		// If the player presses 'F' then add 10000 to the players heigth
		// I.e. this makes 'F' a super jump key that will let me jump through solid objects 
		if (GetAsyncKeyState('F') & 1) {
			std::cout << "   F key pressed";

			int mana = *(int*)LocateDirectMemoryAddress(moduleBase + 0x00097D7C, { 0x1C, 0x6c, 0xBC });
			printf("\n mana value is %d \n", mana);

			float walkspeed = *(float*)LocateDirectMemoryAddress(moduleBase + 0x00097D7C, { 0x1C, 0x6c, (0xbc + 0x64) });
			printf("\n walkspeed value is %f \n", walkspeed);

			float jumpspeed = *(float*)LocateDirectMemoryAddress(moduleBase + 0x00097D7C, { 0x1C, 0x6c, (0xbc + 0x68) });
			printf("\n jumpspeed value is %f \n", jumpspeed);

			float jumpholdtime = *(float*)LocateDirectMemoryAddress(moduleBase + 0x00097D7C, { 0x1C, 0x6c, (0xbc + 0x68 + 0x4) });
			printf("\n jumpholdtime value is %f \n", jumpholdtime);

			float xcoord = *(float*)LocateDirectMemoryAddress(moduleBase + 0x00097E1C, { 0x24, 0xc, 0xf8, 0x18, 0x2fc, 0x280, 0x90 });
			printf("\n X-coord value is %f \n", xcoord);

			float ycoord = *(float*)LocateDirectMemoryAddress(moduleBase + 0x00097E1C, { 0x24, 0xc, 0xf8, 0x18, 0x2fc, 0x280, 0x94 });
			printf("\n Y-coord value is %f \n", ycoord);

			float zcoord = *(float*)LocateDirectMemoryAddress(moduleBase + 0x00097E1C, { 0x24, 0xc, 0xf8, 0x18, 0x2fc, 0x280, 0x98 });
			printf("\n Z-coord value is %f \n", zcoord);

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
