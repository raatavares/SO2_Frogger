#pragma once
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#define PATH_DLL TEXT("..\\x64\\Debug\\DLL.dll")

#define TAM 200

HANDLE hMutex;


DWORD WINAPI ThreadComandos(LPVOID param);
