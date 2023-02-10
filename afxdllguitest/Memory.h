#pragma once
#include"pch.h"
//#include<algorithm>
#include<array>
#define BUFFER_SIZE 20//You could change the buffer size make the vehicle acceleration more stable, but I don't know the best buffer size.
#include<mmsystem.h>//
#pragma comment(lib,"winmm.lib")
#include<typeinfo>
#ifdef _WIN64
#define UDWORD DWORD64
#define _PTR_MAX_VALUE ((PVOID)0x000F000000000000)
#else
#define UDWORD DWORD32
#define _PTR_MAX_VALUE ((PVOID)0xFFF00000)
#endif
#define uint unsigned int
//check project type
#ifdef _DLL_
#define IsDllProj true
#else
#define IsDllProj false
#endif

//check is use cstring
#ifdef _CSTRING_
#define IsCstring true
#else
#define IsCstring false
#endif




__forceinline bool IsValidPtr(PVOID p) { return (p >= (PVOID)0x10000) && (p < _PTR_MAX_VALUE) && p != nullptr; }
class Memory;

#ifdef _CSTRING_
class MyString {
public:
	char String[100];
	CString ToCstring();
	MyString(char* str);
	MyString();
	MyString& operator=( MyString& str);
	MyString& operator=(char* str);
	MyString& operator=(CString& str);
	bool operator==(MyString& str);
	MyString& operator+(MyString& str);
	int Find(const MyString& str);
	MyString& Reverse();
	int GetLength();
	char * Getstring();
	char CharAt(int i);
	CString prefix(int i);
	CString suffix(int i);
	bool IsEmpty();
	bool IsValid();
};
inline MyString::MyString(char* str)
{
	strcpy_s(String, str);
}
inline MyString::MyString()
{
	strcpy_s(String, "");
}

inline CString MyString::ToCstring()
{
	
	return CString(this->String);
}
inline MyString& MyString::operator=(MyString& str)
{
	strcpy_s(this->String, str.String);
	return *this;
}
inline MyString& MyString::operator=(char* str)
{
	strcpy_s(String, str);
	return *this;
}
inline MyString& MyString::operator=(CString& str)
{
#ifdef _UNICODE
	char* tmpch;
	int wLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	tmpch = new char[wLen];
	WideCharToMultiByte(CP_ACP, 0, str, -1, tmpch, wLen, NULL, NULL);
	for (int i = 0; i < wLen; i++)
	{
		String[i] = tmpch[i];
	}
	delete[] tmpch;
#endif // _UNICODE
#ifndef _UNICODE
	//ch = str.GetBuffer(str.GetLength());
	//ch = (LPSTR)(LPCTSTR)str;
	//str.ReleaseBuffer();
	strcpy(ch, str);
#endif // !_UNICODE
	return *this;
}
inline MyString& MyString::operator+(MyString& str)
{
	strcat_s(String, str.String);
	return *this;
}
inline int MyString::Find(const MyString& str)
{
	return strstr(String, str.String) - String;
}
inline MyString& MyString::Reverse()
{
	char* p = String;
	char* q = String + strlen(String) - 1;
	while (p < q)
	{
		char temp = *p;
		*p = *q;
		*q = temp;
		p++;
		q--;
	}
	return *this;
}
inline int MyString::GetLength()
{
	return strlen(String);
}
inline char * MyString::Getstring()
{
	return String;
}
inline char MyString::CharAt(int i)
{
	return String[i];
}
inline CString MyString::prefix(int i){
	return CString(String).Left(i);
}
inline CString MyString::suffix(int i){
	return CString(String).Right(i);
}
inline bool MyString::operator==(MyString& str)
{
	return strcmp(String, str.String) == 0;
}
inline bool MyString::IsEmpty()
{
	return strlen(String) == 0;
}
inline bool MyString::IsValid()
{
	return strlen(String) > 0;
}
#endif
class Memory {
private:
	bool isdll = IsDllProj;
public:
	Memory() = default;
#ifndef _DLL_
	Memory(HANDLE handle)
	{
		buffer = 0x0;
		this->ProcessHandle = handle;
	}
#endif // !_DLL_
	virtual ~Memory(){}
	UDWORD buffer;
	SIZE_T dwRead;
	HANDLE ProcessHandle;
	template <class T= UDWORD>
	inline T read(UDWORD addr)
	{
		T result= T();
		if (IsValid(addr,sizeof(T))){
#ifndef _DLL_
		ReadProcessMemory(ProcessHandle, (LPVOID)(addr), &result, sizeof(T), &dwRead);			
#else
		try {
			return *(T*)addr;
		}
		catch (CException& e)
		{
			TCHAR errorreason[1024];
			e.GetErrorMessage(errorreason, 1024);
			CString errorstr;
			errorstr.Format(TEXT("Read failed! Locate in [ 0x%x ] ,type: %s,Reason:"), addr, typeid(T).raw_name());
			OutputDebugString(errorstr + errorreason);
			return result;
		}
#endif // _DLL_
		}
		return result;
	}
	
	template <class T = float>
	inline T Read(int num, ...)
	{
		va_list valist;
		va_start(valist, num);
		T result =T();
		buffer = read<UDWORD>(va_arg(valist, UDWORD));
		for (int i = 0; i < num - 2; i++)
		{
			buffer = read<UDWORD>((UDWORD)va_arg(valist, DWORD) + buffer);
		}
#ifdef _CSTRING_
		if (typeid(T) == typeid(CString))
		{
			*static_cast<CString*>((void*)&result)= read<MyString>(buffer + (UDWORD)va_arg(valist, DWORD)).ToCstring();
		}
		else
		{
			result = read<T>(buffer + (UDWORD)va_arg(valist, DWORD));
		}
#else
		result = read<T>(buffer + (UDWORD)va_arg(valist, DWORD));
#endif
		va_end(valist);
		return result;
	}
	template<class T=float>
	inline T Read(std::initializer_list<UDWORD> offsets)
	{
		T result=T();
		for (auto it = offsets.begin(); it != offsets.end()-1; ++it)
		{
			buffer = read(*it + buffer);
		}
#ifdef _CSTRING_
		if (typeid(T) == typeid(CString))
		{
			*static_cast<CString*>((void*)&result) = read<MyString>(buffer+ *(offsets.end() - 1)).ToCstring();
		}else{
			result = read<T>(buffer + *(offsets.end() - 1));
		}
#else
		result = read<T>(buffer + *(offsets.end() - 1));
#endif // _CSTRING_
		return result;
	}
	template <class T>
	inline int write(UDWORD addr, T content)
	{
		if (IsValid(addr,sizeof(T))) return -1;//not a valid addr, write error
#ifndef _DLL_
		return (WriteProcessMemory(ProcessHandle, (LPVOID)(addr), &content, sizeof(T), &dwRead) != FALSE) ? content : T();
#else
		try
		{
			if (IsBadReadPtr((LPVOID)addr, sizeof(T)))
			{
				*(T*)addr = content;
			}
			else
			{
				return false;
			}
		}
		catch (CException& e)
		{
			TCHAR errorreason[1024];
			e.GetErrorMessage(errorreason, 1024);
			CString errorstr;
			errorstr.Format(TEXT("Write failed! Locate in [ 0x%x ] ,type: %s,Reason:"), addr, typeid(T).raw_name());
			OutputDebugString(errorstr + errorreason);
			return -1;
		}
#endif // !_DLL_
	}
	template <class T>
	inline void Write(T content, int num, ...)
	{
		va_list valist;
		va_start(valist, num);
		UDWORD old = read<UDWORD>(va_arg(valist, UDWORD));
		for (int i = 0; i < num - 2; i++)
		{
			UDWORD TB = (int)(UDWORD)va_arg(valist, DWORD);
			old = read<UDWORD>(TB + old);
		}
		UDWORD TB1 = (int)(UDWORD)va_arg(valist, DWORD);
		int a = write<T>(old + TB1, content);
		va_end(valist);
		
	}
	template <class T>
	inline void Write(std::initializer_list<UDWORD> offsets, T content)//Untested function,please don't use that
	{
		UDWORD old = read<UDWORD>(offsets.begin()[0]);
		for (auto it = offsets.begin() + 1; it != offsets.end() - 1; ++it)
		{
			old = read<UDWORD>(*it + old);
		}
		int a = write<T>(old + offsets.end()[-1], content);
	}
	inline UDWORD ReadPtr(int num, ...)
	{
		va_list valist;
		va_start(valist, num);
		UDWORD old = 0x0;
		for (int i = 0; i < num-1; i++)
		{
			if (i == 0)
			{
				UDWORD TB = (UDWORD)va_arg(valist, UDWORD);
				old = read<UDWORD>(TB + old);
			}
			else
			{
				UDWORD TB = (UDWORD)va_arg(valist, DWORD);
				old = read<UDWORD>(TB + old);
			}

		}
		UDWORD TB = (UDWORD)va_arg(valist, DWORD);
		UDWORD result = old + TB;
		if (result < 0xFFFF)
		{
			result = 0x0;
		}
		va_end(valist);
		return (result);
	}
	__forceinline bool IsValid(UDWORD addr,size_t nSize=4)
	{
#ifdef _DLL_
		return IsBadReadPtr((LPVOID)addr, nSize);
#else
		return (addr >= 0x10000 && addr < 0x000F000000000000);
#endif // _DLL_
	}
	template<typename T=float>
	T operator[](UDWORD addr)//float as default
	{
		return read<T>(addr);
	}
	__forceinline UDWORD SimpleFindPatten(UDWORD start, UDWORD end,BYTE patten[])
	{
		for (UDWORD i = start; i < end; i++)
		{
			if (read<BYTE>(i) == patten[0])
			{
				for (int j = 1; j < sizeof(patten); j++)
				{
					if (read<BYTE>(i + j) != patten[j])
					{
						break;
					}
					if (j == sizeof(patten) - 1)
					{
						return i;
					}
				}
			}
		}
		return 0x0;
		
	}
	inline UDWORD FindPatten(UDWORD begin, UDWORD end, BYTE patten[], int _patten_length)
	{
		if (end<begin) std::swap<DWORD64>(end, begin);
		DWORD64 addrs_length = end - begin;
		DWORD64 patten_length = _patten_length;
		if (patten_length > addrs_length) return begin;
#ifndef DEBUG
		BYTE* dump_memory = new BYTE[addrs_length];
		DWORD64 read_length = 2048;
		DWORD64 read_count = addrs_length / read_length;
		DWORD64 read_remainder = addrs_length % read_length;
		for (int i = 0; i < read_count; i++)
		{
			size_t rsize = read_length;
			ReadProcessMemory(ProcessHandle, (LPVOID)(begin + i * read_length), dump_memory + i * read_length, read_length, &rsize);
		}
		if (read_remainder != 0)
		{
			size_t rsize = read_remainder;
			ReadProcessMemory(ProcessHandle, (LPVOID)(begin + read_count * read_length), dump_memory + read_count * read_length, read_remainder, &rsize);
		}
#else
		BYTE* dump_memory = (BYTE*)begin
#endif // DEBUG
		int* next = new int[patten_length];
		memset(next, 0, patten_length);
		DWORD32 x = 1, y = 0;
		next[1] = 0;
		while (x < patten_length)
		{
			if (y == 0 || patten[y] == patten[x])
			{
				x++; y++;
				if (patten[x] != patten[y])
				{
					next[x] = y;
				}
				else
				{
					next[x] = next[y];
				}
			}
			else
			{
				y = next[y];
			}
		}
		DWORD64 i = 0;
		DWORD64 j = 0;
		while (i < addrs_length && j < patten_length)
		{
			if (j == 0 || dump_memory[i]== patten[j])
			{
				i++;
				j++;
			}
			else
			{
				j = next[j];
			}
		}
#ifndef _DLL_
		delete[] dump_memory;
#endif // !_DLL_
		delete[] next;
		if (j >= patten_length)
		{
			return (begin + i) - patten_length;
		}
		else
		{
			return begin;
		}
	}
	
};
 









