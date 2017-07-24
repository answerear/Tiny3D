

#ifndef __HELLO_WOLRD_APP_H__
#define __HELLO_WOLRD_APP_H__


#include "../common/SampleApp.h"


class FontApp : public SampleApp
{
public:
    FontApp();
    virtual ~FontApp();

protected:  /// from Tiny3D::ApplicationListener
    virtual bool applicationDidFinishLaunching() override;
};


#endif  /*__HELLO_WOLRD_APP_H__*/
