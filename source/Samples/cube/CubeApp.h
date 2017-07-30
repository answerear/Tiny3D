/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * You may use this sample code for anything you like, it is not covered by the
 * same license as the rest of the engine.
*******************************************************************************/

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
