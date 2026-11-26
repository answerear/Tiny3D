/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#ifndef __T3D_D3D11_QUERY_H__
#define __T3D_D3D11_QUERY_H__


#include "T3DD3D11Prerequisites.h"


namespace Tiny3D
{
    /**
     * \brief D3D11 查询对象
     * \remarks kBinaryOcclusion 建成 ID3D11Predicate，可直接交给 SetPredication
     */
    class D3D11Query : public RHIQuery
    {
    public:
        static D3D11QueryPtr create(QueryType type);

        ~D3D11Query() override;

        void *getNativeObject() const override;

        /// 底层查询；kBinaryOcclusion 时实际类型为 ID3D11Predicate
        ID3D11Query *D3DQuery {nullptr};

    protected:
        explicit D3D11Query(QueryType type);
    };
}


#endif  /*__T3D_D3D11_QUERY_H__*/
