/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_NULLGL4_PLUGIN_H__
#define __T3D_NULLGL4_PLUGIN_H__


#include "T3DNullGL4Prerequisites.h"


namespace Tiny3D
{
    class NullGL4Plugin : public Plugin
    {
    public:
        NullGL4Plugin();
        virtual ~NullGL4Plugin();

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


#endif  /*__T3D_NULLGL4_PLUGIN_H__*/
