/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * You may use this sample code for anything you like, it is not covered by the
 * same license as the rest of the engine.
*******************************************************************************/

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
