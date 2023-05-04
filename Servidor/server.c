#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>


int _tmain(int argc, TCHAR* argv[]) {
    
    int varAdd=0;
    HANDLE hOneServer;
 

    /* ... Mais variáveis ... */

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
   
                //_tprintf(TEXT("\nFactor > "));
                //_tscanf_s(TEXT("%d"), varAdd);

                ReleaseMutex(hOneServer);
                CloseHandle(hOneServer);
    return 0;
}