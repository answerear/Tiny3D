/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_GLES3_PLUGIN_H__
#define __T3D_GLES3_PLUGIN_H__


#include "T3DGLES3Prerequisites.h"


namespace Tiny3D
{
    class GLES3Plugin : public Plugin
    {
    public:
        GLES3Plugin();

        virtual ~GLES3Plugin();

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


#endif  /*__T3D_GLES3_PLUGIN_H__*/
