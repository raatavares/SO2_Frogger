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

#define TAM 200
#define MAXFAIXAS 8
#define BUFFER_SIZE 10

#define PATH_DLL TEXT("..\\x64\\Debug\\DLL.dll")
#define starterPipe TEXT("\\\\.\\pipe\\teste")
#define sendMapTo_S_Pipe TEXT("\\\\.\\pipe\\map_S")
#define sendMapTo_s_Pipe TEXT("\\\\.\\pipe\\map_s")



typedef struct {
	HANDLE hEvent, hMutex;
	TCHAR** board;
	int rows, cols;
	int faixaNumero, faixaVelocidade;
	TCHAR* command;
	int* TERMINAR;
	int modo;
} data;

typedef struct {
	TCHAR board[10][20];
	int rows, cols, terminar;
} matriz;

typedef struct {
	int* TERMINAR;
	data* jogo;
	matriz* board;
	HANDLE hFileMap;
	HANDLE hEvent;
	HANDLE hMutex;
	TCHAR* command;
	HANDLE sharedMapEvent; //evento alerta atualização de mapa
} mapping; //struct para mapping

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


typedef struct {
	//TCHAR board[10][20];
	//int rows, cols, terminar;
	int mode;//0-singleplayer
	TCHAR move;
	TCHAR player_char;
} player;
typedef struct {
	player players[2];
	data* jogo;
} pipe_user_server;

//Mapping BufferCircular
HANDLE hFileBuffer;
//Semaforo BufferCircular
HANDLE hSemEscrita, hSemLeitura;

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)//export
#else
#define DLL_API __declspec(dllimport)//import
#endif

extern DLL_API void escritaBufferCircular(buffer_circular* buffer, pedido* pedidoN);
extern DLL_API void leituraBufferCircular(buffer_circular* buffer, pedido* pedidoN);
extern DLL_API void imprimeMapa(mapping* pDados);

