#pragma once
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>



HANDLE hMutex;


DWORD WINAPI ThreadComandos(LPVOID param);
