#pragma once

#define WIN32_LEAN_AND_MEAN             // Excluir itens raramente utilizados dos cabe�alhos do Windows
// Arquivos de Cabe�alho do Windows
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>



typedef struct {
	char caracter;
} MAPA;

typedef struct {
	int TERMINAR;
	MAPA* mapa;
	MAPA* sharedMap;
} DADOS;


#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)//export
#else
#define DLL_API __declspec(dllimport)//import
#endif

