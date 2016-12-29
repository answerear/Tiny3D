
#include <windows.h>
#include "SkeletonApp.h"


SkeletonApp theApp;


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    return theApp.go();
}