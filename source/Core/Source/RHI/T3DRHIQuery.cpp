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


#include "RHI/T3DRHIQuery.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char *getQueryTypeName(QueryType type)
    {
        switch (type)
        {
        case QueryType::kOcclusion:             return "Occlusion";
        case QueryType::kBinaryOcclusion:       return "BinaryOcclusion";
        case QueryType::kTimestamp:             return "Timestamp";
        case QueryType::kPipelineStatistics:    return "PipelineStatistics";
        default:                                return "Unknown";
        }
    }

    //--------------------------------------------------------------------------

    RHIQuery::RHIQuery(QueryType type)
        : mQueryType(type)
    {
        mPublishedResult.type = type;
    }

    //--------------------------------------------------------------------------

    bool RHIQuery::markBegin(const char *tag)
    {
        if (mQueryType == QueryType::kTimestamp)
        {
            T3D_LOG_ERROR(tag, "beginQuery : timestamp queries have no begin, call endQuery to write the timestamp !");
            return false;
        }

        if (mActive)
        {
            T3D_LOG_ERROR(tag, "beginQuery : %s query is already active, call endQuery first !", getQueryTypeName(mQueryType));
            return false;
        }

        mActive = true;
        return true;
    }

    //--------------------------------------------------------------------------

    bool RHIQuery::markEnd(const char *tag, uint32_t &serial)
    {
        if (mQueryType != QueryType::kTimestamp)
        {
            if (!mActive)
            {
                T3D_LOG_ERROR(tag, "endQuery : %s query was not begun !", getQueryTypeName(mQueryType));
                return false;
            }
            mActive = false;
        }

        ++mIssuedSerial;
        if (mIssuedSerial == 0)
        {
            // 0 留给「从未发起」
            mIssuedSerial = 1;
        }

        serial = mIssuedSerial;
        return true;
    }

    //--------------------------------------------------------------------------

    void RHIQuery::publish(uint32_t serial, QueryStatus status, const QueryResult &result)
    {
        mPublishedStatus = status;
        mPublishedResult = result;
        mPublishedResult.type = mQueryType;
        mPublishedSerial.store(serial, std::memory_order_release);
    }

    //--------------------------------------------------------------------------

    QueryStatus RHIQuery::resolve(QueryResult &result) const
    {
        if (mIssuedSerial == 0)
        {
            return QueryStatus::kInvalid;
        }

        // 后端只会发布已经发起过的序号，而且主线程在这里读的时候不会再发起新的，
        // 所以序号相等时结果不会再被改写
        if (mPublishedSerial.load(std::memory_order_acquire) != mIssuedSerial)
        {
            return QueryStatus::kPending;
        }

        if (mPublishedStatus == QueryStatus::kReady)
        {
            result = mPublishedResult;
        }

        return mPublishedStatus;
    }

    //--------------------------------------------------------------------------
}
