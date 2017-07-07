

#ifndef __CUBE_APP_H__
#define __CUBE_APP_H__


#include "../common/SampleApp.h"
#include <SDL.h>


class CubeApp : public SampleApp
{
public:
    CubeApp();
    virtual ~CubeApp();

    virtual int32_t go() override;

protected:  /// from Tiny3D::ApplicationListener
    virtual bool applicationDidFinishLaunching() override;

    void run();

    SDL_Window  *mSDLWindow;
};


#endif  /*__CUBE_APP_H__*/
