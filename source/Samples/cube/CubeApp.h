

#ifndef __CUBE_APP_H__
#define __CUBE_APP_H__


#include "../common/SampleApp.h"


class CubeApp : public SampleApp
{
public:
    CubeApp();
    virtual ~CubeApp();

protected:  /// from Tiny3D::ApplicationListener
    virtual bool applicationDidFinishLaunching() override;
};


#endif  /*__CUBE_APP_H__*/
