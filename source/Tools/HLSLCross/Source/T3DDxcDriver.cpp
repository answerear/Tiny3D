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

#include "T3DDxcDriver.h"

// Windows 上 dxcapi.h 直接使用 IUnknown / HRESULT / IStream 等 COM 类型，不自带声明；
// 非 Windows 由 dxcapi.h 自己引入 WinAdapter.h。这里只要类型，动态库加载走 SharedLibrary。
#if defined (T3D_OS_WINDOWS)
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <unknwn.h>
    #include <objidl.h>
#endif

#include <dxc/dxcapi.h>

#include <algorithm>
#include <atomic>


namespace Tiny3D
{
    namespace
    {
        void moduleDirAnchor() {}

        /// UTF-8 → wchar_t（Windows 为 UTF-16，其余为 UTF-32）。
        /// 不用 T3D_LOCALE：POSIX 版依赖进程 locale，非法字节序列的行为也不确定
        WString toWide(const String &src)
        {
            WString dst;
            dst.reserve(src.size());
            const size_t n = src.size();
            size_t i = 0;
            while (i < n)
            {
                const uint8_t c = (uint8_t)src[i];
                uint32_t cp = 0xFFFD;
                size_t len = 1;
                if (c < 0x80)
                {
                    cp = c;
                }
                else if ((c & 0xE0) == 0xC0) { cp = c & 0x1F; len = 2; }
                else if ((c & 0xF0) == 0xE0) { cp = c & 0x0F; len = 3; }
                else if ((c & 0xF8) == 0xF0) { cp = c & 0x07; len = 4; }

                if (len > 1)
                {
                    if (i + len > n)
                    {
                        cp = 0xFFFD;
                        len = 1;
                    }
                    else
                    {
                        for (size_t k = 1; k < len; ++k)
                        {
                            const uint8_t cc = (uint8_t)src[i + k];
                            if ((cc & 0xC0) != 0x80)
                            {
                                cp = 0xFFFD;
                                len = k;
                                break;
                            }
                            cp = (cp << 6) | (cc & 0x3F);
                        }
                    }
                }
                i += len;

#if defined (T3D_OS_WINDOWS)
                if (cp >= 0x10000 && cp <= 0x10FFFF)
                {
                    cp -= 0x10000;
                    dst.push_back((wchar_t)(0xD800 + (cp >> 10)));
                    dst.push_back((wchar_t)(0xDC00 + (cp & 0x3FF)));
                    continue;
                }
                if (cp > 0x10FFFF)
                {
                    cp = 0xFFFD;
                }
#endif
                dst.push_back((wchar_t)cp);
            }
            return dst;
        }

        /// 最小的 COM 智能指针，不依赖 ATL（Windows 上 CComPtr 要 atlbase.h）
        template <typename T>
        class DxcPtr
        {
        public:
            DxcPtr() = default;
            DxcPtr(const DxcPtr &) = delete;
            DxcPtr &operator=(const DxcPtr &) = delete;

            ~DxcPtr()
            {
                if (mPtr != nullptr)
                {
                    mPtr->Release();
                }
            }

            T *get() const { return mPtr; }
            T *operator->() const { return mPtr; }
            T **put() { return &mPtr; }
            void **putVoid() { return reinterpret_cast<void **>(&mPtr); }

        private:
            T *mPtr {nullptr};
        };

        class DxcLibrary
        {
        public:
            // 有意泄漏：静态对象析构发生在 Platform 销毁之后，那时 SharedLibrary 的析构
            // 已经不安全；进程退出时卸载 dxcompiler 也可能撞上它内部静态对象的析构顺序
            static DxcLibrary &instance()
            {
                static DxcLibrary *s = new DxcLibrary();
                return *s;
            }

            bool isValid() const { return mCreateInstance != nullptr; }
            const String &error() const { return mError; }
            DxcCreateInstanceProc createInstance() const { return mCreateInstance; }

        private:
            DxcLibrary()
            {
                const String fileName = SharedLibrary::makeFileName("dxcompiler");
                TArray<String> candidates;

                // 1) 环境变量：打包 / CI / 装在系统目录时的逃生口
                if (Environment::has("T3D_DXCOMPILER_PATH"))
                {
                    candidates.push_back(Environment::get("T3D_DXCOMPILER_PATH"));
                }
                // 2) T3DHLSLCross 自身同级目录（默认部署，三平台一致）
                const String dir = SharedLibrary::getModuleDir((const void *)&moduleDirAnchor);
                if (!dir.empty())
                {
                    candidates.push_back(dir + Dir::getNativeSeparator() + fileName);
                }
                // 3) 裸文件名，退到系统搜索路径（Linux 发行版包、macOS Homebrew）
                candidates.push_back(fileName);

                String tried;
                for (const String &c : candidates)
                {
                    if (mLib.open(c, kSharedLibraryIsolated) == T3D_OK)
                    {
                        break;
                    }
                    // Windows 的错误文本已带路径，dlerror 通常也带，避免重复
                    const String err = mLib.getLastError();
                    tried += "\n  " + (err.find(c) != String::npos ? err : c + " : " + err);
                }

                if (mLib.isOpen())
                {
                    mCreateInstance = (DxcCreateInstanceProc)mLib.getSymbol("DxcCreateInstance");
                }

                if (mCreateInstance == nullptr)
                {
                    mError = "Failed to load " + fileName
                           + ". Put it next to T3DHLSLCross, or set T3D_DXCOMPILER_PATH to its full path.";
                    if (mLib.isOpen())
                    {
                        mError += " DxcCreateInstance not found: " + mLib.getLastError();
                    }
                    else
                    {
                        mError += " Tried:" + tried;
                    }
                    if (dir.empty())
                    {
                        mError += "\n  (T3DHLSLCross module directory unavailable)";
                    }
                }
            }

            SharedLibrary           mLib;
            DxcCreateInstanceProc   mCreateInstance {nullptr};
            String                  mError;
        };

        bool isAbsolutePath(const WString &p)
        {
#if defined (T3D_OS_WINDOWS)
            return (p.size() >= 2 && p[1] == L':') || (!p.empty() && (p[0] == L'\\' || p[0] == L'/'));
#else
            return !p.empty() && p[0] == L'/';
#endif
        }

        WString joinPath(const WString &dir, const WString &name)
        {
            if (dir.empty())
            {
                return name;
            }
            const wchar_t last = dir.back();
            return (last == L'/' || last == L'\\') ? dir + name : dir + L'/' + name;
        }

        WString parentDir(const WString &path)
        {
            const size_t pos = path.find_last_of(L"/\\");
            return pos == WString::npos ? WString() : path.substr(0, pos);
        }

        /**
         * 不用 IDxcLibrary::CreateIncludeHandler 的默认实现：旧版 DXC（2dec1cd0）会先拿
         * -I 目录本身调一次 LoadSource，读不到就丢掉这个搜索目录，之后 #include 只在
         * 源文件目录里找。这里先按 DXC 给的路径读，读不到再依次到 includeDirs、源文件目录下找。
         */
        class IncludeHandler : public IDxcIncludeHandler
        {
        public:
            IncludeHandler(IDxcLibrary *library, const HLSLCrossSource &source)
                : mLibrary(library)
            {
                for (const String &dir : source.includeDirs)
                {
                    mSearchDirs.push_back(toWide(dir));
                }
                const WString srcDir = parentDir(toWide(source.fileName));
                if (!srcDir.empty())
                {
                    mSearchDirs.push_back(srcDir);
                }
            }

            virtual ~IncludeHandler() = default;

            HRESULT STDMETHODCALLTYPE LoadSource(LPCWSTR pFilename, IDxcBlob **ppIncludeSource) override
            {
                if (ppIncludeSource == nullptr || pFilename == nullptr)
                {
                    return E_INVALIDARG;
                }
                *ppIncludeSource = nullptr;

                WString name = pFilename;
                if (name.size() > 2 && name[0] == L'.' && (name[1] == L'/' || name[1] == L'\\'))
                {
                    name = name.substr(2);
                }

                TArray<WString> candidates;
                candidates.push_back(name);
                if (!isAbsolutePath(name))
                {
                    for (const WString &dir : mSearchDirs)
                    {
                        candidates.push_back(joinPath(dir, name));
                    }
                }
                else
                {
                    // 旧版 DXC 只会传来 "<includer 所在目录>/<include 名>"，
                    // 把 include 名还原出来，再到其余搜索目录里找
                    const WString normName = normalize(name);
                    for (const WString &base : mSearchDirs)
                    {
                        WString prefix = normalize(base);
                        if (prefix.empty() || prefix.back() != L'/')
                        {
                            prefix += L'/';
                        }
                        if (normName.size() > prefix.size() && normName.compare(0, prefix.size(), prefix) == 0)
                        {
                            const WString rel = name.substr(prefix.size());
                            for (const WString &dir : mSearchDirs)
                            {
                                candidates.push_back(joinPath(dir, rel));
                            }
                            break;
                        }
                    }
                }

                for (const WString &c : candidates)
                {
                    // 不指定码页时 DXC 按系统 ANSI 码页解读，不带 BOM 的 UTF-8 中文注释会转码失败并被当成"找不到文件"
                    UINT32 codePage = DXC_CP_UTF8;
                    IDxcBlobEncoding *blob = nullptr;
                    if (SUCCEEDED(mLibrary->CreateBlobFromFile(c.c_str(), &codePage, &blob)) && blob != nullptr)
                    {
                        *ppIncludeSource = blob;
                        return S_OK;
                    }
                }

                return E_FAIL;
            }

            HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override
            {
                if (ppvObject == nullptr)
                {
                    return E_POINTER;
                }
                if (IsEqualIID(riid, __uuidof(IDxcIncludeHandler)) || IsEqualIID(riid, __uuidof(IUnknown)))
                {
                    *ppvObject = static_cast<IDxcIncludeHandler *>(this);
                    AddRef();
                    return S_OK;
                }
                *ppvObject = nullptr;
                return E_NOINTERFACE;
            }

            ULONG STDMETHODCALLTYPE AddRef() override
            {
                return ++mRef;
            }

            ULONG STDMETHODCALLTYPE Release() override
            {
                const ULONG ref = --mRef;
                if (ref == 0)
                {
                    delete this;
                }
                return ref;
            }

        private:
            /// 仅用于前缀比较：统一分隔符，Windows 下再忽略大小写
            static WString normalize(const WString &path)
            {
                WString s = path;
                for (wchar_t &c : s)
                {
                    if (c == L'\\')
                    {
                        c = L'/';
                    }
#if defined (T3D_OS_WINDOWS)
                    else if (c >= L'A' && c <= L'Z')
                    {
                        c = (wchar_t)(c - L'A' + L'a');
                    }
#endif
                }
                return s;
            }

            IDxcLibrary             *mLibrary {nullptr};
            TArray<WString>         mSearchDirs;
            std::atomic<ULONG>      mRef {1};
        };

        /// argv 里的每个 wstring 必须在 Compile() 期间存活；全部 push 完再取指针，
        /// 否则 vector 扩容会让之前的 c_str() 失效
        struct DxcArgs
        {
            void push(const String &a) { storage.push_back(toWide(a)); }

            void rebuild()
            {
                argv.clear();
                argv.reserve(storage.size());
                for (const auto &w : storage)
                {
                    argv.push_back(w.c_str());
                }
            }

            TArray<WString>     storage;
            TArray<LPCWSTR>     argv;
        };

        String profileName(HLSLStage stage, const HLSLCrossOptions &options)
        {
            static const char *kPrefixes[kHLSLStageCount] = { "vs", "ps", "gs", "hs", "ds", "cs" };
            const uint32_t index = (uint32_t)stage;
            if (index >= kHLSLStageCount)
            {
                return String();
            }

            return String(kPrefixes[index]) + "_" + std::to_string(options.shaderModelMajor)
                + "_" + std::to_string(options.shaderModelMinor);
        }

        bool buildArgs(const HLSLCrossSource &source, const HLSLCrossOptions &options,
            DxcArgs &args, String &outMessage)
        {
            args.push(options.packMatricesInRowMajor ? "-Zpc" : "-Zpr");

            if (options.enable16bitTypes)
            {
                const uint32_t sm = options.shaderModelMajor * 10u + options.shaderModelMinor;
                if (sm < 62)
                {
                    outMessage = "enable16bitTypes requires shader model 6.2 or higher, got "
                        + std::to_string(options.shaderModelMajor) + "." + std::to_string(options.shaderModelMinor);
                    return false;
                }
                args.push("-enable-16bit-types");
            }

            if (options.enableDebugInfo)
            {
                args.push("-Zi");
            }

            if (options.optimizationLevel > 3)
            {
                outMessage = "Invalid optimization level " + std::to_string(options.optimizationLevel)
                    + ", expected 0 to 3.";
                return false;
            }
            args.push("-O" + std::to_string(options.optimizationLevel));

            args.push("-spirv");

            for (const String &dir : source.includeDirs)
            {
                String d = dir;
                std::replace(d.begin(), d.end(), '\\', '/');
                args.push("-I");
                args.push(d);
            }

            return true;
        }
    }

    //--------------------------------------------------------------------------

    bool DxcDriver::isAvailable(String &outMessage)
    {
        DxcLibrary &lib = DxcLibrary::instance();
        if (!lib.isValid())
        {
            outMessage = lib.error();
            return false;
        }
        return true;
    }

    //--------------------------------------------------------------------------

    bool DxcDriver::compileToSpirV(const HLSLCrossSource &source,
        const HLSLCrossOptions &options, TArray<uint8_t> &outSpirv, String &outMessage)
    {
        DxcLibrary &lib = DxcLibrary::instance();
        if (!lib.isValid())
        {
            outMessage = lib.error();
            return false;
        }

        const String profile = profileName(source.stage, options);
        if (profile.empty())
        {
            outMessage = "Invalid shader stage " + std::to_string((uint32_t)source.stage) + ".";
            return false;
        }

        DxcArgs args;
        if (!buildArgs(source, options, args, outMessage))
        {
            return false;
        }
        args.rebuild();

        // 宏走 DxcDefine 而不是 -D，与 ShaderConductor 一致；值原样传，空串即 "NAME="
        TArray<WString> defineStorage;
        defineStorage.reserve(source.defines.size() * 2);
        TArray<DxcDefine> defines;
        defines.reserve(source.defines.size());
        for (const HLSLMacroDefine &d : source.defines)
        {
            defineStorage.push_back(toWide(d.name));
            defineStorage.push_back(toWide(d.value));
        }
        // 同 DxcArgs：defineStorage 填完不再扩容后才取 c_str()
        for (size_t i = 0; i < source.defines.size(); ++i)
        {
            defines.push_back({ defineStorage[i * 2].c_str(), defineStorage[i * 2 + 1].c_str() });
        }

        const WString fileName = toWide(source.fileName);
        const WString entryPoint = toWide(source.entryPoint.empty() ? String("main") : source.entryPoint);
        const WString profileW = toWide(profile);

        DxcPtr<IDxcLibrary> library;
        DxcPtr<IDxcCompiler> compiler;
        DxcPtr<IDxcIncludeHandler> includeHandler;
        DxcPtr<IDxcBlobEncoding> sourceBlob;

        if (FAILED(lib.createInstance()(CLSID_DxcLibrary, __uuidof(IDxcLibrary), library.putVoid()))
            || FAILED(lib.createInstance()(CLSID_DxcCompiler, __uuidof(IDxcCompiler), compiler.putVoid())))
        {
            outMessage = "Failed to create DXC instances.";
            return false;
        }
        *includeHandler.put() = new IncludeHandler(library.get(), source);

        if (FAILED(library->CreateBlobWithEncodingFromPinned(source.source.data(),
            (UINT32)source.source.size(), DXC_CP_UTF8, sourceBlob.put())))
        {
            outMessage = "Failed to create DXC source blob.";
            return false;
        }

        DxcPtr<IDxcOperationResult> result;
        HRESULT hr = compiler->Compile(sourceBlob.get(),
            fileName.c_str(), entryPoint.c_str(), profileW.c_str(),
            args.argv.data(), (UINT32)args.argv.size(),
            defines.data(), (UINT32)defines.size(),
            includeHandler.get(), result.put());
        if (FAILED(hr) || result.get() == nullptr)
        {
            outMessage = "IDxcCompiler::Compile failed to run.";
            return false;
        }

        // 诊断信息：错误与警告都在这里，是否失败只看 GetStatus
        DxcPtr<IDxcBlobEncoding> errors;
        if (SUCCEEDED(result->GetErrorBuffer(errors.put())) && errors.get() != nullptr
            && errors->GetBufferSize() > 0)
        {
            const char *p = (const char *)errors->GetBufferPointer();
            size_t size = errors->GetBufferSize();
            while (size > 0 && p[size - 1] == '\0')
            {
                --size;
            }
            outMessage.assign(p, size);
        }

        HRESULT status = S_OK;
        if (FAILED(result->GetStatus(&status)) || FAILED(status))
        {
            if (outMessage.empty())
            {
                outMessage = "DXC compilation failed without diagnostics.";
            }
            return false;
        }

        DxcPtr<IDxcBlob> object;
        if (FAILED(result->GetResult(object.put())) || object.get() == nullptr
            || object->GetBufferSize() == 0)
        {
            if (outMessage.empty())
            {
                outMessage = "DXC produced no output object.";
            }
            return false;
        }

        const uint8_t *p = (const uint8_t *)object->GetBufferPointer();
        outSpirv.assign(p, p + object->GetBufferSize());
        return true;
    }
}
