#pragma once

#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>


BOOL putSapo(TCHAR** board, int numFaixas, int cols);

void show(TCHAR** board, int numFaixas, int cols);
TCHAR** createBoard(int numFaixas, int cols);
int gerarAleatorio(int minimo, int maximo); 
