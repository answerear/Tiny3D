/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_NULLGL4_WINDOW_H__
#define __T3D_NULLGL4_WINDOW_H__


#include "T3DNullGL4Prerequisites.h"


namespace Tiny3D
{
    class NullGL4Window : public RHIRenderWindow
    {
    public:
        static NullGL4WindowPtr create();
        virtual ~NullGL4Window();

        TResult swapBuffers() override;
        TResult resize(uint32_t w, uint32_t h) override;
        void *getNativeObject() const override;

    protected:
        NullGL4Window();
    };
}


#endif  /*__T3D_NULLGL4_WINDOW_H__*/
