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
        /// 工程根目录
        String                  mProjectPath {};
    };
}


#endif  /*__T3D_REFLECTION_GENERATOR_H__*/
