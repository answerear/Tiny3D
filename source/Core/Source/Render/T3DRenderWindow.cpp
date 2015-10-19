

#include "Render/T3DRenderWindow.h"


namespace Tiny3D
{
    RenderWindow::RenderWindow()
        : RenderTarget()
    {

    }

    RenderWindow::~RenderWindow()
    {

    }

    bool RenderWindow::isFullScreen() const
    {
        return true;
    }
}