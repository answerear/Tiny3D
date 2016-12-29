

#ifndef __SKELETON_APP_H__
#define __SKELETON_APP_H__


#include <Tiny3D.h>


class SkeletonApp 
    : public Tiny3D::ApplicationListener
{
public:
    SkeletonApp();
    virtual ~SkeletonApp();

    int32_t go();

protected:  /// from Tiny3D::ApplicationListener
    virtual bool applicationDidFinishLaunching() override;

    virtual void applicationDidEnterBackground() override;

    virtual void applicationWillEnterForeground() override;

    virtual void applicationWillTerminate() override;

protected:
    Tiny3D::RenderWindowPtr     mRenderWindow;
};


#endif  /*__SKELETON_APP_H__*/
