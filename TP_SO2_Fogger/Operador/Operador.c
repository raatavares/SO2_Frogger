#include "Operador.h"
#include "..\\DLL\Header.h"



void leMapa(mapping* pDados) {
	pDados->hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(mapping), TEXT("TP_MEM_PART"));
	if (pDados->hFileMap == NULL) {
		_tprintf(TEXT("Erro no CreateFileMapping\n"));
		return;
	}
	pDados->board = (matriz*)MapViewOfFile(pDados->hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(pDados->board));
	if (pDados->board == NULL) {
		_tprintf(TEXT("Erro no MapViewOfFile\n"));
		CloseHandle(pDados->hFileMap);
		return;
	}

	matriz* aux = NULL;
	CopyMemory(&aux, pDados->board, sizeof(pDados->board));
	if (aux == NULL) {
		_tprintf(TEXT("\n[MAPA] Servidor não enviou mapa!\n"));
		pDados->board = NULL;
		return;
	}
	hSemAtualizaMapa = CreateSemaphore(NULL, 0, BUFFER_SIZE, TEXT("TP_SEMAFORO_MAPA"));
	if (hSemAtualizaMapa == NULL) {
		_tprintf(TEXT("Erro no CreateSemaphore\n"));
		return 1;
	}

	for (int i = 0; i < pDados->board->rows; i++) {
		for (int j = 0; j < pDados->board->cols; j++) {
			_tprintf(TEXT("%c"), pDados->board->board[i][j]);
		}
		_tprintf(TEXT("\n"));
	}

	_tprintf(TEXT("\n[MAPA] Mapa lido com sucesso!\n\n"));

	return;
}

DWORD WINAPI ThreadAtualizaMapa(LPVOID param) {
	mapping* pDados = ((mapping*)param);
	pDados->board->terminar = 1;
	while (pDados->board->terminar) {
		//_tprintf(TEXT("[%d]"), pDados->board->terminar);

		WaitForSingleObject(hSemAtualizaMapa, INFINITE);
		WaitForSingleObject(hMutex, INFINITE);
		

		_tprintf(TEXT("\n"));
		for (int i = 0; i < pDados->board->rows; i++) {
			for (int j = 0; j < pDados->board->cols; j++) {
				_tprintf(TEXT("%c"), pDados->board->board[i][j]);
			}
			_tprintf(TEXT("\n"));
		}
		ReleaseMutex(hMutex);
		Sleep(1000);
	};
	//_tprintf(TEXT("[%d]"), pDados->board->terminar);

	ExitThread(0);
}

void ajuda() {
	_tprintf(_T("\n\n para - PARA O MOVIMENTO DOS CARROS POR X SEGUNDOS"));
	_tprintf(_T("\n obstaculo - INSERIR OBSTACULO"));
	_tprintf(_T("\n inverte - INVERTE SENTIDO DE RODAGEM DE X FAIXA"));
	_tprintf(_T("\n fim - ENCERRAR TODO O SISTEMA"));
	return;
}

DWORD WINAPI ThreadComandos(LPVOID param) {
	TCHAR op[TAM];
	buffer_circular* buffer = ((buffer_circular*)param);
	int seg, pista;
	pedido pedidoN;

	//buffer = (buffer_circular*)malloc(sizeof(buffer_circular));
	//inicializaBuffer(buffer);
	HANDLE hFileBuffer = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, TEXT("TP_BufferCircular"));

	if (hFileBuffer == NULL) {
		_tprintf(TEXT("Erro no CreateFileMapping\n"));
		return;
	}
	buffer = (buffer_circular*)MapViewOfFile(hFileBuffer, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (buffer == NULL) {
		_tprintf(TEXT("Erro no MapViewOfFile\n"));
		return;
	}
	hSemEscrita = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, TEXT("TP_SEMAFORO_ESCRITA"));
	if (hSemEscrita == NULL) {
		_tprintf(TEXT("Erro no CreateSemaphore\n"));
		return 0;
	}
	hSemLeitura = CreateSemaphore(NULL, 0, BUFFER_SIZE, TEXT("TP_SEMAFORO_LEITURA"));
	if (hSemLeitura == NULL) {
		_tprintf(TEXT("Erro no CreateSemaphore\n"));
		return 0;
	}
	

	do {
		

		
			
		

		WaitForSingleObject(hSemEscrita, INFINITE);
		WaitForSingleObject(hMutex, INFINITE);

		_tprintf(_T("\n\n"));
		fflush(stdin);
		_fgetts(op, sizeof(op) / sizeof(TCHAR), stdin);


		op[_tcslen(op) - 1] = '\0';
		if (!_tcscmp(op, TEXT("para"))) {
			do {
				_tprintf(_T("\n[COMANDO] Introduza os segundos: "));
				_tscanf_s(_T("%d"), &pedidoN.segundosParar);
			} while (pedidoN.segundosParar <= 0);
			pedidoN.paraMovimento = 1;
			pedidoN.inverteSentido = 0;
			pedidoN.insereObstaculo = 0;
			CopyMemory(&buffer->pedidos[buffer->posE], &pedidoN, sizeof(pedido));

			buffer->posE = (buffer->posE + 1) % BUFFER_SIZE;  // Avança a posição de escrita no buffer circular
		}
		else if (!_tcscmp(op, TEXT("obstaculo"))) {
			pedidoN.insereObstaculo = 1;
			pedidoN.paraMovimento = 0;
			pedidoN.inverteSentido = 0;
			CopyMemory(&buffer->pedidos[buffer->posE], &pedidoN, sizeof(pedido));
			buffer->posE = (buffer->posE + 1) % BUFFER_SIZE;  // Avança a posição de escrita no buffer circular
		}
		else if (!_tcscmp(op, TEXT("inverte"))) {
			do {
				_tprintf(_T("\n[COMANDO] Introduza a pista: "));
				_tscanf_s(_T("%d"), &pista);
			} while (pista < 0);
			_tprintf(_T("\nRR"));
			pedidoN.inverteSentido = pista;
			pedidoN.paraMovimento = 0;
			pedidoN.insereObstaculo = 0;
			_tprintf(_T("\n2"));
			pedidoN.pistaInverter = pista;
			_tprintf(_T("\n3"));
			CopyMemory(&buffer->pedidos[buffer->posE], &pedidoN, sizeof(pedido));
			buffer->posE = (buffer->posE + 1) % BUFFER_SIZE;  // Avança a posição de escrita no buffer circular
		}
		else if (!_tcscmp(op, TEXT("ajuda"))) {
			_tprintf(_T("\n[COMANDO] Aqui vai uma ajuda..."));
			ajuda();
		}
		ReleaseMutex(hMutex);

		ReleaseSemaphore(hSemLeitura, 1, NULL);

		_tprintf(_T("\n\n"));
	} while (_tcscmp(op, TEXT("fim")));

	_tprintf(_T("\nA sair...\n"));

	Sleep(1000);

	ExitThread(0);
}


int _tmain(int argc, TCHAR* argv[]) {
	HANDLE hThreadComandos, hThreadAtualizaMapa;
	buffer_circular buffer;
	mapping pDados;
	pDados.TERMINAR = 0;


#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	_tprintf(TEXT("\nA aguardar...\n"));
	HANDLE hSemInstancias = CreateSemaphore(NULL, 2, 2, TEXT("SEM_INSTANCIAS"));
	WaitForSingleObject(hSemInstancias, INFINITE);

	leMapa(&pDados);
	if (pDados.board == NULL) {
		UnmapViewOfFile(pDados.board);
		CloseHandle(pDados.hFileMap);
		return 0;
	}

	if (!pDados.TERMINAR) {
		hThreadComandos = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadComandos, (LPVOID)&buffer, 0, NULL);
		if (hThreadComandos == NULL) {
			_tprintf(TEXT("\n[ERRO] Erro ao lançar Thread!\n"));
			return 0;
		}
		hThreadAtualizaMapa = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadAtualizaMapa, (LPVOID)&pDados, 0, NULL);
		if (hThreadAtualizaMapa == NULL) {
			_tprintf(TEXT("\n[ERRO] Erro ao lançar Thread!\n"));
			return 0;
		}

		/*HANDLE ghEvents[2];
		ghEvents[0] = hThreadComandos;
		ghEvents[1] = hThreadAtualizaMapa;
		WaitForMultipleObjects(2, ghEvents, FALSE, INFINITE);*/
		WaitForSingleObject(hThreadAtualizaMapa, INFINITE);
		TerminateThread(hThreadComandos, 0);
		WaitForSingleObject(hThreadComandos, INFINITE);

	}

	ReleaseSemaphore(hSemInstancias, 1, NULL);

	return 0;
}