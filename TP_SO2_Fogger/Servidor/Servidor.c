#include "Servidor.h"

void show(TCHAR** board, int numFaixas, int cols){
_tprintf(TEXT("\n"));

    for (int i = 0; i < (numFaixas + 2); i++) {
        _tprintf(TEXT("\n"));

        for (int j = 0; j < cols; j++) {
            _tprintf(TEXT("%c"), board[i][j]);
        }
    }
}
BOOL putSapo(TCHAR** board, int numFaixas, int cols) {
    int pos;
    BOOL foundPlace = FALSE,temEspaco=FALSE;
    for (int i = 0;i < cols;i++) {
        if (board[numFaixas + 1][i] != TEXT('S'))
            temEspaco = TRUE;

    }
    if (!temEspaco) return FALSE;

    while (!foundPlace) {
        pos = rand() % cols;
        if (board[numFaixas + 1][pos] != TEXT('S')) {
            board[numFaixas + 1][pos] = TEXT('S');
            foundPlace = TRUE;
        }
        

    }
    return TRUE;
}
TCHAR** createBoard(int numFaixas,int cols) {
    TCHAR** areaJogo = (TCHAR**)malloc((numFaixas + 2) * sizeof(TCHAR*));
    for (int i = 0; i < (numFaixas + 2); i++) {
        areaJogo[i] = (TCHAR*)malloc(cols * sizeof(TCHAR));
    }

    for (int i = 0; i < (numFaixas + 2); i++) {

        for (int j = 0; j < cols; j++) {
            areaJogo[i][j] = TEXT(' ');
        }
    }
    for (int j = 0; j < cols; j++) {
        areaJogo[0][j] = TEXT('_');
    }
    for (int j = 0; j < cols; j++) {
        areaJogo[numFaixas + 1][j] = TEXT('-');
    }
    return areaJogo;

}


DWORD WINAPI ThreadRow(LPVOID param) {
    data* dados = (data*)param;

    WaitForSingleObject(dados->hEvent, INFINITE);

    _tprintf(TEXT("\n[Sou a thread %d]"), dados->faixaNumero);
    switch (dados->faixaNumero){
    case 1:
        dados->board[dados->faixaNumero][10] = TEXT('<');
        dados->board[dados->faixaNumero][2] = TEXT('<');
        break;
    case 2:
        dados->board[dados->faixaNumero][15] = TEXT('>');
        dados->board[dados->faixaNumero][6] = TEXT('>');
        break;
    case 3:
        dados->board[dados->faixaNumero][11] = TEXT('<');
        dados->board[dados->faixaNumero][3] = TEXT('<');
        break;
    case 4:
        dados->board[dados->faixaNumero][19] = TEXT('>');
        dados->board[dados->faixaNumero][4] = TEXT('>');
        break;
    case 5:
        dados->board[dados->faixaNumero][16] = TEXT('<');
        dados->board[dados->faixaNumero][8] = TEXT('<');
        break;
    case 6:
        dados->board[dados->faixaNumero][9] = TEXT('>');
        dados->board[dados->faixaNumero][1] = TEXT('>');
        break;
    case 7:
        dados->board[dados->faixaNumero][12] = TEXT('<');
        dados->board[dados->faixaNumero][3] = TEXT('<');
        break;
    case 8:
        dados->board[dados->faixaNumero][10] = TEXT('>');
        dados->board[dados->faixaNumero][2] = TEXT('>');
        dados->board[dados->faixaNumero][17] = TEXT('>');
        break;
    }
    do {
        for (int i = 0; i < dados->cols; i++){
            //WaitForSingleObject(dados->hMutex, INFINITE);

            
            if (dados->board[dados->faixaNumero][i] == TEXT('<')) {
                dados->board[dados->faixaNumero][i] = TEXT(' ');
                if (i == 0)   dados->board[dados->faixaNumero][dados->cols - 1] = TEXT('<');
                else   dados->board[dados->faixaNumero][i - 1] = TEXT('<');

                


            }

            //ReleaseMutex(dados->hMutex);
        }       

        for (int l = dados->cols; l >= 0; l--) {

            if (dados->board[dados->faixaNumero][l] == TEXT('>')) {
                dados->board[dados->faixaNumero][l] = TEXT(' ');
                if (l == dados->cols - 1)   dados->board[dados->faixaNumero][0] = TEXT('>');
                else   dados->board[dados->faixaNumero][l + 1] = TEXT('>');



            }
        }
        Sleep(dados->faixaVelocidade);
        



    } while (1);
    

    

    

    
    return 0;

}

int _tmain(int argc, TCHAR* argv[]) {
    data dados[MAXFAIXAS];
    HKEY chave;
    DWORD resultado;
    int varAdd = 0, numFaixas;
    HANDLE hOneServer,hMutex;
    HANDLE hGlobalEvent;
    int cols = 20, velocidade,rows;
    TCHAR** areaJogo;
    TCHAR chave_nome[TAM] = TEXT("SOFTWARE\\Frogger\\chave_TP");
    TCHAR par_nome[TAM] = TEXT("velocidade");
    DWORD par_valor;
    HANDLE hRowThread[MAXFAIXAS]; 




#ifdef UNICODE 
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif



    hOneServer = CreateMutex(NULL, TRUE, _T("OneServer"));
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        printf("Ja existe uma instancia do servidor em execucao.\n");
        return 0;
    }


    if (argc < 3 ) {
        if (RegOpenKeyEx(HKEY_CURRENT_USER, chave_nome, 0, KEY_ALL_ACCESS, &chave) != ERROR_SUCCESS) {
            _tprintf(TEXT("Coloque argumentos"));
            return -1;
        }


        DWORD tamanho = sizeof(velocidade);
        if (RegQueryValueEx(chave, TEXT("velocidade"), NULL, NULL, (LPBYTE)&velocidade, &tamanho) == ERROR_SUCCESS)
            _tprintf(TEXT("Velocidade:%d\n"), velocidade);
        else
            _tprintf(TEXT("Erro ao recuperar o valor da chave\n"));

        tamanho = sizeof(numFaixas);
        if (RegQueryValueEx(chave, TEXT("faixas"), NULL, NULL, (LPBYTE)&numFaixas, &tamanho) == ERROR_SUCCESS)
            _tprintf(TEXT("Num de faixas:%d\n"), numFaixas);
        else
            _tprintf(TEXT("Erro ao recuperar o valor da chave\n"));




    }else {
        numFaixas = _ttoi(argv[1]);// <faixas> <velocidade>
        velocidade = _ttoi(argv[2]);
        _tprintf(TEXT("\n numFaixas=%d,velocidade=%d "), numFaixas, velocidade);

    }

    rows = numFaixas + 2;
    

    hGlobalEvent= CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hGlobalEvent == NULL) {
        _tprintf(TEXT("Erro a criar o evento\n"));
        return -1;
    }
    hMutex = CreateMutex(NULL, FALSE, NULL,"abc");
    if (hMutex == NULL) {
        _tprintf(TEXT("Erro a criar mutex.\n"));
        return 1;
    }

    areaJogo = createBoard(numFaixas, cols);
    
    if(!putSapo(areaJogo, numFaixas, cols)) _tprintf(TEXT("\nNao ha espaço "));
    if (!putSapo(areaJogo, numFaixas, cols)) _tprintf(TEXT("\nNao ha espaço "));

    for (int i = 0;i < numFaixas;i++) {
        dados[i].hMutex = hMutex;
        dados[i].faixaVelocidade = velocidade;
        dados[i].hEvent = hGlobalEvent;
        dados[i].board = areaJogo;
        dados[i].rows = rows;
        dados[i].cols = cols;
        dados[i].faixaNumero = i+1;

        hRowThread[i] = CreateThread(NULL, 0, ThreadRow, &dados[i], 0, NULL);
    }
    Sleep(1000);
    SetEvent(hGlobalEvent);
    
    do {
        //WaitForSingleObject(hMutex, INFINITE);
        system("cls");
        show(areaJogo, numFaixas, cols);
        //ReleaseMutex(hMutex);
        Sleep(velocidade);
        
    } while (1);








    //fechar handlers e memoria dinamica
    for (int i = 0;i < numFaixas;i++)
        CloseHandle(hRowThread[i]);

    for (int i = 0; i < (numFaixas + 2); i++) {
        free(areaJogo[i]);
    }
    free(areaJogo);
    
    ReleaseMutex(hOneServer);
    CloseHandle(hOneServer);
    return 0;
}