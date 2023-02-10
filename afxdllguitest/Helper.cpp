#include "pch.h"
#include "Helper.h"

Helper::Helper()
{
}

Helper::~Helper()
{
}

void Helper::HookFunction(PVOID * oFunction, PVOID pDetour)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(oFunction, pDetour);
	DetourTransactionCommit();
}

void Helper::UnHookFunction(PVOID * oFunction, PVOID pDetour)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(oFunction, pDetour);
	DetourTransactionCommit();
}
