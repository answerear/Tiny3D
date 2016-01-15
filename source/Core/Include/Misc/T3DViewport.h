

#ifndef __T3D_VIEWPORT_H__
#define __T3D_VIEWPORT_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Misc/T3DColor4.h"
#include "Misc/T3DObject.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Viewport : public Object
    {
    public:
        static ViewportPtr create(SGCamera *camera, RenderTarget *target, 
            Real left, Real top, Real width, Real height, int32_t nZOrder);

        virtual ~Viewport();

        void update();

        SGCamera *getCamera() const;
        void setCamera(SGCamera *camera);

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

        void setBackgroundColor(const Color4 &rkColor);
        const Color4 &getBackgroundColor() const;

    protected:
        Viewport(SGCamera *camera, RenderTarget *target, 
            Real left, Real top, Real width, Real height, int32_t nZOrder);

        void updateDimemsions();

    protected:
        SGCamera        *mCamera; /// Camera binding this camera
        RenderTarget    *mTarget; /// Render target binding this view port

        Real    mLeft;
        Real    mTop;
        Real    mWidth;
        Real    mHeight;

        int32_t mActualLeft;
        int32_t mActualTop;
        int32_t mActualWidth;
        int32_t mActualHeight;

        Color4  mBkgndColor;
    };
}


#include "T3DViewport.inl"


#endif  /*__T3D_VIEWPORT_H__*/
