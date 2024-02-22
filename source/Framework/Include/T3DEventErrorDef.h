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

#ifndef __T3D_EVENT_ERROR_DEF_H__
#define __T3D_EVENT_ERROR_DEF_H__


#include "T3DEventPrerequisites.h"


namespace Tiny3D
{
    // Framework 模块错误码从 T3D_ERR_FRAMEWORK 开始
    enum FrameworkErrorCode
    {
        /**< 无效的实例句柄 */
        T3D_ERR_FWK_INVALID_INSTANCE    = T3D_ERR_FRAMEWORK,
        /**< 无效的接收者 */
        T3D_ERR_FWK_INVALID_RECVER      = T3D_ERR_FRAMEWORK + 1,
        /**< 无效的发送者 */
        T3D_ERR_FWK_INVALID_SENDER      = T3D_ERR_FRAMEWORK + 2,
        /**< 无效事件ID */
        T3D_ERR_FWK_INVALID_EVID        = T3D_ERR_FRAMEWORK + 3,
        /**< 事件处理超时，无法立刻完成全部，被打断了 */
        T3D_ERR_FWK_HANDLING_TIMEOVER   = T3D_ERR_FRAMEWORK + 4,
        /**< 事件嵌套超出栈调用层次，被打断了 */
        T3D_ERR_FWK_CALLSTACK_OVERFLOW  = T3D_ERR_FRAMEWORK + 5,
        /**< 没有任何对象处理 */
        T3D_ERR_FWK_NONE_HANDLER        = T3D_ERR_FRAMEWORK + 6,
        /**< 无法马上处理，因为被暂停派发了 */
        T3D_ERR_FWK_SUSPENDED           = T3D_ERR_FRAMEWORK + 7,
        /**< 重复实例句柄 */
        T3D_ERR_FWK_DUPLICATE_INSTANCE  = T3D_ERR_FRAMEWORK + 8,
    };
    
}


#endif  /*__T3D_EVENT_ERROR_DEF_H__*/
