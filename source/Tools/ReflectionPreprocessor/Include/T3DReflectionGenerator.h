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

#ifndef __T3D_REFLECTION_GENERATOR_H__
#define __T3D_REFLECTION_GENERATOR_H__


#include "T3DRPPrerequisites.h"
#include "T3DPreprocessorOptions.h"
#include "T3DAbstractSyntaxTree.h"
#include "T3DPlatformGuardScanner.h"


namespace Tiny3D
{
    class ReflectionGenerator : public Allocator, public Singleton<ReflectionGenerator>
    {
    public:
        /// Constructor
        ReflectionGenerator();

        /// Destructor
        virtual ~ReflectionGenerator() override;

        /**
         * @brief 设置工程根目录
         */
        void setProjectPath(const String &projectPath)
        {
            mProjectPath = projectPath;
        }

        /**
         * @brief 分析项目头文件路径，获取包含头文件路径信息
         * @param [in] args : 编译选项
         */
        void parseProjectHeaderPath(const ClangArgs &args);

        /**
         * @brief 收集工程所有头文件
         * @param [in] path : 头文件完整路径
         */
        void collectProjectHeaders(const String &path);

        /**
         * @brief 生成 AST
         * @param [in] srcPath : 源码文件路径 
         * @param [in] args : clang 编译参数
         * @return 调用成功返回 T3D_OK
         */
        TResult generateAST(const String &srcPath, const ClangArgs &args);

        /**
         * @brief 生成源码
         */
        TResult generateSource(const String &generatedPath);

        /**
         * @brief 输出 AST 到 ast.json 文件
         */
        void dumpReflectionInfo(const String &path) const;

        /**
         * @brief 设置内置类，即不打标签也自动反射的类，主要是用于各种 stl 容器模板
         */
        void setBuiltinClass(const StringList &whitelist);

        TResult instantiateClassTemplate(ASTClassTemplate *klassTemplate, const String &name, const StringArray &formalParams, const StringArray &actualParams, const String &headerPath);
        
        /**
         * @brief 获取指定源文件的头文件依赖列表（完整路径）
         */
        const StringList& getFileDependencies(const String &srcTitle) const;

        /**
         * @brief 清除当前收集的依赖信息（每个翻译单元解析前调用）
         */
        void clearCurrentDependencies();

        /**
         * @brief 判断给定路径是否属于项目内头文件（在 -I 包含路径下）
         */
        bool isProjectFile(const String &filePath) const;

        /**
         * @brief 从另一个 generator 合并 AST 结果（用于多线程并行解析后的合并）
         * 将 other 中的 mSourceFiles、mHeaderFiles、mClassTemplates、
         * mFunctionTemplates、mFiles、mFileDependencies 合并到本实例
         */
        void mergeFrom(ReflectionGenerator &other);

        /// 预解析结果结构体，用于并行 parse 后串行 visit
        struct ParsedUnit
        {
            CXTranslationUnit   cxUnit {nullptr};
            String              srcPath {};
            TResult             result {0};
        };

        /**
         * @brief 只做 clang_parseTranslationUnit（线程安全，不依赖 generator 状态）
         * @param [in] srcPath : 源码文件路径
         * @param [in] args : clang 编译参数
         * @param [in] externalIndex : 外部提供的 CXIndex（用于复用 preamble 缓存）
         * @return ParsedUnit 供主线程串行执行 visitChildren
         */
        static ParsedUnit parseOnly(const String &srcPath, const ClangArgs &args, CXIndex externalIndex);

        /**
         * @brief 在已解析的 CXTranslationUnit 上执行 AST 遍历（必须在主线程调用）
         */
        TResult visitParsedUnit(ParsedUnit &unit);

        /**
         * @brief 生成预编译头文件（PCH）
         * @param [in] headerPath : 要预编译的头文件路径
         * @param [in] pchOutputPath : 输出 .pch 文件路径
         * @param [in] args : clang 编译参数
         * @return 成功返回 T3D_OK
         */
        static TResult generatePCH(const String &headerPath, const String &pchOutputPath, const ClangArgs &args);

    protected:
        String toString(const CXString &s) const
        {
            const char *str = clang_getCString(s);
            String result(str);
            clang_disposeString(s);
            return result;
        }

        struct ASTTypeAlias
        {
            ASTTypeAlias()
            {
                cxCursor = clang_getNullCursor();
            }
            
            ASTStruct *Klass {nullptr};
            CXCursor cxCursor;
        };
        
        StringList split(const String &str) const;

        /// 整文件读成内存字节，读不到返回 false
        static bool readWholeFile(const String &path, String &content);

        /**
         * @brief 把刚生成好的临时文件落成正式产物
         * @param [in] tempPath : 刚写完的临时文件
         * @param [in] path : 正式产物路径
         * @remarks 内容与现有产物一致时直接丢掉临时文件，不去动正式产物的修改时间。
         *          rpp 只要重解析过某个 .cpp 就必定重写一遍它的产物，而重写出来的内容
         *          往往一模一样；真让修改时间跳一下，下游 .cpp 会跟着重新编译再重新
         *          链接，跨构建入口切换时这就是一次「什么都没改却全量重编」
         */
        static void commitGeneratedFile(const String &tempPath, const String &path);

        CXChildVisitResult visitRootChildren(CXCursor cxCursor, CXCursor cxParent, ASTNode *parent);

        CXChildVisitResult visitClassChildren(CXCursor cxCursor, CXCursor cxParent, ASTStruct *parent);

        CXChildVisitResult visitEnumChildren(CXCursor cxCursor, CXCursor cxParent, ASTEnum *parent);

        CXChildVisitResult visitFunctionChildren(CXCursor cxCursor, CXCursor cxParent, ASTFunction *parent);

        CXChildVisitResult visitVariableChildren(CXCursor cxCursor, CXCursor cxParent, ASTProperty *parent);

        CXChildVisitResult visitTypeAliasTemplateDeclChildren(CXCursor cxCursor, CXCursor cxParent, const String &alias);

        TResult processClassDeclaration(CXCursor cxCursor, CXCursor cxParent, bool isClass, bool isTemplate);

        TResult processClassBaseSpecifier(CXCursor cxCursor, CXCursor cxParent, ASTNode *parent);

        TResult processFunctionDeclaration(CXCursor cxCursor, CXCursor cxParent, bool isCXXMember, bool isConstructor, bool isDestructor, bool isTemplate);

        TResult processEnumDeclaration(CXCursor cxCursor, CXCursor cxParent);

        TResult processEnumConstDeclaration(CXCursor cxCursor, CXCursor cxParent, ASTNode *parent);

        TResult processVariableDeclaration(CXCursor cxCursor, CXCursor cxParent, bool isCXXMember);

        TResult processTemplateParameter(CXCursor cxCursor, CXCursor cxParent, ASTNode *parent);

        TResult processDefaultArguments(CXCursor cxCursor, CXCursor cxParent, ASTFunction *parent, const String &defaultName);

        // TResult processOverloadDeclaration(CXCursor cxCursor, CXCursor cxParent);

        /// 处理宏
        TResult processMacroExpansion(CXCursor cxCursor, CXCursor cxParent);

        /// 处理反射标签宏
        TResult processMacroTags(const String &name, CXCursor cxCursor, CXCursor cxParent);

        /// 处理反射开关宏
        TResult processMacroSwitch(const String &name, CXCursor cxCursor, CXCursor cxParent);

        /// 处理反射友元宏
        TResult processMacroFriend(const String &name, CXCursor cxCursor, CXCursor cxParent);

        /// 处理包含的头文件
        TResult processInclusionDirective(const String &name, CXCursor cxCursor, CXCursor cxParent);

        TResult processTypeAliasTemplateDecl(CXCursor cxCursor, CXCursor cxParent);

        void getASTNodeInfo(CXCursor cxCursor, String &filePath, uint32_t &start, uint32_t &end, uint32_t &column, uint32_t &offset) const;

        ASTNode *createNode(const ASTNodeInfo &info) const;

        ASTNode *getOrConstructParentNode(CXCursor cxCursor);

        ASTNode *getASTNode(CXCursor cxCursor);

        ASTTypeAlias getASTNode(CXType cxType);

        void insertSourceFiles(const String &path, ASTNode *node, bool isTemplate, const StringList &pathes = StringList());

        void insertClassTemplate(const String &name, ASTClassTemplate *klass);

        void insertFunctionTemplate(const String &name, ASTOverloadFunction *function);

        TResult instantiateClassTemplate(CXCursor cxCursor, const String &headerPath);

        TResult instantiateFunctionTemplate(CXCursor cxCursor);

    protected:
        struct ClientData
        {
            void *parent;
            ReflectionGenerator *generator;
        };
        
        struct RTTISwitch : public Allocator
        {
            RTTISwitch()
                : enabled(false)
            {}
            
            bool            enabled;        /// 是否开启 RTTR
            TList<String>   baseClasses;    /// 基类列表
        };

        typedef std::shared_ptr<RTTISwitch> RTTISwitchPtr;
        typedef TMap<uint32_t, RTTISwitchPtr> RTTISwitches;
        typedef RTTISwitches::value_type RTTISwitchesValue;

        typedef TMap<uint32_t, bool> RTTIFriends;
        typedef RTTIFriends::value_type RTTIFriendsValue;
        
        struct FileReflectionInfo : public Allocator
        {
            FileReflectionInfo() = default;
            
            Specifiers      structs;
            Specifiers      classes;
            Specifiers      functions;
            Specifiers      properties;
            Specifiers      enumerations;
            RTTISwitches    switches;   /// 是否开启 RTTI 功能
            RTTIFriends     friends;    /// 是否对 RTTI 开启友元
        };

        typedef std::shared_ptr<FileReflectionInfo> FileReflectionInfoPtr;

        using Files = TMap<String, FileReflectionInfoPtr>;
        using FilesItr = Files::iterator;
        using FilesConstItr = Files::const_iterator;
        using FilesValue = Files::value_type;

        using ASTNodeMap = TMap<String, ASTNode*>;
        using ASTNodeMapValue = ASTNodeMap::value_type;
        
        using SourceFilesMap = TMap<String, ASTNodeMap>;
        using SourceFilesMapValue = SourceFilesMap::value_type;

        using ASTClassTemplateMap = TMap<String, ASTClassTemplate*>;
        using ASTClassTemplateMapValue = ASTClassTemplateMap::value_type;

        using ASTFunctionTemplateMap = TMap<String, ASTOverloadFunction*>;
        using ASTFunctionTemplateMapValue = ASTFunctionTemplateMap::value_type;

        using HeaderFilesMap = TMap<String, StringList>;
        using HeaderFilesMapValue = HeaderFilesMap::value_type;

        using ASTWhiteList = TMap<String, String>;
        using ASTWhiteListItr = ASTWhiteList::iterator;
        using ASTWhiteListConstItr = ASTWhiteList::const_iterator;
        using ASTWhiteListValue = ASTWhiteList::value_type;

        using ASTTypeAliasMap = TUnorderedMap<String, ASTTypeAlias>;

        bool isRTTIFriend(FileReflectionInfoPtr info, uint32_t start, uint32_t end) const;

        /**
         * @brief 从声明行往上回溯查找反射标签
         * @param [in] filePath : 声明所在文件路径
         * @param [in] specifiers : 按标签行号索引的标签表
         * @param [in] declLine : 声明起始行号
         * @param [out] itrSpec : 命中的标签迭代器
         * @return 找到返回 true
         * @remarks 标签与声明之间只允许夹注释与空行，遇到其它代码即停止回溯
         */
        bool findSpecifierUpward(const String &filePath, Specifiers &specifiers,
            uint32_t declLine, SpecifiersItr &itrSpec);

        /// 取文件的逐行「空行或注释行」标记表，下标即行号（0 位占位），惰性构建
        const TArray<bool> &getSkippableLines(const String &filePath);

        /// 文件路径 -> 逐行空行/注释标记，供反射标签回溯使用
        TMap<String, TArray<bool>> mSkippableLines {};

        /// 类型别名映射表
        ASTTypeAliasMap         mTypeAliasMap {};
        /// 白名单类列表
        ASTWhiteList            mClassWhiteList {};
        /// 函数模板集合
        ASTFunctionTemplateMap  mFunctionTemplates {};
        /// 类模板集合
        ASTClassTemplateMap     mClassTemplates {};
        /// 源码集合
        SourceFilesMap          mSourceFiles {};
        /// 项目头文件
        HeaderFilesMap          mHeaderFiles {};
        /// 项目头文件包含路径
        StringList              mIncludePathes {};
        /// 带反射信息的文件集合
        Files                   mFiles {};
        /// AST 根结点
        ASTNode                 *mRoot {nullptr};
        /// 复用的 CXIndex（构造时创建，析构时销毁）
        CXIndex                 mCxIndex {nullptr};
        /// 工程根目录
        String                  mProjectPath {};
        /// 每个源文件的头文件完整路径依赖列表 (key = 源文件 title)
        using DependenciesMap = TMap<String, StringList>;
        DependenciesMap         mFileDependencies {};
        /// 当前正在解析的源文件的 title
        String                  mCurrentSrcTitle {};

    public:
        /// 模板实例化持久化记录
        struct TemplateInstInfo
        {
            String sourceFilePath;   /// insertSourceFiles 的 key（头文件路径）
            String hierarchyName;    /// 模板实例化的全类型名（如 std::vector<Tiny3D::Buffer>）
            StringList headerPaths;  /// 该实例化需要的 include 头文件绝对路径列表
        };

        using TemplateInstList = std::vector<TemplateInstInfo>;
        using TemplateInstMap = TMap<String, TemplateInstList>;  // key = srcTitle

        /**
         * @brief 获取指定 cpp 的模板实例化记录
         */
        const TemplateInstList& getTemplateInstantiations(const String &srcTitle) const;

        /**
         * @brief 将从 .tpl 文件加载的模板实例化信息注入到 mSourceFiles/mHeaderFiles 中
         */
        void injectTemplateInstantiations(const TemplateInstList &instList);

        /**
         * @brief 获取 AST 节点所在头文件的条件编译守卫
         * @param [in] filePath : 头文件路径
         * @param [in] line : 声明所在行号
         * @return 条件编译守卫字符串，空表示无守卫
         */
        String queryPlatformGuard(const String &filePath, uint32_t line);

    protected:
        /// per-cpp 的模板实例化记录（在 instantiateClassTemplate 成功时填充）
        TemplateInstMap         mTemplateInstantiations {};
        /// 条件编译守卫扫描器
        PlatformGuardScanner    mGuardScanner {};
    };
}


#endif  /*__T3D_REFLECTION_GENERATOR_H__*/
