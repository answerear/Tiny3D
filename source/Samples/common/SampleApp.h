

#ifndef __SAMPLE_APP_H__
#define __SAMPLE_APP_H__


#include <Tiny3D.h>


class SampleApp 
    : public Tiny3D::ApplicationListener
    , public Tiny3D::WindowEventListener
{
public:
    SampleApp();
    virtual ~SampleApp();

    virtual int32_t go();

protected:  /// from Tiny3D::ApplicationListener
    virtual bool applicationDidFinishLaunching() override;

    virtual void applicationDidEnterBackground() override;

    virtual void applicationWillEnterForeground() override;

    virtual void applicationWillTerminate() override;

protected:
    Tiny3D::RenderWindowPtr     mRenderWindow;
};


#endif  /*__CUBE_APP_H__*/
