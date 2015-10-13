

#ifndef __T3D_RENDER_TARGET_H__
#define __T3D_RENDER_TARGET_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    class T3D_ENGINE_API RenderTarget
    {
    public:
        RenderTarget();
        virtual ~RenderTarget();

        const TString &getName() const
        {
            return m_strName;
        }

        void getMetrics(int32_t &nWidth, int32_t &nHeight,
            int32_t &nColorDepth) const;

        int32_t getWidth() const
        {
            return m_nWidth;
        }

        int32_t getHeight() const
        {
            return m_nHeight;
        }

        int32_t getColorDepth() const
        {
            return m_nColorDepth;
        }

        virtual void update();

        Viewport *addViewport(SGCamera *pCamera, int32_t nZOrder, 
            Real left, Real top, Real width, Real height);

        void removeViewport(int32_t nZOrder);

        void removeAllViewports();

        uint32_t getNumViewports() const
        {
            return (uint32_t)m_ViewportList.size();
        }

        Viewport *getViewport(uint32_t unIndex) const;

        void addListener(RenderTargetListener *pListener);
        void removeListener(RenderTargetListener *pListener);
        void removeAllListener();

    protected:
        typedef std::map<int32_t, ViewportPtr>      ViewportList;
        typedef std::pair<int32_t, ViewportPtr>     ViewportValue;

        typedef std::list<RenderTargetListener*>    ListenerList;

        int32_t         m_nWidth;
        int32_t         m_nHeight;
        int32_t         m_nColorDepth;

        TString         m_strName;

        ViewportList    m_ViewportList;

        ListenerList    m_ListenerList;
    };
}


#endif  /*__T3D_RENDER_TARGET_H__*/
