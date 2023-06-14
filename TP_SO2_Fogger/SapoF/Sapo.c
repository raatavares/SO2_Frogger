#include "framework.h"
#include "Sapo.h"
#include "..\\DLL\Header.h"

#define MAX_LOADSTRING 100
#define SAIR_BUTTON 0
#define TESTE_BUTTON 10
#define INDIVIDUAL_BUTTOM 1
#define COMPETICAO_BUTTOM 2
#define ABOUT_BUTTON 3

//DADOS dados;
player jogador;

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
HWND hName, hMainWindow;
HMENU hMenu;


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
    //jogador.mode = 3;


    //================================================================================================

    if (!WaitNamedPipe(starterPipe, NMPWAIT_WAIT_FOREVER)) {
        //_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (WaitNamedPipe)\n"), starterPipe);
        MessageBox(NULL, _T("[ERRO] Ligar ao pipe ") starterPipe _T("! (WaitNamedPipe)"), _T("Erro"), MB_ICONERROR | MB_OK);
        exit(-1);
    }

    //_tprintf(TEXT("[LEITOR] Ligação ao pipe do escritor... (CreateFile)\n"));
    MessageBox(NULL, _T("[LEITOR] Ligação ao pipe do escritor... (CreateFile)"), _T("Leitor"), MB_ICONQUESTION | MB_OK);
    hPipe = CreateFile(starterPipe, GENERIC_ALL, 0, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (hPipe == NULL) {
        //_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (CreateFile)\n"), starterPipe);
        MessageBox(NULL, _T("[ERRO] Ligar ao pipe ") starterPipe _T("'! (CreateFile)"), _T("Erro"), MB_ICONERROR | MB_OK);
        exit(-1);
    }
    //_tprintf(TEXT("[LEITOR] Liguei-me...\n"));
    MessageBox(NULL, _T("[LEITOR] Liguei-me..."), _T("Leitor"), MB_ICONQUESTION | MB_OK);
    // Aguardar a conexão de um cliente
    DWORD n;
    //coloca o modo escolhido no jogador
    jogador.mode = 1;

    if (!WriteFile(hPipe, (LPVOID)&jogador, sizeof(jogador), &n, NULL)) {
      //  _tprintf(TEXT("[LEITOR] %d... (ReadFile)\n"), n);
        MessageBox(NULL, _T("[LEITOR] "+ n) , _T("Erro"), MB_ICONERROR | MB_OK);

    }
    //_tprintf(TEXT("[CLIENTE] Modo escolhido: %d\n"), jogador.mode);
    //MessageBox(NULL, str, _T("Leitor"), MB_ICONQUESTION | MB_OK);


    //recebe o modo escolhido pelo primeiro quer este seja o primeiro ou o segundo
    //tbm recebe o seu caracter unico
    //_tprintf(TEXT("[LEITOR] Recebi %d bytes: ''... (ReadFile)\n"), n);
    MessageBox(NULL, _T("[LEITOR] Recebi " + n), _T("Leitor"), MB_ICONQUESTION | MB_OK);
    if (!ReadFile(hPipe, (LPVOID)&jogador, sizeof(jogador), &n, NULL)) {
        //_tprintf(TEXT("[LEITOR] %d... (ReadFile)\n"), n);
        MessageBox(NULL, _T("[LEITOR]" + n), _T("Erro"), MB_ICONERROR | MB_OK);

    }
    //_tprintf(TEXT("[LEITOR] Recebi %d bytes: ''... (ReadFile)\n"), n);
    DisconnectNamedPipe(hPipe);
    

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
      500, 0, 600, 600, NULL, NULL, hInstance, NULL);

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
                int value = MessageBox(hWnd, TEXT("Tem a certeza que deseja sair?"), TEXT("Confirmação"), MB_ICONQUESTION | MB_YESNO);
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
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Adicione qualquer código de desenho que use hdc aqui...
            AddMenu(hWnd);
            EndPaint(hWnd, &ps);
        }
        break;
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
            //jogador.mode = 0;
            //WriteFile(hPipe, (LPVOID)&jogador.mode, sizeof(jogador.mode), &n, NULL);
            EnableWindow(hMainWindow, TRUE);
            DestroyWindow(hWnd);
            MessageBeep(MB_OK);
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
    HWND hDld = CreateWindowW(TEXT("myDialogClass"), TEXT("Dialog"), WS_VISIBLE | WS_OVERLAPPEDWINDOW, 600, 100, 400, 400, hWnd, NULL, NULL, NULL);


    CreateWindow(TEXT("Static"), TEXT("Enter text here:"), WS_VISIBLE | WS_CHILD | SS_CENTER, 30, 110, 110, 20, hDld, NULL, NULL, NULL);
    /*hName = */
    CreateWindow(TEXT("Edit"), NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 140, 110, 200, 20, hDld, NULL, NULL, NULL);
    CreateWindow(TEXT("Static"), TEXT("Modalidade de jogo:"), WS_VISIBLE | WS_CHILD | SS_CENTER, 15, 160, 150, 20, hDld, NULL, NULL, NULL);
    CreateWindow(TEXT("Button"), TEXT("Individual"), WS_VISIBLE | WS_CHILD, 165, 160, 100, 20, hDld, (HMENU)INDIVIDUAL_BUTTOM, NULL, NULL);
    CreateWindow(TEXT("Button"), TEXT("Competição"), WS_VISIBLE | WS_CHILD, 265, 160, 100, 20, hDld, (HMENU)COMPETICAO_BUTTOM, NULL, NULL);

    EnableWindow(hWnd, FALSE);
}

/*
void recebeMapa(DADOS* dados) {
    DWORD n;
    if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
        _tprintf(TEXT("[ERRO] Ligar ao pipe '%s'!\n"), PIPE_NAME);
        return 0;
    }
    _tprintf(TEXT("[ConPass] Ligação ao pipe da Central...\n"));
    hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hPipe == NULL) {
        _tprintf(TEXT("[ERRO] Ligar ao pipe '%s'!\n"), PIPE_NAME);
        return 0;
    }

    ReadFile(hPipe, dados->mapa, sizeof(MAPA), &n, NULL);
    
    return;
}
*/