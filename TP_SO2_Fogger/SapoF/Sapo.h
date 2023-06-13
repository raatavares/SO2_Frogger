#pragma once

#include "resource.h"
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <commctrl.h>


typedef struct {
	TCHAR board[10][20];
} MAPA;

typedef struct {
	MAPA* mapa;
} DADOS;

HANDLE hPipe;