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

#ifndef __T3D_TYPE_H__
#define __T3D_TYPE_H__

#define NOMINMAX

#include <typeinfo>
#include <string>
#include <sstream>
#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <variant>

#include <string.h>
#include <memory.h>


typedef signed char         char_t;
typedef unsigned char       uchar_t;
typedef signed short        short_t;
typedef unsigned short      ushort_t;
typedef signed int          int_t;
typedef unsigned int        uint_t;

typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef signed short        int16_t;
typedef unsigned short      uint16_t;
typedef signed int          int32_t;
typedef unsigned int        uint32_t;

#if !defined (T3D_OS_ANDROID) && !defined (T3D_OS_LINUX)
    typedef signed long long    int64_t;
    typedef unsigned long long  uint64_t;
#endif

#if defined (T3D_OS_X64)
    typedef int64_t             long_t;
    typedef uint64_t            ulong_t;
#else
    typedef int32_t             long_t;
    typedef uint32_t            ulong_t;
#endif

typedef float               float32_t;
typedef double              float64_t;

typedef std::string         String;
typedef std::wstring        WString;

typedef std::string         UTF8String;
typedef std::u16string      UTF16String;
typedef std::u32string      UTF32String;

typedef void*               THandle;

typedef int32_t             TResult;

namespace Tiny3D
{
    typedef uint32_t            ID;
}

#define T3D_INVALID_ID      0

typedef std::thread                     TThread;
typedef std::mutex                      TMutex;
typedef std::recursive_mutex            TRecursiveMutex;
typedef std::condition_variable         TCondVariable;

template <typename T>
using TAutoLock = std::unique_lock<T>;

template <typename T>
using TArray = std::vector<T>;

template <typename T>
using TList = std::list<T>;

template <typename T>
using TQueue = std::queue<T>;

template <typename T>
using TDeque = std::deque<T>;

template <typename T, typename CONTAINER = TArray<T>, typename CMP = std::less<typename CONTAINER::value_type>>
using TPriorityQueue = std::priority_queue<T, CONTAINER, CMP>;

template <typename T>
using TStack = std::stack<T>;

template <typename T>
using TSet = std::set<T>;

template <typename T>
using TMultiset = std::multiset<T>;

template <typename K, typename V>
using TMap = std::map<K, V>;

template <typename K, typename V>
using TMultimap = std::multimap<K, V>;

template <typename T>
using TUnorderedSet = std::unordered_set<T>;

template <typename T>
using UUnorderedMultiset = std::unordered_multiset<T>;

// template <typename K, typename V>
// using TUnorderedMap = std::unordered_map<K, V>;

template <typename K, typename V, typename HASHER = std::hash<K>, typename KEY_EQ = std::equal_to<K>, typename ALLOC = std::allocator<std::pair<const K, V>>>
using TUnorderedMap = std::unordered_map<K, V, HASHER, KEY_EQ, ALLOC>;

template <typename K, typename V, typename HASHER = std::hash<K>, typename KEY_EQ = std::equal_to<K>, typename ALLOC = std::allocator<std::pair<const K, V>>>
using TUnorderedMultimap = std::unordered_multimap<K, V, HASHER, KEY_EQ, ALLOC>;

template <typename T1, typename T2>
using TPair = std::pair<T1, T2>;

template <typename T>
using TFunction = std::function<T>;

template <typename ...Types>
using TVariant = std::variant<Types...>;

template <typename T>
using TAtomic = std::atomic<T>;

template <typename ...Types>
using TTuple = std::tuple<Types...>;

#endif  /*__T3D_TYPE_H__*/
