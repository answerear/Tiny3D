

#ifndef __TRANSFORMATION_APP_H__
#define __TRANSFORMATION_APP_H__


#include "../common/SampleApp.h"


class TransformationApp : public SampleApp
{
public:
    TransformationApp();
    virtual ~TransformationApp();

protected:  /// from Tiny3D::ApplicationListener
    virtual bool applicationDidFinishLaunching() override;
};


#endif  /*__TRANSFORMATION_APP_H__*/
