#pragma once

#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#define TAM 200
#define MAXFAIXAS 8

typedef struct {
	HANDLE hEvent,hMutex;
	TCHAR** board;
	int rows, cols;
	int faixaNumero,faixaVelocidade;
} data;

BOOL putSapo(TCHAR** board, int numFaixas, int cols);

void show(TCHAR** board, int numFaixas, int cols);
TCHAR** createBoard(int numFaixas, int cols);

