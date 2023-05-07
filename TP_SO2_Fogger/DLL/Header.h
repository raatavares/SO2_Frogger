#pragma once

#define WIN32_LEAN_AND_MEAN             // Excluir itens raramente utilizados dos cabeçalhos do Windows
// Arquivos de Cabeçalho do Windows
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM 200
#define MAXFAIXAS 8

#define PATH_DLL TEXT("..\\x64\\Debug\\DLL.dll")


typedef struct {
	HANDLE hEvent, hMutex;
	TCHAR** board;
	int rows, cols;
	int faixaNumero, faixaVelocidade;
} data;

typedef struct {
	TCHAR board[10][20];
	int TERMINAR;
} DADOS;

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)//export
#else
#define DLL_API __declspec(dllimport)//import
#endif

