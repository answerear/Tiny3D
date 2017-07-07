

#ifndef __TEXTURE_APP_H__
#define __TEXTURE_APP_H__


#include "../common/SampleApp.h"


class TextureApp : public SampleApp
{
public:
    TextureApp();
    virtual ~TextureApp();

protected:  /// from Tiny3D::ApplicationListener
    virtual bool applicationDidFinishLaunching() override;
};


#endif  /*__TEXTURE_APP_H__*/
