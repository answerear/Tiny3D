

#ifndef __MODEL_APP_H__
#define __MODEL_APP_H__


#include "../common/SampleApp.h"


class ModelApp : public SampleApp
{
public:
    ModelApp();
    virtual ~ModelApp();

protected:  /// from Tiny3D::ApplicationListener
    virtual bool applicationDidFinishLaunching() override;
};


#endif  /*__MODEL_APP_H__*/
