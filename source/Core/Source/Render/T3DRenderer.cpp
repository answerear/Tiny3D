

#include "T3DRenderWindow.h"


namespace Tiny3D
{
    RenderWindow::RenderWindow()
        : m_nWidth(0)
        , m_nHeight(0)
        , m_nColorDepth(0)
    {

    }

    RenderWindow::~RenderWindow()
    {

    }

    bool RenderWindow::isFullScreen() const
    {
        return true;
    }

    void RenderWindow::getMetrics(int32_t &nLeft, int32_t &nTop,
                                  int32_t &nWidth, int32_t &nHeight, 
                                  int32_t &nColorDepth)
    {
        nWidth = m_nWidth;
        nHeight = m_nHeight;
        nColorDepth = m_nColorDepth;
        nLeft = nTop = 0;
    }
}