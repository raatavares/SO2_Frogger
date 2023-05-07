#pragma once
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>



HANDLE hMutex;


DWORD WINAPI ThreadAtualizaMapa(LPVOID param);
DWORD WINAPI ThreadComandos(LPVOID param);
