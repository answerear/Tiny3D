

#ifndef __HELLO_WOLRD_APP_H__
#define __HELLO_WOLRD_APP_H__


#include <Tiny3D.h>


class HelloWorldApp 
    : public Tiny3D::ApplicationListener
{
public:
    HelloWorldApp();
    virtual ~HelloWorldApp();

    int32_t go();

protected:  /// from Tiny3D::ApplicationListener
    virtual bool applicationDidFinishLaunching() override;

    virtual void applicationDidEnterBackground() override;

    virtual void applicationWillEnterForeground() override;

    virtual void applicationWillTerminate() override;
};


#endif  /*__HELLO_WOLRD_APP_H__*/
