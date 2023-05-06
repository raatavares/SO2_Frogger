#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#define TAM 200



int _tmain(int argc, TCHAR* argv[]) {
    HKEY chave;
    DWORD resultado;
    int varAdd = 0, numFaixas;
    HANDLE hOneServer;
    int rows, cols,velocidade;
    TCHAR** areaJogo;
    TCHAR chave_nome[TAM] = TEXT("SOFTWARE\\Frogger\\chave_TP");
    TCHAR par_nome[TAM] = TEXT("velocidade");
    DWORD par_valor;



#ifdef UNICODE 
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif



    hOneServer = CreateMutex(NULL, TRUE, "OneServer");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        printf("Ja existe uma instancia do servidor em execucao.\n");
        return 0;
    }
    

    if (/*argc < 3 */1 ) {
        if (RegOpenKeyEx(HKEY_CURRENT_USER, chave_nome,0,KEY_ALL_ACCESS,&chave) != ERROR_SUCCESS) {
            _tprintf(TEXT("Coloque argumentos"));
            return -1;
        }

        
        DWORD tamanho = sizeof(par_valor);
        if (RegQueryValueEx(chave, par_nome,NULL, NULL, (LPBYTE)&par_valor,&tamanho) == ERROR_SUCCESS)
            _tprintf(TEXT("Atributo encontrado com valor:%d e tamanho:%d\n"), par_valor, tamanho);
        else
            _tprintf(TEXT("Erro ao recuperar o valor da chave: %s\n"), par_nome);


        
        
    }/*
    else {
        rows = _ttoi(argv[1]);
        velocidade = _ttoi(argv[2]);
        cols = 20;
    }
    
    _tprintf(TEXT("\n rows=%d,velocidade=%d "), rows, velocidade);





    areaJogo = (TCHAR**)malloc(rows * sizeof(TCHAR*));
    for (int i = 0; i < rows; i++) {
        areaJogo[i] = (TCHAR*)malloc(cols * sizeof(TCHAR));
    }

    for (int i = 0; i < rows; i++) {
        _tprintf(TEXT("\n"));

        for (int j = 0; j < cols; j++) {

            areaJogo[i][j] = TEXT('o');
            _tprintf(TEXT("%c"),areaJogo[i][j]);
        }
    }

    do {
        _tprintf(TEXT("\nFactor > "));
        _tscanf_s(TEXT("%d"), &varAdd);
    } while (1);








    //fechar handlers e memoria dinamica 
    for (int i = 0; i < rows; i++) {
        free(areaJogo[i]);
    }
    free(areaJogo);
    */
    ReleaseMutex(hOneServer);
    CloseHandle(hOneServer);
    return 0;
}