

#ifndef __T3D_VIEWPORT_H__
#define __T3D_VIEWPORT_H__


#include "T3DPrerequisites.h"

#include "T3DColor.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Viewport
    {
    public:
        Viewport(SGCamera *pCamera, RenderTarget *pTarget, 
            Real left, Real top, Real width, Real height, int32_t nZOrder);
        virtual ~Viewport();

        void update();

        SGCamera *getCamera() const;
        void setCamera(SGCamera *pCamera);

        RenderTarget *getRenderTarget() const;

        Real getLeft() const;
        Real getTop() const;
        Real getWidth() const;
        Real getHeight() const;

        int32_t getActualLeft() const;
        int32_t getActualTop() const;
        int32_t getActualWidth() const;
        int32_t getActualHeight() const;

        void setDimensions(Real left, Real top, Real width, Real height);

        void setBackgroundColor(const Color &rkColor);
        const Color &getBackgroundColor() const;

    protected:
        SGCamera        *m_pCamera; /// Camera binding this camera
        RenderTarget    *m_pTarget; /// Render target binding this view port

        Real    m_fLeft;
        Real    m_fTop;
        Real    m_fWidth;
        Real    m_fHeight;

        Color   m_clrBackground;
    };
}


#endif  /*__T3D_VIEWPORT_H__*/
