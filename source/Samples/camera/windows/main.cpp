
#include <windows.h>
#include "CubeApp.h"


CubeApp theApp;


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    return theApp.go();
}