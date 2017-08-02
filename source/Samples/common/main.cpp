/***************************************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * You may use this sample code for anything you like, it is not covered by the
 * same license as the rest of the engine.
***************************************************************************************************/

#include <windows.h>
#include "SampleApp.h"


extern SampleApp *app;


int main(int argc, char *argv[])
{
    String appPath = argv[0];
    return app->go(appPath);
}
