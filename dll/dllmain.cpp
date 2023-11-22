// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream> 
#include <tlhelp32.h>
#include <vector>
#include <string>

// Process ID
DWORD procId = GetCurrentProcessId();

// Some global variables
const char* globalModifiedName;
int recentHealthVal = 100;
bool lonelinessModeEnabled = false;

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

// Runtime base address of "GameLogic.dll" and "PwnAdventure3.exe"
uintptr_t runtimeBaseAddress = GetModuleBaseAddress(procId, L"GameLogic.dll");
uintptr_t PwnAdventAddr = (uintptr_t)GetModuleHandle(L"PwnAdventure3-Win32-Shipping.exe");

// Trampoline hack material
typedef bool(__thiscall* OriginalCanJump)(void* thisPtr);
OriginalCanJump trampolineCanJump = nullptr;

//custom CanJump function
bool __fastcall MyCanJump(void* thisPtr) {
	return 1;
}

// Hook custom jump function to the original
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

/*
* Mana hack material
*/
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

	int* manaValue = (int*)(fourthPointer + 0xBC);
	std::cout << "fourthPointer + 0xBC = " << fourthPointer + 0xBC << " has value " << *manaValue << "\n";

	int mana = *manaValue;
	std::cout << "\nCurrent mana value: " << mana;

	// Change mana value
	*manaValue = newManaValue;
	std::cout << "Mana value set to: " << newManaValue << "\n\n";
}

/*
* Health hack material
*/
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
	std::cout << "\nHealth value set to: " << newHealthValue;

	recentHealthVal = newHealthValue;
	std::cout << "\nrecentHealthVal changed to: " << recentHealthVal << "\n";
}

/*
* Walk speed hack material
*/
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

/*
* Jump speed hack material
*/
void setJumpSpeed(float newJumpSpeed)
{
	// Get the memory address of the jump speed pointer
	float* jumpSpeed = (float*)LocateDirectMemoryAddress(runtimeBaseAddress + 0x00097D7C, { 0x1C, 0x6c, (0xbc + 0x68) });
	std::cout << "\nCurrent jump speed value: " << *jumpSpeed;

	*jumpSpeed = newJumpSpeed;
	std::cout << "Jump speed value set to: " << newJumpSpeed << "\n\n";
}

/*
* X Coordinate hack material
*/
void setXCoord(float newXCoord) {
	float* xCoord = (float*)LocateDirectMemoryAddress(runtimeBaseAddress + 0x00097E1C, { 0x24, 0xc, 0xf8, 0x18, 0x2fc, 0x280, 0x90 });
	std::cout << "\nCurrent x coord value: " << *xCoord;

	*xCoord = newXCoord;
	std::cout << "X coord set to: " << newXCoord << "\n\n";
}

/*
* Y Coordinate hack material
*/
void setYCoord(float newYCoord) {
	float* yCoord = (float*)LocateDirectMemoryAddress(runtimeBaseAddress + 0x00097E1C, { 0x24, 0xc, 0xf8, 0x18, 0x2fc, 0x280, 0x94 });
	std::cout << "\nCurrent y coord value: " << *yCoord;

	*yCoord = newYCoord;
	std::cout << "Y coord set to: " << newYCoord << "\n\n";
}

/*
* Z Coordinate hack material
*/
void setZCoord(float newZCoord) {
	float* zCoord = (float*)LocateDirectMemoryAddress(runtimeBaseAddress + 0x00097E1C, { 0x24, 0xc, 0xf8, 0x18, 0x2fc, 0x280, 0x98 });
	std::cout << "\nCurrent z coord value: " << *zCoord;

	*zCoord = newZCoord;
	std::cout << "Y coord set to: " << newZCoord << "\n\n";
}

/* INVENTORY HACKS HERE!! */

/*
* Get gun hack material
*/
uintptr_t addItemOffset = 0X51BA0;

struct IItem {
};

typedef bool(__thiscall* OriginalAddItem)(void* thisPlayer, IItem* item, unsigned int count, bool allowPartial);
OriginalAddItem originalAddItem = nullptr;

bool CallAddItem(void* thisPlayer, unsigned int count, bool allowPartial) {
	std::cout << "\nPistol addItem function initiated";

	// Calculate the absolute address of AddItem.
	uintptr_t addItemAddress = runtimeBaseAddress + addItemOffset;
	originalAddItem = reinterpret_cast<OriginalAddItem>(addItemAddress);
	std::cout << "\nOriginal AddItem function address: " << addItemAddress;

	uintptr_t dynamicItemAddress = LocateDirectMemoryAddress(runtimeBaseAddress + 0x00097D7C, { 0x04, 0x04, 0x0, 0x0, 0x10, 0xec, 0x0 });
	std::cout << "\nPistol item address: " << dynamicItemAddress;

	// Cast the resolved address to an IItem pointer
	IItem* item = reinterpret_cast<IItem*>(dynamicItemAddress);

	// Cast the playerAddress to a void pointer for the call.
	//void* thisPlayer = reinterpret_cast<void*>(thisPlayer);

	// Call the original AddItem function with the parameters.
	return originalAddItem(thisPlayer, item, count, allowPartial);
}

/* 
* Display name hack material 
*/
typedef const char* (__thiscall* OriginalGetDisplayNameFunc)(void* thisGiantRat);
OriginalGetDisplayNameFunc originalGetDisplayName = nullptr;

// Custom function for GetDisplayName
const char* __fastcall MyCustomGetDisplayName(void* thisGiantRat) {
	std::cout << "Custom GetDisplayName logic executed\n";

	// Call the original GetDisplayName method
	const char* originalName = originalGetDisplayName(thisGiantRat);
	std::cout << "Original rat display name: " << originalName << std::endl;

	const char* modifiedName = globalModifiedName;
	std::cout << "Modified display name: " << modifiedName << std::endl;

	return modifiedName;
}

// Function to hook the GetDisplayName method
void HookGetDisplayNameFunction(uintptr_t offset) {
	std::cout << "HookGetDisplayNameFunction: Starting." << std::endl;

	// Calculate the actual runtime address of the GiantRat::GetDisplayName function.
	uintptr_t actualFunctionAddress = runtimeBaseAddress + offset;
	std::cout << "HookGetDisplayNameFunction: actualFunctionAddress calculated as " << std::hex << actualFunctionAddress << std::endl;

	// Change memory protection to execute-read-write.
	DWORD oldProtect;
	VirtualProtect((LPVOID)actualFunctionAddress, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
	if (!VirtualProtect((LPVOID)actualFunctionAddress, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
		std::cerr << "HookGetDisplayNameFunction: Failed to change memory protection." << std::endl;
		return;
	}

	// Write the jump instruction and the relative address to the GetDisplayName function.
	std::cout << "HookGetDisplayNameFunction: Writing jump instruction." << std::endl;
	*(uint8_t*)actualFunctionAddress = 0xE9; // JMP opcode

	// Calculate the relative address
	uintptr_t relativeAddress = ((uintptr_t)&MyCustomGetDisplayName - actualFunctionAddress - 5);
	std::cout << "HookGetDisplayNameFunction: Writing relative address." << std::endl;
	*(uintptr_t*)(actualFunctionAddress + 1) = relativeAddress;

	// Restore the original memory protection.
	VirtualProtect((LPVOID)actualFunctionAddress, 5, oldProtect, &oldProtect);

	// Save the original function pointer
	originalGetDisplayName = (OriginalGetDisplayNameFunc)(actualFunctionAddress + 5);
	std::cout << "HookGetDisplayNameFunction: Original GetDisplayName function address saved." << std::endl;
}

/* 
* Peace mode hack material
*/
void enablePeaceMode() {
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

	*healthValue = 1000000000000000;
	std::cout << "\nHealth value set to: " << 1000000000000000;
}

/* Common structures to be used in Space Invaders and Loneliness mode*/
struct Vector3 {
	float x, y, z;
};

struct Bear {
};

struct Actor {
};

typedef void(__thiscall* OriginalSetPositionFunc)(Bear* thisActor, const Vector3& newPosition);
OriginalSetPositionFunc originalSetPosition = nullptr;

// Offset for Actor::SetPosition function
uintptr_t setPositionOffset = 0x1C80;

// Function to call Actor::SetPosition
void CallSetPosition(Bear* bearObj, float* xCoord, float* yCoord) {
	std::cout << "\SetPosition function initiated";

	// Calculate the absolute address of SetPosition.
	uintptr_t setPositionAddress = runtimeBaseAddress + setPositionOffset;
	originalSetPosition = reinterpret_cast<OriginalSetPositionFunc>(setPositionAddress);
	std::cout << "\nOriginal GetPosition function address: " << std::hex << setPositionAddress;

	//uintptr_t bearpointer2 = LocateDirectMemoryAddress(runtimeBaseAddress + 0x00097D7C, { 0x1c, 0x194, 0x224, 0x0, 0x18, 0x38c });
	//std::cout << "\nBear Pointer 2: " << bearpointer2;
	//Bear* bearObj2 = reinterpret_cast<Bear*>(bearpointer2);

	// Set the custom location
	Vector3 modifidedPostion;
	modifidedPostion.x = *xCoord;
	modifidedPostion.y = *yCoord;
	modifidedPostion.z = 5000.0f;

	// Call the original function with custom arguments
	originalSetPosition(bearObj, modifidedPostion);
	//originalSetPosition(bearObj2, modifidedPostion);
}

// Throws bears up in the sky - used by both loneliness mode and Space Invaders
void pushBears(int limit) {
	if (lonelinessModeEnabled) {
		std::cout << "\nScan across different bears to push them up in the sky";

		int bearCounter = 0;
		int bearLimit = 10;

		if (limit != NULL)
			bearLimit = limit;

		std::cout << "\nBear limit set to: " << bearLimit;

		while (bearCounter <= bearLimit) {
			if (GetAsyncKeyState(VK_ESCAPE)) {
				std::cout << "\nESC key presses. Quiting...\n";
				break;
			}

			uintptr_t bearPointer1 = LocateDirectMemoryAddress(runtimeBaseAddress + 0x00097D7C, { 0x1c, 0x18c, 0x224, 0x0, 0x18, 0x38c, 0x0 });
			std::cout << "\nBear Pointer 1: " << bearPointer1;
			Bear* bearObj = reinterpret_cast<Bear*>(bearPointer1);

			float* xCoord = (float*)LocateDirectMemoryAddress(runtimeBaseAddress + 0x00097E1C, { 0x24, 0xc, 0xf8, 0x18, 0x2fc, 0x280, 0x90 });
			float* yCoord = (float*)LocateDirectMemoryAddress(runtimeBaseAddress + 0x00097E1C, { 0x24, 0xc, 0xf8, 0x18, 0x2fc, 0x280, 0x94 });

			std::cout << "\nCalling setPosition function for bear" << bearCounter;
			CallSetPosition(bearObj, xCoord, yCoord);

			bearCounter++;
		}
	}
	else
		std::cout << "\nLoneliness mode is not enabled.\nCurrent value: " << lonelinessModeEnabled << "\n";
}

/*
* Space Invaders mini-game material
*/
void spaceInvaders() {
	// Push 10 bears in the sky
	lonelinessModeEnabled = true;
	pushBears(10);

	//
}

/*
* Chat hack material
** This is where the other hacks can be triggered by specific commands through chat
*/
//Offset for Player::Chat
uintptr_t chatFunctionOffset = 0x551a0;

typedef void(__thiscall* OriginalChatFunc)(void* thisPlayer, const char* text);
OriginalChatFunc originalChat = nullptr;

// Declare a pointer-to-member function type
typedef void(__thiscall* ChatFuncType)(const char* text);

// Custom function to intercept and modify the Chat function behavior
void __fastcall MyCustomChat(void* thisPlayer, ChatFuncType func, const char* originalText) {
	std::cout << "Chat input string: " << originalText;

	std::string originalTextStr = originalText;

	// Commands to start a hack
	if (strcmp(originalText, "init spaceInvaders") == 0) {
		std::cout << "Space Invaders started";
		spaceInvaders();
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
	else if (originalTextStr.rfind("set inventory1", 0) == 0) {
		std::cout << "Inventory 1 hack started";

	}
	else if (originalTextStr.rfind("set inventory2", 0) == 0) {
		std::cout << "Inventory 2 hack started";

	}
	else if (originalTextStr.rfind("set inventory3", 0) == 0) {
		std::cout << "Inventory 3 hack started";

	}
	else if (originalTextStr.rfind("get gun", 0) == 0) {
		std::cout << "\nGun hack started";
		CallAddItem(thisPlayer, 1, true);
	}
	else if (strcmp(originalText, "change bearDisplayName") == 0) {
		std::cout << "\nBear Display name hack started";
		std::string name = getLastChar(originalText, " ");
		globalModifiedName = name.c_str();
		HookGetDisplayNameFunction(0x51c0);
	}
	else if (strcmp(originalText, "change ratDisplayName") == 0) {
		std::cout << "\nRat Display name hack started";

		HookGetDisplayNameFunction(0x38370);
	}
	else if (strcmp(originalText, "enable peace mode") == 0) {
		std::cout << "\nPeace mode hack started - enable";
		enablePeaceMode();
		std::cout << "Peace mode enabled";
	}
	else if (strcmp(originalText, "disable peace mode") == 0) {
		std::cout << "\nPeace mode hack started - disable";
		setHealth(recentHealthVal);
		std::cout << "Peace mode disabled";
	}
	else if (strcmp(originalText, "enable loneliness") == 0) {
		std::cout << "Loneliness mode hack started - enable";
		lonelinessModeEnabled = true;
		pushBears(NULL);
		std::cout << "Loneliness mode enabled";
	}
	else if (strcmp(originalText, "disable loneliness") == 0) {
		std::cout << "Loneliness mode hack started - disable";
		lonelinessModeEnabled = false;
		std::cout << "Loneliness mode disabled";
	}
}

// Intersept the existing chat function and execute our custom one
void HookChatFunction() {
	std::cout << "HookChatFunction: Starting." << std::endl;

	// Calculate the actual runtime address of the Chat function.
	uintptr_t actualFunctionAddress = runtimeBaseAddress + chatFunctionOffset;
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
