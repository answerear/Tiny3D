

#ifndef __TEXTURE_APP_H__
#define __TEXTURE_APP_H__


#include <Tiny3D.h>


class TextureApp 
    : public Tiny3D::ApplicationListener
{
public:
    TextureApp();
    virtual ~TextureApp();

    int32_t go();

protected:  /// from Tiny3D::ApplicationListener
    virtual bool applicationDidFinishLaunching() override;

    virtual void applicationDidEnterBackground() override;

    virtual void applicationWillEnterForeground() override;

    virtual void applicationWillTerminate() override;

protected:
    Tiny3D::RenderWindowPtr     mRenderWindow;
};


#endif  /*__TEXTURE_APP_H__*/
