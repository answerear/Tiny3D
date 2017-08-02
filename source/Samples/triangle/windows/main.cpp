
#include <windows.h>
#include "TriangleApp.h"


TriangleApp theApp;


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    String appPath;
    return theApp.go(appPath);
}