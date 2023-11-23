#include "pch.h"
#include <Windows.h>
#include <iostream> 

DWORD WINAPI MyThread(HMODULE hModule)
{
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

	float* x = (float*)(forthStep + 0x90);
	float* y = (float*)(forthStep + 0x94);
	float* z = (float*)(forthStep + 0x98);

	while (true) {
		// If the player presses H, they teleport to the House quest
		if (GetAsyncKeyState('H') & 1) {
			*x = -41096.36328;
			*y = -16556.55273;
			*z = 2400;
		}
		// If the player presses B, they teleport to the Bear quest
		if (GetAsyncKeyState('B') & 1) {
			*x = -7969.634766;
			*y = 63891.65234;
			*z = 2700;
		}
		// If the player presses F, they teleport to the Fort Blox quest
		if (GetAsyncKeyState('F') & 1) {
			*x = -14638.12109;
			*y = -6318.598633;
			*z = 2300;
		}
	}
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MyThread, hModule, 0, nullptr));
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}