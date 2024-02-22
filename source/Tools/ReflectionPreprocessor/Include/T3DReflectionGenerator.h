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
    class ReflectionGenerator : public Noncopyable
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
        
    protected:
        String toString(const CXString &s) const
        {
            const char *str = clang_getCString(s);
            String result(str);
            clang_disposeString(s);
            return result;
        }

        StringList split(const String &str) const;

        CXChildVisitResult visitRootChildren(CXCursor cxCursor, CXCursor cxParent, ASTNode *parent);

        CXChildVisitResult visitClassChildren(CXCursor cxCursor, CXCursor cxParent, ASTStruct *parent);

        CXChildVisitResult visitEnumChildren(CXCursor cxCursor, CXCursor cxParent, ASTEnum *parent);

        CXChildVisitResult visitFunctionChildren(CXCursor cxCursor, CXCursor cxParent, ASTFunction *parent);

        CXChildVisitResult visitVariableChildren(CXCursor cxCursor, CXCursor cxParent, ASTProperty *parent);

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

        void getASTNodeInfo(CXCursor cxCursor, String &filePath, uint32_t &start, uint32_t &end, uint32_t &column, uint32_t &offset) const;

        ASTNode *createNode(const ASTNodeInfo &info) const;

        ASTNode *getOrConstructParentNode(CXCursor cxCursor);

        void insertSourceFiles(const String &path, ASTNode *node, bool isTemplate, const StringList &pathes = StringList());

        void insertClassTemplate(const String &name, ASTClassTemplate *klass);

        void insertFunctionTemplate(const String &name, ASTOverloadFunction *function);

        TResult instantiateClassTemplate(CXCursor cxCursor, const String &headerPath);

        TResult instantiateFunctionTemplate(CXCursor cxCursor);

    protected:
        struct ClientData
        {
            ASTNode *parent;
            ReflectionGenerator *generator;
        };
        
        struct RTTISwitch
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
        
        struct FileReflectionInfo
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

        typedef TMap<String, FileReflectionInfoPtr> Files;
        typedef Files::iterator FilesItr;
        typedef Files::const_iterator FilesConstItr;
        typedef Files::value_type FilesValue;

        typedef TMap<String, ASTNode*> ASTNodeMap;
        typedef ASTNodeMap::value_type ASTNodeMapValue;
        
        typedef TMap<String, ASTNodeMap> SourceFilesMap;
        typedef SourceFilesMap::value_type SourceFilesMapValue;

        typedef TMap<String, ASTClassTemplate*> ASTClassTemplateMap;
        typedef ASTClassTemplateMap::value_type ASTClassTemplateMapValue;

        typedef TMap<String, ASTOverloadFunction*> ASTFunctionTemplateMap;
        typedef ASTFunctionTemplateMap::value_type ASTFunctionTemplateMapValue;

        typedef TMap<String, StringList> HeaderFilesMap;
        typedef HeaderFilesMap::value_type HeaderFilesMapValue;

        typedef TMap<String, String> ASTWhiteList;
        typedef ASTWhiteList::iterator ASTWhiteListItr;
        typedef ASTWhiteList::const_iterator ASTWhiteListConstItr;
        typedef ASTWhiteList::value_type ASTWhiteListValue;

        bool isRTTIFriend(FileReflectionInfoPtr info, uint32_t start, uint32_t end) const;

        ASTWhiteList            mClassWhiteList;    /// 白名单类列表
        ASTFunctionTemplateMap  mFunctionTemplates; /// 函数模板集合
        ASTClassTemplateMap     mClassTemplates;    /// 类模板集合
        SourceFilesMap          mSourceFiles;       /// 源码集合
        HeaderFilesMap          mHeaderFiles;       /// 项目头文件
        StringList              mIncludePathes;     /// 项目头文件包含路径
        Files                   mFiles;             /// 带反射信息的文件集合
        ASTNode                 *mRoot;             /// AST 根结点
        String                  mProjectPath;       /// 工程根目录
    };
}


#endif  /*__T3D_REFLECTION_GENERATOR_H__*/
