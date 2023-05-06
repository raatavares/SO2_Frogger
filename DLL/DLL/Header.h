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


#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)//export
#else
#define DLL_API __declspec(dllimport)//import
#endif