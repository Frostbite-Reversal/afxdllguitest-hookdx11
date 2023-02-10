#pragma once
#include"pch.h"
class Helper
{
public:
	Helper();
	~Helper();
	static void HookFunction(PVOID *oFunction, PVOID pDetour);
	static void UnHookFunction(PVOID *oFunction, PVOID pDetour);
};

