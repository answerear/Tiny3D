

#ifndef __TRIANGLE_APP_H__
#define __TRIANGLE_APP_H__


#include <Tiny3D.h>


class TriangleApp 
    : public Tiny3D::ApplicationListener
{
public:
    TriangleApp();
    virtual ~TriangleApp();

    int32_t go();

protected:  /// from Tiny3D::ApplicationListener
    virtual bool applicationDidFinishLaunching() override;

    virtual void applicationDidEnterBackground() override;

    virtual void applicationWillEnterForeground() override;

    virtual void applicationWillTerminate() override;

protected:
    Tiny3D::RenderWindowPtr     mRenderWindow;
};


#endif  /*__TRIANGLE_APP_H__*/
