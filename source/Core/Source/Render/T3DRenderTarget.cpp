

#include "T3DRenderTarget.h"
#include "T3DViewport.h"


namespace Tiny3D
{
    RenderTarget::RenderTarget()
        : m_nWidth(0)
        , m_nHeight(0)
        , m_nColorDepth(0)
    {

    }

    RenderTarget::~RenderTarget()
    {

    }

    void RenderTarget::getMetrics(int32_t &nWidth, int32_t &nHeight, 
        int32_t &nColorDepth) const
    {
        nWidth = m_nWidth;
        nHeight = m_nHeight;
        nColorDepth = m_nColorDepth;
    }

    Viewport *RenderTarget::addViewport(SGCamera *pCamera, int32_t nZOrder, 
        Real left, Real top, Real width, Real height)
    {
        Viewport *pViewport = nullptr;

        auto itr = m_ViewportList.find(nZOrder);

        if (itr == m_ViewportList.end())
        {
            pViewport = new Viewport();
            ViewportPtr v(pViewport);
            m_ViewportList.insert(ViewportValue(nZOrder, v));
        }

        return pViewport;
    }
}
