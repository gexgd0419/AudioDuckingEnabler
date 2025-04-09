#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <detours.h>
#include <oleacc.h>

decltype(&AccSetRunningUtilityState) Real_AccSetRunningUtilityState = nullptr;
decltype(&GetTokenInformation) Real_GetTokenInformation = nullptr;
HMODULE hOleacc = nullptr, hSecbase = nullptr;
DWORD threadId = 0;
CRITICAL_SECTION critSec;

HRESULT WINAPI My_AccSetRunningUtilityState(HWND hwndApp, DWORD dwUtilityStateMask, DWORD dwUtilityState)
{
	EnterCriticalSection(&critSec);
	threadId = GetCurrentThreadId();
	HRESULT hr = Real_AccSetRunningUtilityState(hwndApp, dwUtilityStateMask, dwUtilityState);
	threadId = 0;
	LeaveCriticalSection(&critSec);
	return hr;
}

BOOL WINAPI My_GetTokenInformation(
    HANDLE TokenHandle,
    TOKEN_INFORMATION_CLASS TokenInformationClass,
    LPVOID TokenInformation,
    DWORD TokenInformationLength,
    PDWORD ReturnLength
)
{
	BOOL ret = Real_GetTokenInformation(TokenHandle, TokenInformationClass,
		TokenInformation, TokenInformationLength, ReturnLength);
	if (!ret || threadId != GetCurrentThreadId())
	{
		// Only modify TokenUIAccess for the thread calling AccSetRunningUtilityState
		return ret;
	}
	if (TokenInformationClass == TokenUIAccess)
	{
		// Force TokenUIAccess to return TRUE to pass the UIAccess check
		DWORD info = TRUE;
		memcpy(TokenInformation, &info, sizeof(DWORD));
	}
	return ret;
}

BOOL APIENTRY DllMain(
	HMODULE hModule,
	DWORD   ul_reason_for_call,
	LPVOID  lpReserved
)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		InitializeCriticalSection(&critSec);
		hOleacc = LoadLibraryW(L"oleacc.dll");
		Real_AccSetRunningUtilityState =
			reinterpret_cast<decltype(&AccSetRunningUtilityState)>
			(GetProcAddress(hOleacc, "AccSetRunningUtilityState"));
		if (!Real_AccSetRunningUtilityState)
		{
			FreeLibrary(hOleacc);
			return FALSE;
		}
		hSecbase = LoadLibraryW(L"api-ms-win-security-base-l1-1-0");
		Real_GetTokenInformation =
			reinterpret_cast<decltype(&GetTokenInformation)>
			(GetProcAddress(hSecbase, "GetTokenInformation"));
		if (!Real_GetTokenInformation)
		{
			FreeLibrary(hSecbase);
			FreeLibrary(hOleacc);
			return FALSE;
		}
		
		DetourTransactionBegin();
		DetourAttach(&Real_AccSetRunningUtilityState, My_AccSetRunningUtilityState);
		DetourAttach(&Real_GetTokenInformation, My_GetTokenInformation);
		if (DetourTransactionCommit() != NO_ERROR)
		{
			FreeLibrary(hSecbase);
			FreeLibrary(hOleacc);
			return FALSE;
		}
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		DetourTransactionBegin();
		DetourDetach(&Real_AccSetRunningUtilityState, My_AccSetRunningUtilityState);
		DetourDetach(&Real_GetTokenInformation, My_GetTokenInformation);
		DetourTransactionCommit();
		FreeLibrary(hSecbase);
		FreeLibrary(hOleacc);
	}
	return TRUE;
}
