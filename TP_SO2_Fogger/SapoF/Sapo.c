#include "framework.h"
#include "Sapo.h"
#include "..\\DLL\Header.h"
#include <new.h>

#define MAX_LOADSTRING 100
#define SAIR_BUTTON 0
#define TESTE_BUTTON 10
#define INDIVIDUAL_BUTTOM 1
#define COMPETICAO_BUTTOM 2
#define ABOUT_BUTTON 3

//DADOS dados;
player jogador;
pipe_user_server dadosMap;
matriz* board;

matriz matrizMapa;

// Variáveis Globais:
HINSTANCE hInst;                                // instância atual
WCHAR szTitle[MAX_LOADSTRING];                  // O texto da barra de título
WCHAR szWindowClass[MAX_LOADSTRING];            // o nome da classe da janela principal

// Declarações de encaminhamento de funções incluídas nesse módulo de código:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void AddMenu(HWND);
void registerDialogClass(HINSTANCE);
void displayDialog(HWND);
HWND hName, hMainWindow, hSecondWindow;
HMENU hMenu;
TCHAR nomeS[50];
TCHAR caracterUnic;
HANDLE hThreadAtualizaMapa, hThreadTime;


BITMAP bmp;
HDC bmpDC;
HDC* memDC;

//Icones
HBITMAP hParede, hSapo, hCarro, hPedra;
HDC hdcParede, hdcSapo, hdcCarro, hdcPedra;

// Função para a thread que decrementa o tempo
int timeconta = 20;
DWORD WINAPI TimeThread(LPVOID param) {

    while (1)
    {
        Sleep(1000); // Aguarda 1 segundo

        timeconta--; // Decrementa o tempo

        if (timeconta == 0)
        {
            // Tempo chegou a zero, exibe a mensagem "Game Over"
            MessageBox(NULL, TEXT("Game Over"), TEXT("Tempo esgotado"), MB_OK);
            break;
        }
    }

    ExitThread(0);
}


DWORD WINAPI ThreadAtualizaMapa(LPVOID param) {
    //matriz* matrizMap = ((matriz*)param);
    DWORD n;
    board = (matriz*)malloc(sizeof(matriz));
    int teste = 0;
    HANDLE hPipe1;


    matrizMapa.hMapaLidoEvent = CreateEvent(
        NULL,
        TRUE,
        FALSE,
        TEXT("ATUALIZAMAP_EVENTO_USER"));

    if (matrizMapa.hMapaLidoEvent == NULL) {
        MessageBox(NULL, _T("[ERRO] Erro no CreateEvent (hMapaLidoEvent)"), _T("Erro"), MB_ICONERROR | MB_OK);
        return 1;
    }
    //SetEvent(matrizMapa.hMapaLidoEvent);
    //ResetEvent(matrizMapa.hMapaLidoEvent);

    if (!WaitNamedPipe(sendMapTo_S_Pipe, NMPWAIT_WAIT_FOREVER)) {
        _tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (WaitNamedPipe)\n"), sendMapTo_S_Pipe);
        return 0;
    }
    _tprintf(TEXT("[ConTaxi] Ligação ao pipe da Central...\n"));
    hPipe1 = CreateFile(sendMapTo_S_Pipe, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hPipe1 == NULL) {
        _tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (CreateFile)\n"), sendMapTo_S_Pipe);
        return 0;
    }

    _tprintf(TEXT("Criou pipe conexão do cliente...\n"));


    while (1) {
        WaitForSingleObject(hMutexPipe, INFINITE);

        WaitForSingleObject(board->hMapaLidoEvent, INFINITE);

        if (!IsWindow(hSecondWindow)) {
            // A janela foi destruída, faça algo aqui
            if (!ReadFile(hPipe1, (LPVOID)board, sizeof(matriz), &n, NULL)) {
                MessageBox(NULL, _T("[ERRO] Erro na leitura (matriz)"), _T("Erro"), MB_ICONERROR | MB_OK);
                exit(-2);
            }

        }

        InvalidateRect(hMainWindow, NULL, FALSE);
        ReleaseMutex(hMutexPipe);
       // UpdateWindow(hMainWindow);
        Sleep(100);
    }
    Sleep(500);

    ExitThread(0);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Inicializar cadeias de caracteres globais
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SAPOF, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    registerDialogClass(hInst);
    // Realize a inicialização do aplicativo:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    //================================================================================================

    //hMapaLidoEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    

    //==================================================================================================================

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SAPOF));

    MSG msg;

    // Loop de mensagem principal:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Fechar o Named Pipe antes de sair
    CloseHandle(hPipe);
    return (int) msg.wParam;
}



//
//  FUNÇÃO: MyRegisterClass()
//
//  FINALIDADE: Registra a classe de janela.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SAPOF));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SAPOF);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNÇÃO: InitInstance(HINSTANCE, int)
//
//   FINALIDADE: Salva o identificador de instância e cria a janela principal
//
//   COMENTÁRIOS:
//
//        Nesta função, o identificador de instâncias é salvo em uma variável global e
//        crie e exiba a janela do programa principal.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Armazenar o identificador de instância em nossa variável global

   hMainWindow = CreateWindowW(szWindowClass, TEXT("Sapo"), WS_OVERLAPPEDWINDOW,
      450, 0, 700, 600, NULL, NULL, hInstance, NULL);

   if (!hMainWindow)
   {
      return FALSE;
   }

   ShowWindow(hMainWindow, nCmdShow);
   UpdateWindow(hMainWindow);

   return TRUE;
}

//
//  FUNÇÃO: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  FINALIDADE: Processa as mensagens para a janela principal.
//
//  WM_COMMAND  - processar o menu do aplicativo
//  WM_PAINT    - Pintar a janela principal
//  WM_DESTROY  - postar uma mensagem de saída e retornar
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int value;
    //matriz matrizMapa;

    HDC hdc;
    PAINTSTRUCT ps;
    HFONT hFont;
    HWND hText;

    RECT rect;
    static HDC bmpDC;
    HBITMAP hBmp;
    static BITMAP bmp;
    static HDC backBufferDC = NULL;
    static HBITMAP hBackBuffer = NULL;

    static HDC memDC = NULL;
    TCHAR caract;
    int xPos = 0, yPos = 0;

    // Defina a fonte para o texto
    hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        TEXT("Arial"));

    switch (message)
    {
    case WM_COMMAND:
        switch (HIWORD(wParam)) /* command kind */
        {
        case 0: // menus
            switch (LOWORD(wParam)) /* child ID */
            {
            case TESTE_BUTTON:
                MessageBeep(MB_OK);
                break;
            case SAIR_BUTTON:
                value = MessageBox(hWnd, TEXT("Tem a certeza que deseja sair?"), TEXT("Confirmação"), MB_ICONQUESTION | MB_YESNO);
                if (value == IDYES)
                {
                    DestroyWindow(hWnd);
                }
                break;
            case ABOUT_BUTTON: {
                MessageBox(hWnd, TEXT("Este trabalho é realizado por:\n   Daniel Bravo - 2021137795;\n   Ricardo Tavares - 2021144652;\nNo âmbito de Sistemas Operativos II - 2022/2023"), TEXT("Sobre"), MB_ICONINFORMATION | MB_OK);
                break;
            }
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        }
        break;
    case WM_CREATE: {
        AddMenu(hWnd);
        hdc = GetDC(hWnd);
        GetClientRect(hWnd, &rect);

        backBufferDC = CreateCompatibleDC(hdc);
        hBackBuffer = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        SelectObject(backBufferDC, hBackBuffer);

        hdcCarro = CreateCompatibleDC(hdc);
        hCarro = (HBITMAP)LoadImage(GetModuleHandle(NULL), TEXT("carro.bmp"), IMAGE_BITMAP, 40, 20, LR_LOADFROMFILE);
        SelectObject(hdcCarro, hCarro);

        hdcSapo = CreateCompatibleDC(hdc);
        hSapo = (HBITMAP)LoadImage(GetModuleHandle(NULL), TEXT("sapo.bmp"), IMAGE_BITMAP, 25, 20, LR_LOADFROMFILE);
        SelectObject(hdcSapo, hSapo);

        hdcPedra = CreateCompatibleDC(hdc);
        hPedra = (HBITMAP)LoadImage(GetModuleHandle(NULL), TEXT("pedra.bmp"), IMAGE_BITMAP, 25, 20, LR_LOADFROMFILE);
        SelectObject(hdcPedra, hPedra);

        hdcParede = CreateCompatibleDC(hdc);
        hParede = (HBITMAP)LoadImage(GetModuleHandle(NULL), TEXT("parede.bmp"), IMAGE_BITMAP, 25, 20, LR_LOADFROMFILE);
        SelectObject(hdcParede, hParede);

        ReleaseDC(hWnd, backBufferDC);

        displayDialog(hWnd);
        break;
    }
    case WM_CLOSE: {
        int value = MessageBox(hWnd, TEXT("Tem a certeza que deseja sair?"), TEXT("Confirmação"), MB_ICONQUESTION | MB_YESNO);

        if (value == IDYES)
        {
            DestroyWindow(hWnd);
        }

        break;
    }
    case WM_PAINT:
        {
        hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rect);

        if (backBufferDC == NULL)
        {
            backBufferDC = CreateCompatibleDC(hdc);
            hBackBuffer = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
            SelectObject(backBufferDC, hBackBuffer);
        }

        FillRect(backBufferDC, &rect, (HBRUSH)(WHITE_BRUSH));
        SetBkMode(backBufferDC, TRANSPARENT);

        // Defina a posição do texto no canto superior esquerdo
        int textX = 5; // Posição X do texto
        int textY = 5; // Posição Y do texto
        SelectObject(backBufferDC, hFont);
        SetTextColor(backBufferDC, RGB(0, 0, 0));
        SetBkColor(backBufferDC, RGB(255, 255, 255)); // Defina a cor de fundo como branco

        TextOut(backBufferDC, textX, textY, nomeS, lstrlen(nomeS));

            if (!IsWindow(hSecondWindow)) {      
                if (jogador.mode == 0) {
                    textX = textX + 600; // Posição X do texto
                    textY = textY + 450; // Posição Y do texto
                    TCHAR valorString[20]; // Definir um tamanho suficiente para a string
                    wsprintf(valorString, TEXT("Time: %d s"), timeconta);
                    TextOut(backBufferDC, textX, textY, valorString, lstrlen(valorString));
                }
                for (int i = 0; i < board->rows; i++) {
                    for (int j = 0; j < board->cols; j++) {
                        caract = board->board[i][j];
                        rect.left = 40 + (15 * xPos);
                        rect.top = 100 + (10 * yPos);
                        if (caract == '_' || caract == '-')
                            BitBlt(backBufferDC, rect.left, rect.top, 100, 100, hdcParede, 0, 0, SRCCOPY);
                        else if (caract == 'x')
                            BitBlt(backBufferDC, rect.left, rect.top, 100, 100, hdcPedra, 0, 0, SRCCOPY);
                        else if (caract == 'S')
                            BitBlt(backBufferDC, rect.left, rect.top, 100, 100, hdcSapo, 0, 0, SRCCOPY);
                        else if (caract == '<')
                            BitBlt(backBufferDC, rect.left, rect.top, 100, 100, hdcCarro, 0, 0, SRCCOPY);
                        else if (caract == '>')
                            StretchBlt(backBufferDC, rect.left, rect.top, -100, 100, hdcCarro, 0, 0, 100, 100, SRCCOPY);
                        xPos = xPos + 2;
                    }
                    yPos = yPos + 2;
                    xPos = 0;
                }
            }

            BitBlt(hdc, 0, 0, rect.right, rect.bottom, backBufferDC, 0, 0, SRCCOPY);

            EndPaint(hWnd, &ps);
            break;
        }
    case WM_KEYDOWN:
    {
        DWORD n;
        int keyCode = wParam;

        if (keyCode == VK_LEFT || keyCode == VK_RIGHT || keyCode == VK_UP || keyCode == VK_DOWN)
        {
            switch (keyCode)
            {
            case VK_LEFT:
                //MessageBox(hWnd, TEXT("Tecla esquerda pressionada"), TEXT("Tecla de Direção"), MB_OK);
                jogador.move = 'L';
                if (!WriteFile(hPipe, (LPVOID)&jogador, sizeof(jogador), &n, NULL)) {
                    MessageBox(NULL, _T("[ERRO] Erro na escrita (jogador.move)"), _T("Erro"), MB_ICONERROR | MB_OK);
                    exit(-4);
                }
                MessageBox(NULL, _T("[LEITOR] Escrita bem sucedida!"), _T("TECLA"), MB_ICONQUESTION | MB_OK);
                break;
            case VK_RIGHT:
                //MessageBox(hWnd, TEXT("Tecla direita pressionada"), TEXT("Tecla de Direção"), MB_OK);
                jogador.move = 'R';
                if (!WriteFile(hPipe, (LPVOID)&jogador, sizeof(jogador), &n, NULL)) {
                    MessageBox(NULL, _T("[ERRO] Erro na escrita (jogador.move)"), _T("Erro"), MB_ICONERROR | MB_OK);
                    exit(-4);
                }
                MessageBox(NULL, _T("[LEITOR] Escrita bem sucedida!"), _T("TECLA"), MB_ICONQUESTION | MB_OK);
                break;
            case VK_UP:
                //MessageBox(hWnd, TEXT("Tecla cima pressionada"), TEXT("Tecla de Direção"), MB_OK);
                jogador.move = 'U';
                if (!WriteFile(hPipe, (LPVOID)&jogador, sizeof(jogador), &n, NULL)) {
                    MessageBox(NULL, _T("[ERRO] Erro na escrita (jogador.move)"), _T("Erro"), MB_ICONERROR | MB_OK);
                    exit(-4);
                }
                MessageBox(NULL, _T("[LEITOR] Escrita bem sucedida!"), _T("TECLA"), MB_ICONQUESTION | MB_OK);
                break;
            case VK_DOWN:
                //MessageBox(hWnd, TEXT("Tecla baixo pressionada"), TEXT("Tecla de Direção"), MB_OK);
                jogador.move = 'D';
                if (!WriteFile(hPipe, (LPVOID)&jogador, sizeof(jogador), &n, NULL)) {
                    MessageBox(NULL, _T("[ERRO] Erro na escrita (jogador.move)"), _T("Erro"), MB_ICONERROR | MB_OK);
                    exit(-4);
                }
                MessageBox(NULL, _T("[LEITOR] Escrita bem sucedida!"), _T("TECLA"), MB_ICONQUESTION | MB_OK);
                break;
            }
        }

        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void AddMenu(HWND hWnd) {
    hMenu = CreateMenu();
    
    AppendMenu(hMenu, MF_STRING, ABOUT_BUTTON, TEXT("SOBRE"));
    AppendMenu(hMenu, MF_STRING, TESTE_BUTTON, TEXT("TESTE"));
    AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
    AppendMenu(hMenu, MF_STRING, SAIR_BUTTON, TEXT("SAIR"));

    SetMenu(hWnd, hMenu);

    return;
}

LRESULT CALLBACK DialogProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    DWORD n;
    switch (message)
    {
    case WM_COMMAND:
        switch (wParam)
        {
        case INDIVIDUAL_BUTTOM:
            GetWindowText(hName, nomeS, sizeof(nomeS) / sizeof(nomeS[0]));
            if (_tcscmp(nomeS, TEXT("")))
            {
                MessageBox(NULL, nomeS, _T("NOME"), MB_ICONQUESTION | MB_OK);

                ///*********************************************
            
                //coloca o modo escolhido no jogador
                if (!WaitNamedPipe(starterPipe, NMPWAIT_WAIT_FOREVER)) {
                    MessageBox(NULL, _T("[ERRO] Ligar ao pipe ") starterPipe _T("! (WaitNamedPipe)"), _T("Erro"), MB_ICONERROR | MB_OK);
                    exit(-1);
                }

                MessageBox(NULL, _T("[LEITOR] Ligação ao pipe do escritor... (CreateFile)"), _T("Leitor"), MB_ICONQUESTION | MB_OK);
                hPipe = CreateFile(starterPipe, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL, NULL);
                if (hPipe == NULL) {
                    MessageBox(NULL, _T("[ERRO] Ligar ao pipe ") starterPipe _T("! (CreateFile)"), _T("Erro"), MB_ICONERROR | MB_OK);
                    exit(-1);
                }
                MessageBox(NULL, _T("[LEITOR] Liguei-me..."), _T("Leitor"), MB_ICONQUESTION | MB_OK);

                jogador.mode = 0;
                if (!WriteFile(hPipe, (LPVOID)&jogador, sizeof(jogador), &n, NULL)) {
                    MessageBox(NULL, _T("[ERRO] Erro na escrita (jogador.mode)"), _T("Erro"), MB_ICONERROR | MB_OK);
                    exit(-2);
                }
                MessageBox(NULL, _T("[LEITOR] Escrita bem sucedida!"), _T("Leitor"), MB_ICONQUESTION | MB_OK);

                Sleep(1000);

                //Recebe o seu caracter unico
                if (!ReadFile(hPipe, (LPVOID)&jogador, sizeof(jogador), &n, NULL)) {
                    MessageBox(NULL, _T("[ERRO] Erro na leitura (jogador.player_char)"), _T("Erro"), MB_ICONERROR | MB_OK);
                    exit(-2);
                }
                caracterUnic = jogador.player_char;
                MessageBox(NULL, &caracterUnic, _T("LEU: "), MB_ICONQUESTION | MB_OK);


                ///*********************************************
                

                //SetEvent(hMapaLidoEvent);

                EnableWindow(hMainWindow, TRUE);
                DestroyWindow(hWnd);

                hThreadAtualizaMapa = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadAtualizaMapa, (LPVOID)&matrizMapa, 0, NULL);
                if (hThreadAtualizaMapa == NULL) {
                    _tprintf(TEXT("\n[ERRO] Erro ao lançar ThreadAtualizaMapa!\n"));
                    return 0;
                }

                hThreadTime = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TimeThread, (LPVOID)NULL, 0, NULL);
                if (hThreadTime == NULL) {
                    _tprintf(TEXT("\n[ERRO] Erro ao lançar TimeThread!\n"));
                    return 0;
                }


                //InvalidateRect(hMainWindow, NULL, TRUE);
                //UpdateWindow(hMainWindow);

            }
            else
            {
                MessageBox(NULL, _T("Tem de preencher o nome!"), _T("[ERRO] - Nome"), MB_ICONERROR | MB_OK);
            }
            
            break;
        case COMPETICAO_BUTTOM:
            EnableWindow(hMainWindow, TRUE);
            DestroyWindow(hWnd);
            break;
        default:
            break;
        }
        break;
    case WM_CLOSE:
        DisconnectNamedPipe(hPipe);
        TerminateThread(hThreadAtualizaMapa, 0);
        TerminateThread(hThreadTime, 0);
        EnableWindow(hMainWindow, TRUE);
        DestroyWindow(hWnd);
        break;
    default:
        return DefWindowProcW(hWnd,message,wParam,lParam);
    }
}

void registerDialogClass(HINSTANCE hInst) {
     WNDCLASSEX dialog = {0};

     dialog.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    dialog.hCursor = LoadCursor(NULL, IDC_ARROW);
    dialog.hInstance = hInst;
    dialog.lpszClassName = TEXT("myDialogClass");
    dialog.lpfnWndProc = DialogProcedure;

    RegisterClassW(&dialog);
}

void displayDialog(HWND hWnd) {
    hSecondWindow = CreateWindowW(TEXT("myDialogClass"), TEXT("Dialog"), WS_VISIBLE | WS_OVERLAPPEDWINDOW, 600, 100, 400, 400, hWnd, NULL, NULL, NULL);


    CreateWindow(TEXT("Static"), TEXT("Enter text here:"), WS_VISIBLE | WS_CHILD | SS_CENTER, 30, 110, 110, 20, hSecondWindow, NULL, NULL, NULL);
    hName = CreateWindow(TEXT("Edit"), NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 140, 110, 200, 20, hSecondWindow, NULL, NULL, NULL);
    CreateWindow(TEXT("Static"), TEXT("Modalidade de jogo:"), WS_VISIBLE | WS_CHILD | SS_CENTER, 15, 160, 150, 20, hSecondWindow, NULL, NULL, NULL);
    CreateWindow(TEXT("Button"), TEXT("Individual"), WS_VISIBLE | WS_CHILD, 165, 160, 100, 20, hSecondWindow, (HMENU)INDIVIDUAL_BUTTOM, NULL, NULL);
    CreateWindow(TEXT("Button"), TEXT("Competição"), WS_VISIBLE | WS_CHILD, 265, 160, 100, 20, hSecondWindow, (HMENU)COMPETICAO_BUTTOM, NULL, NULL);

    EnableWindow(hWnd, FALSE);
}
