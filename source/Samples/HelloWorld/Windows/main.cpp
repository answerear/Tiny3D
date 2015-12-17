
#include <windows.h>
#include "HelloWorldApp.h"


HelloWorldApp theApp;


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    return theApp.go();
}