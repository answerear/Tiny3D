

#ifndef __TRIANGLE_APP_H__
#define __TRIANGLE_APP_H__


#include "../common/SampleApp.h"


class TriangleApp : public SampleApp
{
public:
    TriangleApp();
    virtual ~TriangleApp();

protected:  /// from Tiny3D::ApplicationListener
    virtual bool applicationDidFinishLaunching() override;
};


#endif  /*__TRIANGLE_APP_H__*/
