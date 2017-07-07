

#ifndef __SKELETON_APP_H__
#define __SKELETON_APP_H__


#include "../common/SampleApp.h"


class SkeletonApp : public SampleApp
{
public:
    SkeletonApp();
    virtual ~SkeletonApp();

protected:  /// from Tiny3D::ApplicationListener
    virtual bool applicationDidFinishLaunching() override;
};


#endif  /*__SKELETON_APP_H__*/
