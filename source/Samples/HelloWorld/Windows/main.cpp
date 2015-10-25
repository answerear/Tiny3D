
#include <windows.h>
#include <Tiny3D.h>


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    Tiny3D::Entrance *entrance = new Tiny3D::Entrance("../../media/config/Tiny3D.cfg");

    return 0;
}