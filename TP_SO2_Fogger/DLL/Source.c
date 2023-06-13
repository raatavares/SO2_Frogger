// dllmain.cpp : Define o ponto de entrada para o aplicativo DLL.
#include "Header.h"

void escritaBufferCircular(buffer_circular* buffer, pedido* pedidoN) {
	CopyMemory(&buffer->pedidos[buffer->posE], pedidoN, sizeof(pedido));
	return;
}

void leituraBufferCircular(buffer_circular* buffer, pedido* pedidoN) {
	CopyMemory(pedidoN, &buffer->pedidos[buffer->posL], sizeof(pedido));
	return;
}

void imprimeMapa(mapping* pDados) {
	_tprintf(TEXT("\n"));
	for (int i = 0; i < pDados->board->rows; i++) {
		for (int j = 0; j < pDados->board->cols; j++) {
			_tprintf(TEXT("%c"), pDados->board->board[i][j]);
		}
		_tprintf(TEXT("\n"));
	}
}
