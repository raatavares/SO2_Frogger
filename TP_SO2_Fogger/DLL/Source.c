// dllmain.cpp : Define o ponto de entrada para o aplicativo DLL.
#include "Header.h"

void inicializaBuffer(buffer_circular* buffer) {

	hFileBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(buffer_circular), TEXT("TP_BufferCircular"));

	if (hFileBuffer == NULL) {
		_tprintf(TEXT("Erro no CreateFileMapping\n"));
		return;
	}
	buffer = (buffer_circular*)MapViewOfFile(hFileBuffer, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (buffer == NULL) {
		_tprintf(TEXT("Erro no MapViewOfFile\n"));
		return;
	}
	return;
}
