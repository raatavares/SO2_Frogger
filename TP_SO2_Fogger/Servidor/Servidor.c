#include "Servidor.h"
#include "..\\DLL\Header.h"

int PAUSE = 0;

int gerarAleatorio(int minimo, int maximo) {
    int intervalo = maximo - minimo + 1;
    return rand() % intervalo + minimo;
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

DWORD WINAPI ThreadMapToUser(LPVOID param) {
    data* dados = (data*)param;
    matriz dadosPassados;
    HANDLE hPipe[2];
    dadosPassados.terminar = 1;
    dadosPassados.rows = dados->rows;
    dadosPassados.cols = dados->cols;
    for (int i = 0; i < dados->rows; i++) {
        for (int j = 0; j < dados->cols; j++) {
            dadosPassados.board[i][j] = dados->board[i][j];
        }
    }
    if (!_tcscmp(dados->command, TEXT("sair"))) {
        dadosPassados.terminar = 0;
        
    }


    hPipe[0] = CreateNamedPipe(sendMapTo_S_Pipe, PIPE_ACCESS_OUTBOUND, PIPE_WAIT |
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 1,
        sizeof(matriz), sizeof(matriz), 1000, NULL);
    if (hPipe == INVALID_HANDLE_VALUE) {
        _tprintf(TEXT("[ERRO] Criar Named Pipe! (CreateNamedPipe)"));
        exit(-1);
    }

    _tprintf(TEXT("Aguardando conexão do cliente...\n"));

    if (!ConnectNamedPipe(hPipe[0], NULL)) {
        _tprintf(TEXT("[ERRO] Ligação ao leitor! (ConnectNamedPipe)\n"));
        exit(-1);
    }

    if (dados->modo==1){//multiplayer
        hPipe[1] = CreateNamedPipe(sendMapTo_s_Pipe, PIPE_ACCESS_OUTBOUND, PIPE_WAIT |
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 1,
            sizeof(matriz), sizeof(matriz), 1000, NULL);
        if (hPipe == INVALID_HANDLE_VALUE) {
            _tprintf(TEXT("[ERRO] Criar Named Pipe! (CreateNamedPipe)"));
            exit(-1);
        }

        _tprintf(TEXT("Aguardando conexão do cliente...\n"));

        if (!ConnectNamedPipe(hPipe[1], NULL)) {
            _tprintf(TEXT("[ERRO] Ligação ao leitor! (ConnectNamedPipe)\n"));
            exit(-1);
        }

    }


    DWORD bytesWrite;
    do
    {
        _tprintf(TEXT("[ERRO]Código de erro: %lu\n"), GetLastError());
        if (WriteFile(hPipe[0], (LPVOID)&dados, sizeof(matriz), &bytesWrite, NULL)) {
                                                                                        //cliente 1
            _tprintf(TEXT("[ERRO]Código de erro: %lu\n"), GetLastError());
            exit(1);
        }
        if (dados->modo == 1) {//entra se multiplayer
            if (WriteFile(hPipe[1], (LPVOID)&dados, sizeof(matriz), &bytesWrite, NULL)) {
                                                                                        //cliente 2
                _tprintf(TEXT("[ERRO]Código de erro: %lu\n"), GetLastError());
                exit(1);
            }
        }
        Sleep(500);
    } while (_tcscmp(dados->command, TEXT("sair")));



    ExitThread(0);

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

void reinicializaBoard(TCHAR** areaJogo,int numFaixas, int cols) {
    for (int i = 1; i < numFaixas-1 ; i++) {

        for (int j = 0; j < cols; j++) {
            areaJogo[i][j] = TEXT(' ');
        }
    }
    areaJogo[1][10] = TEXT('<');
    areaJogo[1][2] = TEXT('<');
    areaJogo[2][15] = TEXT('>');
    areaJogo[2][6] = TEXT('>');
    areaJogo[3][11] = TEXT('<');
    areaJogo[3][3] = TEXT('<');
    areaJogo[4][19] = TEXT('>');
    areaJogo[4][4] = TEXT('>');
    areaJogo[5][16] = TEXT('<');
    areaJogo[5][8] = TEXT('<');
    areaJogo[6][9] = TEXT('>');
    areaJogo[6][1] = TEXT('>');
    areaJogo[7][12] = TEXT('<');
    areaJogo[7][3] = TEXT('<');
    areaJogo[8][10] = TEXT('>');
    areaJogo[8][2] = TEXT('>');
    areaJogo[8][17] = TEXT('>');
}

DWORD WINAPI ThreadUI(LPVOID param) {
    data* dados = (data*)param;
    TCHAR i;
    TCHAR* command;
    command = dados->command;

    do {
        i = _gettch();
        _tprintf(_T("%c"), i);
        command[0] = i;
        _fgetts(&command[1], sizeof(command), stdin);
        command[_tcslen(command) - 1] = TEXT('\0');
        _tprintf(_T("%s"), command);
        
        WaitForSingleObject(dados->hMutex, INFINITE);
        if (!_tcscmp(dados->command, TEXT("restart"))) { reinicializaBoard(dados->board, dados->rows, dados->cols);dados->command[0] = '\0'; }
        ReleaseMutex(dados->hMutex);

    } while (_tcscmp(dados->command, TEXT("sair")));

    ExitThread(0);
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
        if (_tcscmp(dados->command, TEXT("para"))) {
            PAUSE = 0;
        }else {
            continue;
        }

        WaitForSingleObject(dados->hMutex, INFINITE);
        for (int i = 0; i < dados->cols; i++){
            //WaitForSingleObject(dados->hMutex, INFINITE);

            if (dados->board[dados->faixaNumero][i] == TEXT('<')) {
                if (dados->board[dados->faixaNumero][i-1] == TEXT('X')|| dados->board[dados->faixaNumero][i-1] == TEXT('<')) continue;
                dados->board[dados->faixaNumero][i] = TEXT(' ');
                if (i == 0)   dados->board[dados->faixaNumero][dados->cols - 1] = TEXT('<');
                else   dados->board[dados->faixaNumero][i - 1] = TEXT('<');
            }
            //ReleaseMutex(dados->hMutex);
        }       

        for (int l = dados->cols; l >= 0; l--) {

            if (dados->board[dados->faixaNumero][l] == TEXT('>')) {
                if (dados->board[dados->faixaNumero][l + 1] == TEXT('X') || dados->board[dados->faixaNumero][l + 1] == TEXT('>')) continue;

                dados->board[dados->faixaNumero][l] = TEXT(' ');
                if (l == dados->cols - 1)   dados->board[dados->faixaNumero][0] = TEXT('>');
                else   dados->board[dados->faixaNumero][l + 1] = TEXT('>');

            }
        }
        ReleaseMutex(dados->hMutex);

        Sleep(dados->faixaVelocidade);


    } while (_tcscmp(dados->command, TEXT("sair")));
    
    ExitThread(0);
    return 0;
}
DWORD WINAPI ThreadMapping(LPVOID param) {
    int fim;
    mapping* dados = (mapping*)param;
    matriz dadosPassados;
    TCHAR limpeza[20];

        
    do {
        WaitForSingleObject(dados->hMutex, INFINITE);
        
        ZeroMemory(dados->board, sizeof(matriz));
        dadosPassados.terminar = 1;
        dadosPassados.rows = dados->jogo->rows;
        dadosPassados.cols = dados->jogo->cols;
        for (int i = 0; i < dados->jogo->rows; i++) {
            for (int j = 0; j < dados->jogo->cols; j++) {
                dadosPassados.board[i][j] = dados->jogo->board[i][j];
            }
        }
        if (!_tcscmp(dados->command, TEXT("sair"))) {
            dadosPassados.terminar = 0;
            dados->TERMINAR = 0;
        }
        
        CopyMemory(dados->board, &dadosPassados,sizeof(matriz));
        fim = dados->TERMINAR;
        ReleaseMutex(dados->hMutex);

        if (PAUSE == 0) {
            SetEvent(dados->sharedMapEvent);
            Sleep(500);
            ResetEvent(dados->sharedMapEvent);
        }


        SetEvent(dados->hEvent);
        Sleep(500);

        ResetEvent(dados->hEvent); 
        if (_tcscmp(dados->command, TEXT("sair"))){
            ;
        }
        
    } while (_tcscmp(dados->command, TEXT("sair")));
    

    dadosPassados.terminar = 0;
    CopyMemory(dados->board, &dadosPassados, sizeof(matriz));
    ExitThread(0);

}

DWORD WINAPI ThreadBuffer(LPVOID param, LPVOID mem) {
    data* dados = (data*)param;
    mapping* pDados = (mapping*)mem;


    pedido pedidos;
    buffer_circular* bufferData;
    HANDLE hMutexBuffer;
    HANDLE hThreadCountdown;

    hMutexBuffer = CreateMutex(NULL, FALSE, TEXT("TP_MUTEX_CONSUMIDOR"));
    if (hMutexBuffer == NULL) {
        _tprintf(TEXT("Erro no CreateMutex\n"));
        return -1;
    }

    HANDLE hFileBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(buffer_circular), TEXT("TP_BufferCircular"));

    if (hFileBuffer == NULL) {
        _tprintf(TEXT("Erro no CreateFileMapping\n"));
        return;
    }
    bufferData = (buffer_circular*)MapViewOfFile(hFileBuffer, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (bufferData == NULL) {
        _tprintf(TEXT("Erro no MapViewOfFile\n"));
        return;
    }
    hSemEscrita = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, TEXT("TP_SEMAFORO_ESCRITA"));
    if (hSemEscrita == NULL) {
        _tprintf(TEXT("Erro no CreateSemaphore\n"));
        return;
    }
    hSemLeitura = CreateSemaphore(NULL, 0, BUFFER_SIZE, TEXT("TP_SEMAFORO_LEITURA"));
    if (hSemLeitura == NULL) {
        _tprintf(TEXT("Erro no CreateSemaphore\n"));
        return;
    }
    bufferData->posE = 0;
    bufferData->posL = 0;
    
    while (1) {
        


        WaitForSingleObject(hSemLeitura, INFINITE);
        //WaitForSingleObject(hMutexBuffer, INFINITE);

        leituraBufferCircular(bufferData, &pedidos);

        //_tprintf(TEXT("\n(%d)%d %d\n"),bufferData->posL, pedidos.paraMovimento, pedidos.segundosParar);
        if (pedidos.paraMovimento){
            WaitForSingleObject(dados->hMutex,pedidos.segundosParar);
            PAUSE = 1;
            _tprintf(TEXT("\n(%d)Movimento parado por %ds\n"), bufferData->posL, pedidos.segundosParar);
            Sleep(pedidos.segundosParar * 1000);
            PAUSE = 0;
            bufferData->posL++;
            ReleaseMutex(dados->hMutex);
        }
        else if (pedidos.inverteSentido) {
            _tprintf(TEXT("\n(%d)Inverte sentido da pista %d\n"), bufferData->posL, pedidos.pistaInverter);
            WaitForSingleObject(dados->hMutex, pedidos.segundosParar);
            for (int i = 0; i < dados[pedidos.pistaInverter].cols; i++)
            {
                if (dados[pedidos.pistaInverter].board[pedidos.pistaInverter][i] == TEXT('<')) {
                    dados->board[pedidos.pistaInverter][i] = TEXT('>');

                }else if(dados[pedidos.pistaInverter].board[pedidos.pistaInverter][i] == TEXT('>')){
                    dados->board[pedidos.pistaInverter][i] = TEXT('<');
                }

            }
            bufferData->posL++;
            ReleaseMutex(dados->hMutex);

        }
        else if (pedidos.insereObstaculo) {
            int x, y;
            _tprintf(TEXT("\n(%d)Obstáculo inserido\n"), bufferData->posL);
            WaitForSingleObject(dados->hMutex, pedidos.segundosParar);
            do {
                x = gerarAleatorio(1, dados->rows - 2);
                y = gerarAleatorio(3, dados->cols - 4);
            } while (dados->board[x][y] != TEXT(' '));

            dados->board[x][y] = TEXT('X');

            bufferData->posL++;
            ReleaseMutex(dados->hMutex);

        }

        if (bufferData->posL == BUFFER_SIZE)
            bufferData->posL = 0;
        
        //ReleaseMutex(hMutexBuffer);
        ReleaseSemaphore(hSemEscrita, 1, NULL);
        
        Sleep(1000);
    } 
   
    ExitThread(0);
}


int _tmain(int argc, TCHAR* argv[]) {
    int TERMINAR=0;
    int Mode;//0-singleplayer   1-multiplayer
    mapping pDados;
    data dados[MAXFAIXAS];
    HKEY chave;
    DWORD resultado;
    int varAdd = 0, numFaixas;
    HANDLE hOneServer, hMutex;//MUTEXES
    HANDLE hGlobalEvent;
    int cols = 20, velocidade,rows;
    TCHAR** areaJogo;
    TCHAR chave_nome[TAM] = TEXT("SOFTWARE\\Frogger\\chave_TP");
    TCHAR par_nome[TAM] = TEXT("SOFTWARE\\Frogger\\velocidade");
    TCHAR command[TAM];
    DWORD par_valor;
    HANDLE hRowThread[MAXFAIXAS], hUIThread, hMapThread, hBufferThread, hCriaSaposThread, hThreadMapToUsers;// THREADS
    HANDLE hPipe;//PIPES


    pipe_user_server userServerData;
    

#ifdef UNICODE 
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif



    hOneServer = CreateMutex(NULL, TRUE, _T("OneServer"));
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        printf("Ja existe uma instancia do servidor em execução.\n");
        return 0;
    }
    DWORD tipo;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, chave_nome, 0,NULL,REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,NULL, &chave,&tipo) != ERROR_SUCCESS) {
        _tprintf(TEXT("Coloque argumentos"));
        return -1;
    }

    DWORD tamanho = sizeof(velocidade);
    if (argc < 3 ) {
        
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
        DWORD Velocidade = velocidade;
        _tprintf(TEXT("\n numFaixas=%d,velocidade=%d "), numFaixas, velocidade);

        if (RegSetValueEx(chave, TEXT("velocidade"), 0, REG_DWORD, (BYTE*)&Velocidade, sizeof(DWORD)) != ERROR_SUCCESS) {

            RegCloseKey(chave);
            return 1;
        }
        DWORD NumFaixas = numFaixas;

        if (RegSetValueEx(chave, TEXT("faixas"), 0, REG_DWORD, (BYTE*)&NumFaixas, sizeof(DWORD)) != ERROR_SUCCESS) {

            RegCloseKey(chave);
            return 1;
        }

    }

    rows = numFaixas + 2;

    //----- mapping -----
    pDados.hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,sizeof(mapping),TEXT("TP_MEM_PART"));
    if (pDados.hFileMap == NULL) {
        _tprintf(TEXT("Erro no CreateFileMapping\n"));
        return 1;
    }
     pDados.board= (matriz*)MapViewOfFile(pDados.hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(pDados.board));
     pDados.jogo = &dados[0];
    if (pDados.board == NULL) {
        _tprintf(TEXT("Erro no MapViewOfFile\n"));
        CloseHandle(pDados.hFileMap);
        return 1;
    }
    pDados.command = command;

    pDados.hEvent = CreateEvent(
        NULL,
        TRUE,
        FALSE,
        TEXT("Map_EVENTO"));

    if (pDados.hEvent == NULL) {
        _tprintf(TEXT("Erro no CreateEvent\n"));
        UnmapViewOfFile(pDados.board);
        return 1;
    }

    pDados.sharedMapEvent = CreateEvent(
        NULL,
        TRUE,
        FALSE,
        TEXT("ATUALIZAMAP_EVENTO"));

    if (pDados.sharedMapEvent == NULL) {
        _tprintf(TEXT("Erro no CreateEvent\n"));
        return 1;
    }
    SetEvent(pDados.sharedMapEvent);



    pDados.hMutex = CreateMutex(
        NULL,
        FALSE,
        TEXT("Map_MUTEX"));

    if (pDados.hMutex == NULL) {
        _tprintf(TEXT("Erro no CreateMutex\n"));
        UnmapViewOfFile(pDados.board);
        return 1;
    }
    areaJogo = createBoard(numFaixas, cols);
    //------- sapos --------
    userServerData.jogo = pDados.jogo;


    hPipe = CreateNamedPipe(starterPipe, PIPE_ACCESS_DUPLEX, PIPE_WAIT |
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 1,
        sizeof(player), sizeof(player), 1000, NULL);
    if (hPipe == INVALID_HANDLE_VALUE) {
        _tprintf(TEXT("[ERRO] Criar Named Pipe! (CreateNamedPipe)"));
        exit(-1);
    }

    _tprintf(TEXT("Aguardando conexão do cliente...\n"));

    if (!ConnectNamedPipe(hPipe, NULL)) {
        _tprintf(TEXT("[ERRO] Ligação ao leitor! (ConnectNamedPipe)\n"));
        exit(-1);
    }


    _tprintf(TEXT("Cliente 1 conectado!\n"));
    userServerData.players[0].mode = 3;
    
    _tprintf(TEXT("Modo inicial: %d\n"), &userServerData.players[0].mode);
    DWORD bytesRead;
    if (ReadFile(hPipe, (LPVOID)&userServerData.players[0], sizeof(player), &bytesRead, NULL)) {
        _tprintf(TEXT("Modo escolhido pelo cliente: %d\n"), userServerData.players[0].mode);
    }
    else {
        _tprintf(TEXT("Falha ao receber o inteiro. Código de erro: %lu\n"), GetLastError());
        exit(1);
    }
    Sleep(1000);
    userServerData.players[0].player_char = _T('S');
    //userServerData.players[1].mode = userServerData.players[0].mode;

    DWORD bytesWrite;
    if (WriteFile(hPipe, (LPVOID)&userServerData.players[0], sizeof(player), &bytesWrite, NULL)) {
        _tprintf(TEXT("Cliente atribuido com: 'S'\n"));
    }
    else {
        _tprintf(TEXT("[ERRO]Código de erro: %lu\n"), GetLastError());
        exit(1);
    }

    Sleep(1000);
    if (userServerData.players[0].mode != 0)
    {
        if (!ConnectNamedPipe(hPipe, NULL)) {
            _tprintf(TEXT("[ERRO] Ligação ao leitor! (ConnectNamedPipe)\n"));
            exit(-1);
        }

        _tprintf(TEXT("Cliente 2 conectado!\n"));

        DWORD bytesRead;
        if (ReadFile(hPipe, &userServerData.players[1], sizeof(player), &bytesRead, NULL)) {
            _tprintf(TEXT("Modo escolhido pelo cliente2: %d\n"), userServerData.players[1].mode);
        }
        else {
            _tprintf(TEXT("Falha ao receber o inteiro. Código de erro: %lu\n"), GetLastError());
            exit(1);
        }

        userServerData.players[1].player_char = _T("s");
        userServerData.players[1].mode = userServerData.players[0].mode;

        if (WriteFile(hPipe, &userServerData.players[0], sizeof(player), &bytesWrite, NULL)) {
            _tprintf(TEXT("Cliente atribuido com: 's'\n"));
        }
        else {
            _tprintf(TEXT("[ERRO]Código de erro: %lu\n"), GetLastError());
            exit(1);
        }

    }//ja estao os dois/um com as informaçoes criadas basta agora criar os pipes individuais
    dados[0].modo = userServerData.players[0].mode;
    hThreadMapToUsers = CreateThread(NULL, 0, ThreadMapToUser, &dados[0], 0, NULL);
    


    //---------------------------
    
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

    
    
    if(!putSapo(areaJogo, numFaixas, cols)) _tprintf(TEXT("\nNão há espaço "));
    if (!putSapo(areaJogo, numFaixas, cols)) _tprintf(TEXT("\nNão há espaço "));

    for (int i = 0;i < numFaixas;i++) {
        dados[i].command = command;
        dados[i].hMutex = hMutex;
        dados[i].faixaVelocidade = velocidade;
        dados[i].hEvent = hGlobalEvent;
        dados[i].board = areaJogo;
        dados[i].rows = rows;
        dados[i].cols = cols;
        dados[i].faixaNumero = i+1;
        dados[i].TERMINAR = TERMINAR;
        hRowThread[i] = CreateThread(NULL, 0, ThreadRow, &dados[i], 0, NULL);
    }
    Sleep(1000);
    SetEvent(hGlobalEvent);

    hUIThread = CreateThread(NULL, 0, ThreadUI, &dados, 0, NULL);
    pDados.TERMINAR = TERMINAR;
    hMapThread = CreateThread(NULL, 0, ThreadMapping, &pDados, 0, NULL);
    //----- BUFFER ------


    hBufferThread = CreateThread(NULL, 0, ThreadBuffer, &dados, &pDados, 0, NULL);
    if (hBufferThread == NULL) {
        _tprintf(TEXT("Problema com threadBuffer ...\n"));

    }

    int fim;

    do {

        //WaitForSingleObject(hMutex, INFINITE);
        system("cls");
        imprimeMapa(&pDados);
        if (!_tcscmp(dados->command, TEXT("para"))) {
            PAUSE = 1;
            _tprintf(TEXT("\nPressione uma tecla para retomar jogo:\n"));
        }
        else    _tprintf(TEXT("\nEscreva comando:\n"));
        WaitForSingleObject(dados->hMutex, INFINITE);
        if (!_tcscmp(dados->command, TEXT("sair"))) {
            _tprintf(TEXT("\nSaindo..."));
            TERMINAR = 1;
        }
        fim = TERMINAR;
        ReleaseMutex(dados->hMutex);
        
        Sleep(100);
        
    } while (!fim);
    
    TerminateThread(hBufferThread, 0);

    WaitForMultipleObjects(numFaixas, hRowThread, FALSE, INFINITE);
    WaitForSingleObject(hUIThread, INFINITE);
    WaitForSingleObject(hMapThread, INFINITE);
    WaitForSingleObject(hBufferThread, INFINITE);
    WaitForSingleObject(hThreadMapToUsers, INFINITE);

    //fechar handlers e memoria dinamica
    UnmapViewOfFile(pDados.board);
    for (int i = 0;i < numFaixas;i++)
        CloseHandle(hRowThread[i]);
    CloseHandle(hUIThread);
    CloseHandle(hMapThread);
    CloseHandle(hBufferThread);
    CloseHandle(hThreadMapToUsers);


    for (int i = 0; i < (numFaixas + 2); i++) {
        free(areaJogo[i]);
    }
    free(areaJogo);
    
    ReleaseMutex(hOneServer);
    CloseHandle(pDados.hFileMap);
    CloseHandle(hMutex);
    CloseHandle(hOneServer);
    return 0;
}