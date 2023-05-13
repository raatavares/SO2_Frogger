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
#define BUFFER_SIZE 10

#define PATH_DLL TEXT("..\\x64\\Debug\\DLL.dll")


typedef struct {
	HANDLE hEvent, hMutex;
	TCHAR** board;
	int rows, cols;
	int faixaNumero, faixaVelocidade;
	TCHAR* command;
	int* TERMINAR;
} data;

typedef struct {
	TCHAR board[10][20];
	int rows, cols, terminar;
}matriz;
typedef struct {
	int* TERMINAR;
	data* jogo;
	matriz* board;
	HANDLE hFileMap;
	HANDLE hEvent;
	HANDLE hMutex;
} mapping; //struct para mapping

//Semaforo AtualizaMapa
HANDLE hSemAtualizaMapa;

typedef struct {
	int segundosParar, pistaInverter;
	int insereObstaculo, paraMovimento, inverteSentido;
} pedido;

typedef struct {
	pedido pedidos[BUFFER_SIZE];
	int posE; //proxima posicao de escrita
	int posL; //proxima posicao de leitura
	int* TERMINAR;
} buffer_circular;

//Mapping BufferCircular
HANDLE hFileBuffer;
//Semaforo BufferCircular
HANDLE hSemEscrita, hSemLeitura;

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)//export
#else
#define DLL_API __declspec(dllimport)//import
#endif

extern DLL_API void inicializaBuffer(buffer_circular* buffer);
