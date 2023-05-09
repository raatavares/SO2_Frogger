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
		_tprintf(TEXT("\n[MAPA] CenTaxi não enviou mapa!\n"));
		pDados->board = NULL;
		return;
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


	while (!pDados->TERMINAR) {
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
	TCHAR op[TAM], i;
	buffer_circular* dados = ((buffer_circular*)param);
	int seg, pista;

	do {
		_tprintf(_T("\n\n"));
		i = _gettch();
		WaitForSingleObject(hMutex, INFINITE);
		_tprintf(_T("%c"), i);
		op[0] = i;
		_fgetts(&op[1], sizeof(op), stdin);
		op[_tcslen(op) - 1] = '\0';
		//PARA O MOVIMENTO DOS CARROS POR X SEGUNDOS
		if (!_tcscmp(op, TEXT("para"))) {
			do {
				_tprintf(_T("\n[COMANDO] Introduza os segundos: "));
				_tscanf_s(_T("%d"), &seg);
			} while (seg <= 0);
			dados->paraMovimento = TRUE;
			dados->segundosParar = seg;
		}
		//INSERIR OBSTACULO
		else if (!_tcscmp(op, TEXT("obstaculo"))) {
			dados->insereObstaculo = TRUE;
		}
		//INVERTE SENTIDO DE RODAGEM DE X FAIXA
		else if (!_tcscmp(op, TEXT("inverte"))) {
			do {
				_tprintf(_T("\n[COMANDO] Introduza a pista: "));
				_tscanf_s(_T("%d"), &pista);
			} while (pista < 0 || pista > dados->maxPista);
			dados->inverteSentido = TRUE;
			dados->pistaInverter = pista;
		}
		//AJUDA NOS COMANDOS
		else if (!_tcscmp(op, TEXT("ajuda"))) {
			_tprintf(_T("\n[COMANDO] Aqui vai uma ajuda..."));
			ajuda();
		}
		if (_tcscmp(op, TEXT("fim")))
			ReleaseMutex(hMutex);
		else
			_tprintf(_T("\nA sair...\n"));
		_tprintf(_T("\n\n"));
	} while (_tcscmp(op, TEXT("fim")));

	//pDados->TERMINAR = 1;

	ReleaseMutex(hMutex);

	Sleep(1000);

	ExitThread(0);
}


int _tmain(int argc, TCHAR* argv[]) {
	HANDLE hThreadComandos, hThreadAtualizaMapa;
	buffer_circular dados;
	mapping pDados;
	HINSTANCE hLib;
	pDados.TERMINAR = 0;

	hLib = LoadLibrary(PATH_DLL);
	if (hLib == NULL)
		return 0;


#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif


	leMapa(&pDados);
	if (pDados.board == NULL) {
		UnmapViewOfFile(pDados.board);
		CloseHandle(pDados.hFileMap);
		return 0;
	}



	if (!pDados.TERMINAR) {
		hThreadComandos = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadComandos, (LPVOID)&dados, 0, NULL);
		if (hThreadComandos == NULL) {
			_tprintf(TEXT("\n[ERRO] Erro ao lançar Thread!\n"));
			return 0;
		}
		hThreadAtualizaMapa = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadAtualizaMapa, (LPVOID)&pDados, 0, NULL);
		if (hThreadAtualizaMapa == NULL) {
			_tprintf(TEXT("\n[ERRO] Erro ao lançar Thread!\n"));
			return 0;
		}


		HANDLE ghEvents[2];
		ghEvents[0] = hThreadComandos;
		ghEvents[1] = hThreadAtualizaMapa;
		WaitForMultipleObjects(2, ghEvents, FALSE, INFINITE);
	}

	return 0;
}