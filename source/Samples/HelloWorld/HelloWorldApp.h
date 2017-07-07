

#ifndef __HELLO_WOLRD_APP_H__
#define __HELLO_WOLRD_APP_H__


#include "../common/SampleApp.h"


class HelloWorldApp : public SampleApp
{
public:
    HelloWorldApp();
    virtual ~HelloWorldApp();

protected:  /// from Tiny3D::ApplicationListener
    virtual bool applicationDidFinishLaunching() override;
};


#endif  /*__HELLO_WOLRD_APP_H__*/
