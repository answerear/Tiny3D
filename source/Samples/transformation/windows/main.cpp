
#include <windows.h>
#include "TransformationApp.h"


TransformationApp theApp;


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    return theApp.go();
}