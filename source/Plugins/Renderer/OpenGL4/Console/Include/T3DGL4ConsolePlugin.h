/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_GL4CONSOLE_PLUGIN_H__
#define __T3D_GL4CONSOLE_PLUGIN_H__


#include "T3DGL4ConsolePrerequisites.h"


namespace Tiny3D
{
    class GL4ConsolePlugin : public Plugin
    {
    public:
        GL4ConsolePlugin();
        virtual ~GL4ConsolePlugin();

        virtual const String &getName() const override;
        virtual TResult install() override;
        virtual TResult startup() override;
        virtual TResult shutdown() override;
        virtual TResult uninstall() override;

    protected:
        String          mName;
        RHIRendererPtr  mRenderer;
    };
}


#endif  /*__T3D_GL4CONSOLE_PLUGIN_H__*/
