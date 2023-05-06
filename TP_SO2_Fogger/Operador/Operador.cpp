#include "Operador.h"
#include "..\\DLL\Header.h"


int _tmain(int argc, LPTSTR argv[]) {
	HANDLE hThreadComandos;
	DADOS dados;
	HINSTANCE hLib;
	dados.TERMINAR = 0;

	hLib = LoadLibrary(PATH_DLL);
	if (hLib == NULL)
		return 0;

#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif
	if (!dados.TERMINAR) {
		hThreadComandos = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadComandos, (LPVOID)&dados, 0, NULL);
		if (hThreadComandos == NULL) {
			_tprintf(TEXT("\n[ERRO] Erro ao lançar Thread!\n"));
			return 0;
		}


		HANDLE ghEvents[1];
		ghEvents[0] = hThreadComandos;
		WaitForMultipleObjects(1, ghEvents, FALSE, INFINITE);
	}

	return 0;
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
	DADOS* dados = ((DADOS*)param);
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
		}
		//INSERIR OBSTACULO
		else if (!_tcscmp(op, TEXT("obstaculo"))) {
			do {
				_tprintf(_T("\n[COMANDO] Introduza a pista: "));
				_tscanf_s(_T("%d"), &pista);
			} while (pista < 0);
		}
		//INVERTE SENTIDO DE RODAGEM DE X FAIXA
		else if (!_tcscmp(op, TEXT("inverte"))) {

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

	dados->TERMINAR = 1;

	ReleaseMutex(hMutex);

	Sleep(1000);

	ExitThread(0);
}