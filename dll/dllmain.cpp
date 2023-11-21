// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream> 
#include <tlhelp32.h>
#include <vector>
#include <string>

DWORD procId = GetCurrentProcessId();

std::string getLastChar(std::string s, std::string delimiter) {
	size_t pos = 0;
	std::string token;

	while ((pos = s.find(delimiter)) != (std::string::npos))
	{
		token = s.substr(0, pos);
		s.erase(0, pos + delimiter.length());
	}

	return s;
}

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

// Placeholder for the runtime base address of "GameLogic.dll" and "PwnAdventure3.exe"
uintptr_t runtimeBaseAddress = GetModuleBaseAddress(procId, L"GameLogic.dll");
uintptr_t PwnAdventAddr = (uintptr_t)GetModuleHandle(L"PwnAdventure3-Win32-Shipping.exe");

typedef bool(__thiscall* OriginalCanJump)(void* thisPtr);
OriginalCanJump trampolineCanJump = nullptr;

//custom CanJump function
bool __fastcall MyCanJump(void* thisPtr) {
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



void setMana(int newManaValue) {
	std::cout << "PwnAventAddr: " << PwnAdventAddr << "\n";

	// Offset for the mana pointer
	uintptr_t manaPointerOffset = 0x18FCD60;

	// Adding offsets one by one
	uintptr_t firstPointer = *(uintptr_t*)(PwnAdventAddr + manaPointerOffset);
	std::cout << "PwnAventAddr + 0x18FCD60 = " << PwnAdventAddr + manaPointerOffset << " has value " << firstPointer << "\n";

	uintptr_t secondPointer = *(uintptr_t*)(firstPointer + 0x20);
	std::cout << "firstPointer + 0x20 = " << firstPointer + 0x20 << " has value " << secondPointer << "\n";

	uintptr_t thirdPointer = *(uintptr_t*)(secondPointer + 0x284);
	std::cout << "secondPointer + 0x284 = " << secondPointer + 0x284 << " has value " << thirdPointer << "\n";

	uintptr_t fourthPointer = *(uintptr_t*)(thirdPointer + 0x3E0);
	std::cout << "thirdPointer + 0x3E0 = " << thirdPointer + 0x3E0 << " has value " << fourthPointer << "\n";

	int *manaValue = (int*)(fourthPointer + 0xBC);
	std::cout << "fourthPointer + 0xBC = " << fourthPointer + 0xBC << " has value " << *manaValue << "\n";

	int mana = *manaValue;
	std::cout << "\nCurrent mana value: " << mana;

	*manaValue = newManaValue;
	std::cout << "Mana value set to: " << newManaValue << "\n\n";
}

void setHealth(int newHealthValue) {
	uintptr_t healthOffset = 0x00097D7C;

	uintptr_t firstPointer = *(uintptr_t*)(runtimeBaseAddress + healthOffset);
	uintptr_t secondPointer = *(uintptr_t*)(firstPointer + 0x04);
	uintptr_t thirdPointer = *(uintptr_t*)(secondPointer + 0x00);
	uintptr_t fourthPointer = *(uintptr_t*)(thirdPointer + 0x10);
	uintptr_t fifthPointer = *(uintptr_t*)(fourthPointer + 0x148);
	uintptr_t sixthPointer = *(uintptr_t*)(fifthPointer + 0x6c);

	int* healthValue = (int*)(sixthPointer - 0x40);
	int health = *healthValue;

	std::cout << "\nCurrent health value: " << health;

	*healthValue = newHealthValue;
	std::cout << "Health value set to: " << newHealthValue << "\n\n";
}

void setWalkSpeed(float newWalkSpeed)
{
	uintptr_t walkSpeedOffset = 0x00097D7C;
	
	uintptr_t firstPointer = *(uintptr_t*)(runtimeBaseAddress + walkSpeedOffset);
	uintptr_t secondPointer = *(uintptr_t*)(firstPointer + 0x1C);
	uintptr_t thirdPointer = *(uintptr_t*)(secondPointer + 0x6c);

	float* walkSpeed = (float*)(thirdPointer + (0xbc + 0x64));
	
	std::cout << "\nCurrent walk speed value: " << *walkSpeed;

	*walkSpeed = newWalkSpeed;
	std::cout << "Walk speed value set to: " << newWalkSpeed << "\n\n";
}

void setJumpSpeed(float newJumpSpeed)
{
	float *jumpSpeed = (float*)LocateDirectMemoryAddress(runtimeBaseAddress + 0x00097D7C, { 0x1C, 0x6c, (0xbc + 0x68) });
	std::cout << "\nCurrent jump speed value: " << *jumpSpeed;

	*jumpSpeed = newJumpSpeed;
	std::cout << "Jump speed value set to: " << newJumpSpeed << "\n\n";
}

void setXCoord(float newXCoord) {
	float *xCoord = (float*)LocateDirectMemoryAddress(runtimeBaseAddress + 0x00097E1C, { 0x24, 0xc, 0xf8, 0x18, 0x2fc, 0x280, 0x90 });
	std::cout << "\nCurrent x coord value: " << *xCoord;

	*xCoord = newXCoord;
	std::cout << "X coord set to: " << newXCoord << "\n\n";
}

void setYCoord(float newYCoord) {
	float *yCoord = (float*)LocateDirectMemoryAddress(runtimeBaseAddress + 0x00097E1C, { 0x24, 0xc, 0xf8, 0x18, 0x2fc, 0x280, 0x94 });
	std::cout << "\nCurrent y coord value: " << *yCoord;

	*yCoord = newYCoord;
	std::cout << "Y coord set to: " << newYCoord << "\n\n";
}

void setZCoord(float newZCoord) {
	float *zCoord = (float*)LocateDirectMemoryAddress(runtimeBaseAddress + 0x00097E1C, { 0x24, 0xc, 0xf8, 0x18, 0x2fc, 0x280, 0x98 });
	std::cout << "\nCurrent z coord value: " << *zCoord;

	*zCoord = newZCoord;
	std::cout << "Y coord set to: " << newZCoord << "\n\n";
}

// Original function AddItem
typedef void(__thiscall* OriginalAddItem)(void* thisPlayer, IItem* item, uint quantity, bool someBool);
OriginalAddItem trampolineAddItem = nullptr;

// Correct offset for Player::Chat
uintptr_t offset = 0x551a0;

// Calculate the actual runtime address of the Chat function
uintptr_t chatFuncAddr = runtimeBaseAddress + offset;

typedef void(__thiscall* OriginalChatFunc)(void* thisPlayer, const char* text);
OriginalChatFunc originalChat = nullptr;

// Declare a pointer-to-member function type
typedef void(__thiscall* ChatFuncType)(const char* text);

uintptr_t ResolvePointerChain(uintptr_t baseAddress, const std::vector<unsigned int>& offsets) {
	uintptr_t addr = baseAddress;
	for (auto offset : offsets) {
		addr = *(uintptr_t*)addr;
		addr += offset;
	}
	return addr;
}

//Function AddItem
void __fastcall MyAddItem(void* thisPlayer, void* EDX_unused, IItem* item, uint quantity, bool someBool) {
	// Custom logic for adding an item goes here

	// Resolve the pointer chain to get the dynamic address of the item value
	uintptr_t baseAddress = GetModuleBaseAddress(procId, L"GameLogic.dll");
	std::vector<unsigned int> offsets = { 0xEC, 0x10, 0x0, 0x4, 0x4 };
	uintptr_t itemAddress = ResolvePointerChain(baseAddress + 0x97D7C, offsets); // 0x97D7C is assumed to be the static part of the address from GameLogic.dll

	// Now itemAddress points to the dynamic address of the item we want to manipulate
	// You can read or write to the memory at itemAddress as needed

	// Call the original function if you want to preserve original behavior
	trampolineAddItem(thisPlayer, item, quantity, someBool);

}
void HookAddItemFunction() {
	// First, we resolve the base address of the module where the AddItem function is located.
	uintptr_t baseAddress = GetModuleBaseAddress(procId, L"GameLogic.dll");

	// We resolve the pointer chain to get the address of the AddItem function.
	std::vector<unsigned int> offsets = { /* ... offsets from your pointer scan ... */ };
	uintptr_t addItemAddr = ResolvePointerChain(baseAddress, offsets);

	// Save the original AddItem function address.
	trampolineAddItem = (OriginalAddItem)addItemAddr;

	// Change memory protection to execute-read-write.
	DWORD oldProtect;
	VirtualProtect((LPVOID)addItemAddr, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

	// Calculate the relative jump distance from the original AddItem to MyAddItem.
	intptr_t relativeJumpDistance = (intptr_t)MyAddItem - (intptr_t)addItemAddr - 5;

	// Write the JMP instruction to MyAddItem at the beginning of AddItem.
	*(uint8_t*)addItemAddr = 0xE9; // JMP opcode
	*(intptr_t*)(addItemAddr + 1) = relativeJumpDistance;

	// Restore the original memory protection.
	VirtualProtect((LPVOID)addItemAddr, 5, oldProtect, &oldProtect);
}


// Custom function to intercept and modify the Chat function behavior
void __fastcall MyCustomChat(void* thisPlayer, ChatFuncType func, const char* originalText) {
	std::cout << "Chat input string: " << originalText;

	std::string originalTextStr = originalText;

	// Commands to start a hack
	if (strcmp(originalText, "init spaceInvaders") == 0) {
		std::cout << "Space Invaders started";
		
	}
	else if (strcmp(originalText, "init trampoline") == 0) {
		std::cout << "Trampoline hack started";
		HookCanJump();
	}
	else if (originalTextStr.rfind("set mana", 0) == 0) {
		std::cout << "Mana hack started";
		int newManaValue = stoi(getLastChar(originalTextStr, " "));
		setMana(newManaValue);
	}
	else if (originalTextStr.rfind("set health", 0) == 0) {
		std::cout << "Health hack started";
		int newHealthValue = stoi(getLastChar(originalTextStr, " "));
		setHealth(newHealthValue);
	}
	else if (originalTextStr.rfind("set walkSpeed", 0) == 0) {
		std::cout << "Walk speed hack started";
		float newWalkSpeed = stof(getLastChar(originalTextStr, " "));
		setWalkSpeed(newWalkSpeed);
	}
	else if (originalTextStr.rfind("set jumpSpeed", 0) == 0) {
		std::cout << "Jump speed hack started";
		float newJumpSpeed = stof(getLastChar(originalTextStr, " "));
		setJumpSpeed(newJumpSpeed);
	}
	else if (originalTextStr.rfind("set x", 0) == 0) {
		std::cout << "X coordinate hack started";
		float newXCoord = stof(getLastChar(originalTextStr, " "));
		setXCoord(newXCoord);
	}
	else if (originalTextStr.rfind("set y", 0) == 0) {
		std::cout << "Y coordinate hack started";
		float newYCoord = stof(getLastChar(originalTextStr, " "));
		setYCoord(newYCoord);
	}
	else if (originalTextStr.rfind("set z", 0) == 0) {
		std::cout << "Z coordinate hack started";
		float newZCoord = stof(getLastChar(originalTextStr, " "));
		setZCoord(newZCoord);
	}
	else if (originalTextStr.rfind("gun", 0) == 0) {
		std::cout << "Z coordinate hack started";
		HookAddItemFunction();
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

// This method defines a threat that will run concurrently with the game
DWORD WINAPI MyThread(HMODULE hModule)
{
	//Console setup. 
	AllocConsole();
	FILE* f = new FILE;
	freopen_s(&f, "CONOUT$", "w", stdout);

	std::cout << "Injection worked\n";
	std::cout << "Process ID is: " << GetCurrentProcessId() << std::endl;
	
	HookChatFunction();


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
