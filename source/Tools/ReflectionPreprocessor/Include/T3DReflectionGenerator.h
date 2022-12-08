/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
        
    protected:
        String toString(const CXString &s) const
        {
            const char *str = clang_getCString(s);
            String result(str);
            clang_disposeString(s);
            return result;
        }

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

        // TResult processOverloadDeclaration(CXCursor cxCursor, CXCursor cxParent);

        /// 处理宏
        TResult processMacroExpansion(CXCursor cxCursor, CXCursor cxParent);

        /// 处理反射标签宏
        TResult processMacroTags(const String &name, CXCursor cxCursor, CXCursor cxParent);

        /// 处理反射开关宏
        TResult processMacroSwitch(const String &name, CXCursor cxCursor, CXCursor cxParent);

        void getASTNodeInfo(CXCursor cxCursor, String &filePath, uint32_t &start, uint32_t &end, uint32_t &column, uint32_t &offset) const;

        ASTNode *createNode(const ASTNodeInfo &info) const;

        ASTNode *getOrConstructParentNode(CXCursor cxCursor);

        void insertSourceFiles(const String &path, ASTNode *node);

        void insertClassTemplate(const String &name, ASTClassTemplate *klass);

        void insertFunctionTemplate(const String &name, ASTOverloadFunction *function);

        TResult instantiateClassTemplate(CXCursor cxCursor);

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
            
            bool            enabled;
            TList<String>   baseClasses;
        };

        typedef std::shared_ptr<RTTISwitch> RTTISwitchPtr;
        typedef TMap<uint32_t, RTTISwitchPtr> RTTISwitches;
        typedef RTTISwitches::value_type RTTISwitchesValue;
        
        struct FileReflectionInfo
        {
            Specifiers      structs;
            Specifiers      classes;
            Specifiers      functions;
            Specifiers      properties;
            Specifiers      enumerations;
            RTTISwitches    switches;   /// 是否开启 RTTI 功能
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

        typedef TMap<String, String> HeaderFilesMap;
        typedef HeaderFilesMap::value_type HeaderFilesMapValue;

        ASTFunctionTemplateMap  mFunctionTemplates; /// 函数模板集合
        ASTClassTemplateMap     mClassTemplates;    /// 类模板集合
        SourceFilesMap          mSourceFiles;       /// 源码集合
        HeaderFilesMap          mHeaderFiles;       /// 项目头文件
        StringList              mIncludePathes;     /// 项目头文件包含路径
        Files                   mFiles;             /// 带反射信息的文件集合
        ASTNode                 *mRoot;             /// AST 根结点
    };
}


#endif  /*__T3D_REFLECTION_GENERATOR_H__*/
