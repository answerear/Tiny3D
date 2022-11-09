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

#include "T3DReflectionGenerator.h"
#include "T3DRPErrorCode.h"

namespace Tiny3D
{
    //-------------------------------------------------------------------------

    #define CHECK_TAG_RET_FILE_SPEC(RESULT, ITR_FILE, ITR_SPEC, CURSOR, SPECIFIERS, PATH, START, END, COL, OFFSET)   \
    {   \
        (RESULT) = false; \
        getASTNodeInfo(CURSOR, PATH, START, END, COL, OFFSET); \
        FilesItr _itrFile = mFiles.find(PATH);   \
        if (_itrFile != mFiles.end())    \
        {   \
            (ITR_FILE) = _itrFile; \
            FileReflectionInfoPtr info = _itrFile->second;   \
            uint32_t tagLine = (START) - 1;   \
            for (uint32_t _idx = tagLine; _idx >= tagLine - 3; _idx--)  \
            {   \
                const auto &_itrSpec = info->SPECIFIERS.find(tagLine);    \
                if (_itrSpec != info->SPECIFIERS.end())    \
                {   \
                    (RESULT) = true;  \
                    (ITR_SPEC) = _itrSpec;  \
                    break;  \
                }   \
            }   \
        }   \
    }

    #define CHECK_TAG_RET_SPEC(RESULT, ITR_SPEC, CURSOR, SPECIFIERS, PATH, START, END, COL, OFFSET)    \
        {   \
            FilesItr itFile;   \
            CHECK_TAG_RET_FILE_SPEC(RESULT, itFile, ITR_SPEC, CURSOR, SPECIFIERS, PATH, START, END, COL, OFFSET); \
        }

    #define CHECK_TAG(RESULT, CURSOR, SPECIFIERS, PATH, START, END, COL, OFFSET) \
        {   \
            SpecifiersConstItr itSpec; \
            CHECK_TAG_RET_SPEC(RESULT, itSpec, CURSOR, SPECIFIERS, PATH, START, END, COL, OFFSET); \
        }
    
    //-------------------------------------------------------------------------

    #define DEFAULT_ROOT_NAME    "___###ASTRoot###___"
    
    ReflectionGenerator::ReflectionGenerator()
        : mRoot(new ASTNode(DEFAULT_ROOT_NAME))
    {
    }
    
    //-------------------------------------------------------------------------

    ReflectionGenerator::~ReflectionGenerator()
    {
        delete mRoot;
    }

    //-------------------------------------------------------------------------

    TResult ReflectionGenerator::generateAST(const String &srcPath, const ClangArgs &args)
    {
        TResult ret = T3D_OK;

        do
        {
            RP_LOG_INFO("[%s] Generating reflection source file ...", srcPath.c_str());

            mSourceFiles.insert(SourceFileMapValue(srcPath, ASTNodeMap()));
            
            CXIndex cxIndex = clang_createIndex(0, 0);

            CXTranslationUnit cxUnit = clang_parseTranslationUnit(cxIndex, srcPath.c_str(),
                args.data(), args.size(), nullptr, 0,
                CXTranslationUnit_DetailedPreprocessingRecord | CXTranslationUnit_SkipFunctionBodies);
            if (cxUnit == nullptr)
            {
                RP_LOG_ERROR("Parse source file [%s] failed !", srcPath.c_str());
                ret = T3D_ERR_RP_PARSE_SOURCE;
                break;
            }

            const auto cxNumDiag = clang_getNumDiagnostics(cxUnit);
            if (cxNumDiag != 0)
            {
                bool hasErrors = false;
                RP_LOG_ERROR("> Diagnostics:");
                for (uint32_t i = 0; i < cxNumDiag; ++i)
                {
                    const auto cxDiag = clang_getDiagnostic(cxUnit, i);
                    RP_LOG_ERROR(">>> %s", toString(clang_formatDiagnostic(cxDiag, clang_defaultDiagnosticDisplayOptions())).c_str());
                    CXDiagnosticSeverity cxSeverity = clang_getDiagnosticSeverity(cxDiag);
                    if (cxSeverity == CXDiagnostic_Error || cxSeverity == CXDiagnostic_Fatal)
                    {
                        hasErrors = true;
                    }
                    clang_disposeDiagnostic(cxDiag);
                }

                if (hasErrors)
                {
                    ret = T3D_ERR_RP_COMPILE_ERROR;
                    break;
                }
                else
                {
                    ret = T3D_ERR_RP_COMPILE_WARNING;
                }
            }
        
            auto cxCursor = clang_getTranslationUnitCursor(cxUnit);
			
            clang_visitChildren(
                cxCursor,
                [](CXCursor cxCursor, CXCursor cxParent, CXClientData cxData)
                {
                    auto *rp = static_cast<ReflectionGenerator*>(cxData);
                    return rp->visitChildren(cxCursor, cxParent);
                },
                this);
			
            clang_disposeTranslationUnit(cxUnit);
            
            clang_disposeIndex(cxIndex);

            RP_LOG_INFO(">---- [%s] Completed ! ----", srcPath.c_str());
        } while (false);

        return ret;
    }

    //-------------------------------------------------------------------------

    CXChildVisitResult ReflectionGenerator::visitChildren(CXCursor cxCursor, CXCursor cxParent)
    {
        CXCursorKind cxKind = clang_getCursorKind(cxCursor);

        CXString cxName = clang_getCursorSpelling(cxCursor);
        String name = toString(cxName);
        CXString cxType = clang_getCursorKindSpelling(cxKind);
        String type = toString(cxType);

        // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());

        switch (cxKind)
        {
        case CXCursor_StructDecl:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                processClassDeclaration(cxCursor, cxParent, false, false);
            }
            break;
        case CXCursor_ClassDecl:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                processClassDeclaration(cxCursor, cxParent, true, false);
            }
            break;
        case CXCursor_EnumDecl:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                processEnumDeclaration(cxCursor, cxParent);
            }
            break;
        case CXCursor_FieldDecl:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                processVariableDeclaration(cxCursor, cxParent, true);
            }
            break;
        case CXCursor_EnumConstantDecl:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                processEnumConstDeclaration(cxCursor, cxParent);
            }
            break;
        case CXCursor_FunctionDecl:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                processFunctionDeclaration(cxCursor, cxParent, false, false, false);
            }
            break;
        case CXCursor_VarDecl:
            {
                processVariableDeclaration(cxCursor, cxParent, false);
            }
            break;
        case CXCursor_CXXMethod:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                processFunctionDeclaration(cxCursor, cxParent, true, false, false);
            }
            break;
        case CXCursor_Namespace:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                // processNamespace(cxCursor, cxParent);
            }
            break;
        case CXCursor_Constructor:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                processFunctionDeclaration(cxCursor, cxParent, true, true, false);
            }
            break;
        case CXCursor_Destructor:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                processFunctionDeclaration(cxCursor, cxParent, true, false, true);
            }
            break;
        case CXCursor_TemplateTypeParameter:
            {
                RP_LOG_INFO("CXCursor_TemplateTypeParameter %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
            }
            break;
        case CXCursor_NonTypeTemplateParameter:
            {
                RP_LOG_INFO("CXCursor_NonTypeTemplateParameter %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
            }
            break;
        case CXCursor_TemplateTemplateParameter:
            {
                RP_LOG_INFO("CXCursor_TemplateTemplateParameter %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
            }
            break;
        case CXCursor_FunctionTemplate:
            {
                RP_LOG_INFO("CXCursor_FunctionTemplate %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
            }
            break;
        case CXCursor_ClassTemplate:
            {
                RP_LOG_INFO("CXCursor_ClassTemplate %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
            }
            break;
        case CXCursor_ClassTemplatePartialSpecialization:
            {
                RP_LOG_INFO("CXCursor_ClassTemplatePartialSpecialization %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
            }
            break;
        case CXCursor_CXXBaseSpecifier:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                processClassBaseSpecifier(cxCursor, cxParent);
            }
            break;
        case CXCursor_MacroExpansion:
            {
                processMacroExpansion(cxCursor, cxParent);
            }
            break;
        default:
            {
            }
            break;
        }
		
        return CXChildVisit_Recurse;
    }
    
    //-------------------------------------------------------------------------

    TResult ReflectionGenerator::processMacroExpansion(CXCursor cxCursor, CXCursor cxParent)
    {
        TResult ret = T3D_OK;

        do
        {
            CXString cxName = clang_getCursorSpelling(cxCursor);
            String name = toString(cxName);

            if (name == kMacroClass || name == kMacroEnum
                || name == kMacroFunction || name == kMacroProperty
                || name == kMacroStruct)
            {
                // 反射标签
                ret = processMacroTags(name, cxCursor, cxParent);
            }
            else if (name == kRTTIEnable)
            {
                // 反射开关
                ret = processMacroSwitch(name, cxCursor, cxParent);
            }
        } while (false);
        
        return ret;
    }

    //-------------------------------------------------------------------------

    TResult ReflectionGenerator::processMacroTags(const String &name,
        CXCursor cxCursor, CXCursor cxParent)
    {
        TResult ret = T3D_OK;

        do
        {
            TStack<String> stack;
            
            String path;
            uint32_t start, end, column, offset;
            // 获取反射标签的源码文件、行数等信息
            getASTNodeInfo(cxCursor, path, start, end, column, offset);

            FileReflectionInfoPtr info;
            auto itr = mFiles.find(path);
            if (itr == mFiles.end())
            {
                // 该文件第一次解析，缓存起来
                info.reset(new FileReflectionInfo());
                mFiles.insert(FilesValue(path, info));
            }
            else
            {
                info = itr->second;
            }
            
            Specifiers *smap = nullptr;
            
            if (name == kMacroClass)
            {
                smap = &info->classes;
            }
            else if (name == kMacroEnum)
            {
                smap = &info->enumerations;
            }
            else if (name == kMacroFunction)
            {
                smap = &info->functions;
            }
            else if (name == kMacroProperty)
            {
                smap = &info->properties;
            }
            else if (name == kMacroStruct)
            {
                smap = &info->structs;
            }
            
            TList<Specifier> specifiers;

            // 解析符号，提取反射标签信息
            CXSourceRange cxRange = clang_getCursorExtent(cxCursor);
            CXTranslationUnit cxUnit = clang_Cursor_getTranslationUnit(cxCursor);
            CXToken *cxTokens;
            uint32_t cxNumTokens = 0;
            clang_tokenize(cxUnit, cxRange, &cxTokens, &cxNumTokens);
            for (uint32_t i = 1; i < cxNumTokens; ++i)
            {
                CXTokenKind cxTokenKind = clang_getTokenKind(cxTokens[i]);
                CXString cxStr = clang_getTokenSpelling(cxUnit, cxTokens[i]);
                String token = toString(cxStr);
                
                switch (cxTokenKind)
                {
                case CXToken_Punctuation:
                    {
                        // RP_LOG_INFO("Token(punctuation) : %s", token.c_str());
                        if (token == "(")
                        {
                            // Do nothing
                        }
                        else if (token == "=")
                        {
                            // the key-value splitter
                            stack.push(token);
                        }
                        else if (token == "," || token == ")" && !stack.empty())
                        {
                            // At the end of specifier
                            Specifier spec;
                            String t1 = stack.top();
                            stack.pop();
                            if (!stack.empty() && stack.top() == "=")
                            {
                                String t2 = stack.top();
                                stack.pop();
                                String key = stack.top();
                                stack.pop();
                                
                                // the temp is value of specifier
                                Specifier specifier;
                                specifier.value = t1;
                                specifier.name = key;
                                specifiers.push_back(std::move(specifier));
                            }
                            else
                            {
                                // the temp is name of specifier and empty value of specifier
                                Specifier specifier;
                                specifier.name = t1;
                                specifiers.push_back(std::move(specifier));
                            }
                        }
                        else if (!stack.empty())
                        {
                            // Errors
                            RP_LOG_ERROR("Reflection tag (%s:%s) [%s:%u] syntax error !",
                                name.c_str(), token.c_str(), path.c_str(), start);
                            ret = T3D_ERR_RP_TAG_SYNTAX;
                        }
                    }
                    break;
                case CXToken_Keyword:
                    {
                        RP_LOG_INFO("Token(keyword) : %s", token.c_str());
                    }
                    break;
                case CXToken_Identifier:
                    {
                        // RP_LOG_INFO("Token(identifier) : %s", token.c_str());
                        stack.push(token);
                    }
                    break;
                case CXToken_Literal:
                    {
                        // RP_LOG_INFO("Token(literal) : %s", token.c_str());
                        stack.push(token);
                    }
                    break;
                case CXToken_Comment:
                    {
                        // RP_LOG_INFO("Token(comment) : %s", token.c_str());
                    }
                    break;
                }

                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            if (T3D_FAILED(ret))
            {
                break;
            }

            // 把标签记录下来，表示这个对象使用了反射
            CXString cxType = clang_getCursorKindSpelling(CXCursor_MacroExpansion);
            String type = toString(cxType);
            
            // RP_LOG_INFO("%s : %s, %s[%u]", type.c_str(), name.c_str(), path.c_str(), start);
            clang_disposeTokens(cxUnit, cxTokens, cxNumTokens);

            smap->insert(SpecifiersValue(start, std::move(specifiers)));
        } while (false);

        return ret;
    }

    //-------------------------------------------------------------------------

    TResult ReflectionGenerator::processMacroSwitch(const String &name, CXCursor cxCursor, CXCursor cxParent)
    {
        TResult ret = T3D_OK;

        do
        {
            String path;
            uint32_t start, end, column, offset;
            // 获取反射标签的源码文件、行数等信息
            getASTNodeInfo(cxCursor, path, start, end, column, offset);

            FileReflectionInfoPtr info;
            auto itrFile = mFiles.find(path);
            if (itrFile == mFiles.end())
            {
                // 该文件第一次解析，缓存起来
                info.reset(new FileReflectionInfo());
                mFiles.insert(FilesValue(path, info));
            }
            else
            {
                info = itrFile->second;
            }

            TList<String> baseClasses;
            TStack<String> stack;
            
            // 解析符号，提取反射标签信息
            CXSourceRange cxRange = clang_getCursorExtent(cxCursor);
            CXTranslationUnit cxUnit = clang_Cursor_getTranslationUnit(cxCursor);
            CXToken *cxTokens;
            uint32_t cxNumTokens = 0;
            clang_tokenize(cxUnit, cxRange, &cxTokens, &cxNumTokens);
            for (uint32_t i = 1; i < cxNumTokens; ++i)
            {
                CXTokenKind cxTokenKind = clang_getTokenKind(cxTokens[i]);
                CXString cxStr = clang_getTokenSpelling(cxUnit, cxTokens[i]);
                String token = toString(cxStr);
                
                switch (cxTokenKind)
                {
                case CXToken_Punctuation:
                    {
                        // RP_LOG_INFO("Token(punctuation) : %s", token.c_str());
                        if (token == "(")
                        {
                            // Do nothing
                        }
                        else if (token == "," || token == ")" && stack.size() == 1)
                        {
                            // At the end of identifier
                            Specifier spec;
                            String klass = stack.top();
                            stack.pop();
                            baseClasses.push_back(klass);
                        }
                        else if (token == "::")
                        {
                            stack.top().append(token);
                        }
                        else if (!stack.empty())
                        {
                            // Errors
                            RP_LOG_ERROR("Reflection tag (%s:%s) [%s:%u] syntax error !",
                                name.c_str(), token.c_str(), path.c_str(), start);
                            ret = T3D_ERR_RP_TAG_SYNTAX;
                        }
                    }
                    break;
                case CXToken_Keyword:
                    {
                        RP_LOG_INFO("Token(keyword) : %s", token.c_str());
                    }
                    break;
                case CXToken_Identifier:
                    {
                        // RP_LOG_INFO("Token(identifier) : %s", token.c_str());
                        if (stack.empty())
                        {
                            stack.push(token);
                        }
                        else
                        {
                            stack.top().append(token);
                        }
                    }
                    break;
                case CXToken_Literal:
                    {
                        // RP_LOG_INFO("Token(literal) : %s", token.c_str());
                        if (stack.empty())
                        {
                            stack.push(token);
                        }
                        else
                        {
                            stack.top().append(token);
                        }
                    }
                    break;
                case CXToken_Comment:
                    {
                        // RP_LOG_INFO("Token(comment) : %s", token.c_str());
                    }
                    break;
                }

                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            if (T3D_FAILED(ret))
            {
                break;
            }

            RTTISwitchPtr sw(new RTTISwitch());
            sw->enabled = true;
            sw->baseClasses = std::move(baseClasses);

            info->switches.insert(RTTISwitchesValue(start, sw));
        } while (false);

        return ret;
    }

    //-------------------------------------------------------------------------

#if 1
    TResult ReflectionGenerator::processClassDeclaration(CXCursor cxCursor, CXCursor cxParent, bool isClass, bool isTemplate)
    {
        TResult ret = T3D_OK;

        do
        {
            // 先检测是否有打反射标签
            bool rval;
            FilesItr itrFile;
            SpecifiersItr itrSpec;
            String path;
            uint32_t start, end, column, offset;
            CHECK_TAG_RET_FILE_SPEC(rval, itrFile, itrSpec, cxCursor, classes, path, start, end, column, offset);
            if (!rval)
            {
                break;
            }

            // 检测反射开关是否打开
            bool RTTIEnabled = false;
            FileReflectionInfoPtr info = itrFile->second;
            TList<String> *baseClasses = nullptr;
            for (const auto &pair : info->switches)
            {
                if (pair.first >= start && pair.first <= end)
                {
                    // 开关在类内部
                    RTTIEnabled = pair.second->enabled;
                    baseClasses = &pair.second->baseClasses;
                    break;
                }
            }

            if (!RTTIEnabled)
            {
                // 没有打开反射功能
                ret = T3D_ERR_RP_RTTI_DISABLED;
                RP_LOG_ERROR("RTTI has not enable [%s:%u] !", path.c_str(), start);
                break;
            }

            // 获取类名
            CXString cxName = clang_getCursorSpelling(cxCursor);
            String name = toString(cxName);

            // 获取父结点
            ASTNode *parent = getOrConstructParentNode(cxCursor);
            if (parent == nullptr)
            {
                // 父结点不存在
                ret = T3D_ERR_RP_AST_NO_PARENT;
                RP_LOG_ERROR("The parent of class %s is null [%s:%u] !",
                    name.c_str(), path.c_str(), start);
                break;
            }

            ASTNode *node = parent->getChild(name);
            if (node != nullptr)
            {
                // 如果自己已经在父结点下，则只记录类所在的文件信息
                if (isTemplate)
                {
                    insertClassTemplate(node->getHierarchyName(), static_cast<ASTClassTemplate*>(node));
                }
                else
                {
                    insertSourceFiles(path, node);
                }
                break;
            }
            
            // 新建 class 结点
            ASTStruct *klass = nullptr;
            if (isTemplate)
            {
                klass = new ASTClassTemplate(name);
            }
            else if (isClass)
            {
                klass = new ASTClass(name);
            }
            else
            {
                klass = new ASTStruct(name);
            }
            klass->FileInfo.Path = path;
            klass->FileInfo.StartLine = start;
            klass->FileInfo.EndLine = end;
            klass->RTTIEnabled = RTTIEnabled;
            klass->RTTIBaseClasses = baseClasses;
            klass->Specifiers = &itrSpec->second;

            // 把自己加到父结点上
            parent->addChild(name, klass);

            // 放到源码文件缓存中
            if (isTemplate)
            {
                insertClassTemplate(klass->getHierarchyName(), static_cast<ASTClassTemplate*>(klass));
            }
            else
            {
                insertSourceFiles(path, klass);
            }
        } while (false);
        
        return ret;
    }

#else
    TResult ReflectionGenerator::processClassDeclaration(CXCursor cxCursor, CXCursor cxParent)
    {
        TResult ret = T3D_OK;

        do
        {
            CXString cxName = clang_getCursorSpelling(cxCursor);
            String name = toString(cxName);
            
            CXSourceLocation cxLocation = clang_getCursorLocation(cxCursor);
            CXSourceRange cxRange = clang_getCursorExtent(cxCursor);
            
            CXFile cxFile;
            uint32_t start, end, column, offset;

            // 文件路径、起始行号，列号，文件中偏移
            CXSourceLocation cxStart = clang_getRangeStart(cxRange);
            clang_getFileLocation(cxStart, &cxFile, &start, &column, &offset);
            CXString cxPath = clang_File_tryGetRealPathName(cxFile);
            String path = toString(cxPath);

            auto itrFile = mFiles.find(path);
            if (itrFile == mFiles.end())
            {
                // 该类没有 TCLASS 标签
                break;
            }

            FileReflectionInfoPtr info = itrFile->second;
            
            uint32_t classTagLine = start - 1;
            auto itrClass = info->classes.find(classTagLine);
            if (itrClass == info->classes.end())
            {
                // 该类的 TCLASS 标签不符合语法规则，不是在 class 声明上一行
                break;
            }

            // 结束行号
            CXSourceLocation cxEnd = clang_getRangeEnd(cxRange);
            clang_getFileLocation(cxEnd, &cxFile, &end, &column, &offset);

            const std::function<void(int32_t,std::stringstream&)> formatTabs = [](int32_t tab, std::stringstream &ss)
            {
                for (int32_t i = 0; i < tab; i++)
                {
                    ss << "\t";
                }
            };

            const std::function<void(CXCursor, CXCursorKind, std::stringstream&)> formatCursorInfo = [this](CXCursor cxCursor, CXCursorKind cxKind, std::stringstream &ss)
            {
                CXString cxStrName = clang_getCursorSpelling(cxCursor);
                String name = toString(cxStrName);
                CXString cxStrKind = clang_getCursorKindSpelling(cxKind);
                String kind = toString(cxStrKind);
                ss << "Kind : " << kind << ", Name : " << name;
            };

            std::stringstream ss;
            int32_t tab = 0;

            StringList names;
            CXCursor cxAncestor = cxCursor;//clang_getCursorSemanticParent(cxCursor);
            CXCursorKind cxAncestorKind = CXCursor_ClassDecl;//clang_getCursorKind(cxAncestor);
            while (!clang_equalCursors(cxAncestor, clang_getNullCursor()) && cxAncestorKind != CXCursor_TranslationUnit)
            {
                CXString cxAncestorName = clang_getCursorSpelling(cxAncestor);
                String ancestorName = toString(cxAncestorName);
                names.push_front(ancestorName);

                if (tab > 0)
                {
                    ss << std::endl;
                }

                formatTabs(tab, ss);
                formatCursorInfo(cxAncestor, cxAncestorKind, ss);
                tab++;
                // CXString cxAncestorType = clang_getCursorKindSpelling(cxAncestorKind);

                // String ancestorType = toString(cxAncestorType);
                // RP_LOG_INFO("%s, (parent %s : %s)", name.c_str(), ancestorType.c_str(), ancestorName.c_str());
                //
                T3D_ASSERT(cxAncestorKind == CXCursor_Namespace || cxAncestorKind == CXCursor_ClassDecl || cxAncestorKind == CXCursor_StructDecl);
                
                cxAncestor = clang_getCursorSemanticParent(cxAncestor);
                cxAncestorKind = clang_getCursorKind(cxAncestor);
            }

            RP_LOG_INFO("\n---- Declaration of Class ---- \n This class -> %s", ss.str().c_str());

            break;
            
            ASTClassPtr klass(new ASTClass());
            klass->FileInfo.Path = path;
            klass->FileInfo.StartLine = start;
            klass->FileInfo.EndLine = end;
            
            String rootName;

            if (!names.empty())
            {
                rootName = names.front();

                // 第一个是 root ，要去掉
                names.pop_front();  

                // 最后一个是自己，也要去掉
                if (!names.empty())
                {
                    names.pop_back();
                }
            }
            else
            {
                rootName = name;
            }
            
            if (names.empty())
            {
                auto itrRoot = mRoots.find(rootName);
                if (itrRoot == mRoots.end())
                {
                    // 没有现成的类结点，把自己作为根结点
                    mRoots.insert(ASTRootsValue(name, klass));
                    klass->HaveRTTI = true;
                }
                else
                {
                    itrRoot->second->HaveRTTI = true;
                    
                    if (rootName == name)
                    {
                        // 根结点就是跟当前结点是相同结点，什么都不做
                    }
                    else
                    {
                        // 根结点不是新结点，根结点作为父结点
                        ASTNodePtr parent = itrRoot->second;
                        ASTNodePtr node = parent->getChild(name);
                        if (node == nullptr)
                        {
                            // 根结点下没有该结点，创建一个
                            parent->addChild(name, klass);
                        }
                    }
                }
            }
            else
            {
                auto itrRoot = mRoots.find(rootName);
                if (itrRoot == mRoots.end())
                {
                    // 没有对应的根结点，出错了
                    RP_LOG_ERROR("Did not find the root node of class [%s] in AST !", name.c_str());
                    ret = T3D_ERR_NOT_FOUND;
                    break;
                }

                // 递归找到最终的子结点
                ASTNodePtr parent = itrRoot->second->getChild(names);
                if (parent == nullptr)
                {
                    // 出错了
                    RP_LOG_ERROR("Did not find the parent node of class [%s] in AST !", name.c_str());
                    ret = T3D_ERR_NOT_FOUND;
                    break;
                }
                
                itrRoot->second->HaveRTTI = true;
                parent->addChild(name, klass);
            }
            
            // CXCursorKind cxParentKind = clang_getCursorKind(cxParent);
            // CXString cxParentName = clang_getCursorSpelling(cxParent);
            // String parentName = toString(cxParentName);
            // CXString cxParentType = clang_getCursorKindSpelling(cxParentKind);
            // String parentType = toString(cxParentType);
            //
            // ASTNodePtr parent;
            // if (cxParentKind == CXCursor_Namespace)
            // {
            //     // this class is in namespace
            //
            //     // 这里先放到命名空间缓存中
            //     ASTNamespacePtr ns;
            //     auto itrNS = mNamespaces.find(parentName);
            //     if (itrNS == mNamespaces.end())
            //     {
            //         ns.reset(new ASTNamespace());
            //         mNamespaces.insert(NamespacesValue(parentName, ns));
            //     }
            //     else
            //     {
            //         ns = itrNS->second;
            //     }
            //
            //     // 构造命名空间 AST 结点，作为类 AST 结点的父结点
            //     ASTNamespaceRefPtr nsref;
            //     ASTNodePtr child = info->root->getChild(parentName);
            //     if (child == nullptr)
            //     {
            //         nsref.reset(new ASTNamespaceRef());
            //         nsref->Namespace = ns;
            //
            //         ASTFilePtr root = std::static_pointer_cast<ASTFile>(info->root);
            //         ns->RefFiles.insert(ASTFilesValue(path, root));
            //
            //         // 命名空间在这个文件引用中的起始行号
            //         CXSourceRange cxParentRange = clang_getCursorExtent(cxParent);
            //         CXSourceLocation cxParentStart = clang_getRangeStart(cxParentRange);
            //         uint32_t parentStart, parentEnd;
            //         clang_getFileLocation(cxParentStart, &cxFile, &parentStart, &column, &offset);
            //         CXSourceLocation cxParentEnd = clang_getRangeEnd(cxRange);
            //         clang_getFileLocation(cxParentEnd, &cxFile, &parentEnd, &column, &offset);
            //         nsref->StartLine = parentStart;
            //         nsref->EndLine = parentEnd;
            //
            //         info->root->addChild(parentName, nsref);
            //     }
            //     else
            //     {
            //         T3D_ASSERT(child->getType() == ASTNode::Type::kNamespace);
            //         nsref = std::static_pointer_cast<ASTNamespaceRef>(child);
            //     }
            //
            //     parent = nsref;
            // }
            // else
            // {
            //     // no namespace, so the class parent ASTNode is the file.
            //     parent = info->root;
            // }

            // 生成类 AST 结点，放到 AST 中
            // ASTClassPtr klass(new ASTClass());
            // parent->addChild(name, klass);
            
            // CXString cxType = clang_getCursorKindSpelling(CXCursor_ClassDecl);
            // String type = toString(cxType);
            // RP_LOG_INFO("%s : %s (parent %s : %s), %s[%u - %u]", type.c_str(), name.c_str(), parentType.c_str(), parentName.c_str(), path.c_str(), start, end);
        } while (false);
        
        return ret;
    }
#endif
    
    //-------------------------------------------------------------------------

#if 1
    TResult ReflectionGenerator::processClassBaseSpecifier(CXCursor cxCursor, CXCursor cxParent)
    {
        TResult ret = T3D_OK;

        do
        {
            const std::function<ASTNode*(CXCursor)> getASTStructNode = [this](CXCursor cxCursor) -> ASTNode*
            {
                StringList names;
                CXCursor cxAncestor = cxCursor;
                CXCursorKind cxAncestorKind = clang_getCursorKind(cxAncestor);
                while (!clang_equalCursors(cxAncestor, clang_getNullCursor()) && cxAncestorKind != CXCursor_TranslationUnit)
                {
                    CXString cxAncestorName = clang_getCursorSpelling(cxAncestor);
                    String ancestorName = toString(cxAncestorName);
                    names.push_front(ancestorName);
                    cxAncestor = clang_getCursorSemanticParent(cxAncestor);
                    cxAncestorKind = clang_getCursorKind(cxAncestor);
                }

                ASTNode *node = mRoot->getChild(names);
                if (node == nullptr)
                {
                    // RP_LOG_ERROR("Cound not find the struct node : %s", names.back().c_str());
                    return nullptr;
                }

                if (node->getType() != ASTNode::Type::kClass && node->getType() != ASTNode::Type::kStruct)
                {
                    // 这里需要是结构体、类
                    RP_LOG_ERROR("Invalid AST type [%s] ! ", names.back().c_str());
                    return nullptr;
                }

                return node;
            };
            
            // 基类信息，获取基类及其作用域
            CXType cxType = clang_getCursorType(cxCursor);
            CXCursor cxBaseCursor = clang_getTypeDeclaration(cxType);
            ASTNode *node = getASTStructNode(cxBaseCursor);
            if (node == nullptr)
            {
                ret = T3D_ERR_RP_BASE_CLASS_NO_RTTI;
                break;
            }
            auto baseNode = dynamic_cast<ASTStruct*>(node);

            // 派生类信息
            node = getASTStructNode(cxParent);
            if (node == nullptr)
            {
                ret = T3D_ERR_NOT_FOUND;
                break;
            }
            
            auto derivedNode = dynamic_cast<ASTStruct*>(node);

            // 检测反射继承链是否跟实际父子类继承链相一致
            T3D_ASSERT(baseNode->RTTIEnabled);
            auto itr = std::find(derivedNode->RTTIBaseClasses->begin(), derivedNode->RTTIBaseClasses->end(), baseNode->getHierarchyName());
            if (itr == derivedNode->RTTIBaseClasses->end())
            {
                // 父类没有在使用 RTTR_ENABLE 打开反射功能
                ret = T3D_ERR_RP_BASE_CLASS_NO_RTTI;
                RP_LOG_ERROR("Base class has not enabled RTTI feature [%s:%u] ! "
                    "Please use TRTTI_ENABLE() in class declaration to enable this feature.",
                    baseNode->FileInfo.Path.c_str(), baseNode->FileInfo.StartLine);
                break;
            }

            // 都合法，放到基类列表中
            derivedNode->BaseClasses.insert(ASTBaseClassesValue(baseNode->getHierarchyName(), baseNode));
        } while (false);
        
        return ret;
    }

#else
    TResult ReflectionGenerator::processClassBaseSpecifier(CXCursor cxCursor, CXCursor cxParent)
    {
        TResult ret = T3D_OK;

        do
        {
            CXString cxName = clang_getCursorSpelling(cxCursor);
            String name = toString(cxName);
            CXString cxStrType = clang_getCursorKindSpelling(CXCursor_CXXBaseSpecifier);
            String type = toString(cxStrType);

            CXSourceRange cxRange = clang_getCursorExtent(cxCursor);
            
            CXFile cxFile;
            uint32_t start, end, column, offset;

            // file path, start line, column and offset in the file
            CXSourceLocation cxStart = clang_getRangeStart(cxRange);
            clang_getFileLocation(cxStart, &cxFile, &start, &column, &offset);
            CXString cxPath = clang_File_tryGetRealPathName(cxFile);
            String path = toString(cxPath);

            // end line
            CXSourceLocation cxEnd = clang_getRangeEnd(cxRange);
            clang_getFileLocation(cxEnd, &cxFile, &end, &column, &offset);

            CXCursor cxAncestor = cxParent;
            CXCursorKind cxAncestorKind = clang_getCursorKind(cxParent);
            if (cxAncestorKind != CXCursor_ClassDecl && cxAncestorKind != CXCursor_StructDecl)
                break;
            
            const std::function<void(int32_t,std::stringstream&)> formatTabs = [](int32_t tab, std::stringstream &ss)
            {
                for (int32_t i = 0; i < tab; i++)
                {
                    ss << "\t";
                }
            };

            const std::function<void(CXCursor, CXCursorKind, std::stringstream&)> formatCursorInfo = [this](CXCursor cxCursor, CXCursorKind cxKind, std::stringstream &ss)
            {
                CXString cxStrName = clang_getCursorSpelling(cxCursor);
                String name = toString(cxStrName);
                CXString cxStrKind = clang_getCursorKindSpelling(cxKind);
                String kind = toString(cxStrKind);
                ss << "Kind : " << kind << ", Name : " << name;
            };
            
            std::stringstream ss;
            ss << std::endl << "----";
            formatCursorInfo(cxCursor, CXCursor_CXXBaseSpecifier, ss);
            ss << "----" << std::endl;

            // base class hierarchy
            int32_t tab = 2;
            formatTabs(1, ss);
            ss << "Base Class Info : ";
            CXType cxType = clang_getCursorType(cxCursor);
            cxAncestor = clang_getTypeDeclaration(cxType);
            cxAncestorKind = clang_getCursorKind(cxAncestor);
            while (!clang_equalCursors(cxAncestor, clang_getNullCursor()) && cxAncestorKind != CXCursor_TranslationUnit)
            {
                CXString cxAncestorName = clang_getCursorSpelling(cxAncestor);
                String ancestorName = toString(cxAncestorName);

                ss << std::endl;
                
                formatTabs(tab, ss);
                formatCursorInfo(cxAncestor, cxAncestorKind, ss);

                tab++;
                cxAncestor = clang_getCursorSemanticParent(cxAncestor);
                cxAncestorKind = clang_getCursorKind(cxAncestor);
            }

            // Derived Class Info
            cxAncestor = cxParent;
            tab = 2;
            ss << std::endl;
            formatTabs(1, ss);
            ss << "Derived Class Info : ";
            cxAncestorKind = clang_getCursorKind(cxParent);
            while (!clang_equalCursors(cxAncestor, clang_getNullCursor()) && cxAncestorKind != CXCursor_TranslationUnit)
            {
                CXString cxAncestorName = clang_getCursorSpelling(cxAncestor);
                CXString cxAncestorType = clang_getCursorKindSpelling(cxAncestorKind);

                ss << std::endl;
                
                formatTabs(tab, ss);
                formatCursorInfo(cxAncestor, cxAncestorKind, ss);
                
                tab++;
                cxAncestor = clang_getCursorSemanticParent(cxAncestor);
                cxAncestorKind = clang_getCursorKind(cxAncestor);
            }

            // Print information
            RP_LOG_INFO("%s", ss.str().c_str());
        } while (false);

        return ret;
    }
#endif
    
    //-------------------------------------------------------------------------

#if 1
    TResult ReflectionGenerator::processFunctionDeclaration(CXCursor cxCursor, CXCursor cxParent, bool isCXXMember, bool isConstructor, bool isDestructor)
    {
        TResult ret = T3D_OK;

        do
        {
            bool isProperty = false;
            bool rval;
            SpecifiersItr itrSpec;
            
            ASTFileInfo fileInfo;
            
            if (!isConstructor && !isDestructor)
            {
                // 非构造和析构函数，需要看是否打标签
                String path;
                uint32_t start, end, column, offset;
                CHECK_TAG_RET_SPEC(rval, itrSpec, cxCursor, functions, path, start, end, column, offset);
                if (!rval)
                {
                    // 没有打函数标签，那可能是属性，查是否打了属性标签
                    CHECK_TAG_RET_SPEC(rval, itrSpec, cxCursor, properties, path, start, end, column, offset);
                    if (!rval)
                    {
                        // 属性标签也没有，那没有反射
                        break;
                    }
                    else
                    {
                        // 属性函数
                        isProperty = true;
                    }
                }
                
                fileInfo.Path = path;
                fileInfo.StartLine = start;
                fileInfo.EndLine = end;
                
                if (cxParent.kind == CXCursor_ClassDecl
                    || cxParent.kind == CXCursor_ClassTemplate
                    || cxParent.kind == CXCursor_ClassTemplatePartialSpecialization)
                {
                    // 是类函数，还需要类有打反射标签
                    CHECK_TAG(rval, cxParent, classes, path, start, end, column, offset);
                    if (!rval)
                    {
                        break;
                    }
                }
                else if (cxParent.kind == CXCursor_StructDecl)
                {
                    // 结构体成员函数，需要结构体有打反射标签
                    CHECK_TAG(rval, cxParent, structs, path, start, end, column, offset);
                    if (!rval)
                    {
                        break;
                    }
                }
            }
            else
            {
                // 构造函数和析构函数不用打标签，只要类打了标签就可以反射
                String path;
                uint32_t start, end, column, offset;
                if (cxParent.kind == CXCursor_ClassDecl
                    || cxParent.kind == CXCursor_ClassTemplate
                    || cxParent.kind == CXCursor_ClassTemplatePartialSpecialization)
                {
                    // 类函数，需要类有打反射标签
                    CHECK_TAG_RET_SPEC(rval, itrSpec, cxParent, classes, path, start, end, column, offset);
                    if (!rval)
                    {
                        break;
                    }
                }
                else if (cxParent.kind == CXCursor_StructDecl)
                {
                    // 结构体函数，需要结构体有打反射标签
                    CHECK_TAG_RET_SPEC(rval, itrSpec, cxParent, structs, path, start, end, column, offset);
                    if (!rval)
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }

                fileInfo.Path = path;
                fileInfo.StartLine = start;
                fileInfo.EndLine = end;
            }
            
            // 创建函数重载结点，哪怕没有函数重载，函数的声明都在该结点上，ASTFunction 只是函数入口结点
            CXString cxStrUSR = clang_getCursorUSR(cxCursor);
            String USR;
            if (isProperty)
            {
                // 属性函数
                CXString cxName = clang_getCursorSpelling(cxCursor);
                USR = toString(cxName);
            }
            else
            {
                // 函数
                USR = toString(cxStrUSR);
            }
            
            ASTOverloadFunction *overload = nullptr;

            if (clang_CXXMethod_isStatic(cxCursor))
            {
                // 静态函数
                overload = new ASTStaticFunction(USR);
            }
            else if (isConstructor)
            {
                // 构造函数
                overload = new ASTConstructor(USR);
            }
            else if (isDestructor)
            {
                // 析构函数
                break;
                overload = new ASTDestructor(USR);
            }
            else if (isCXXMember)
            {
                // 成员函数
                overload = new ASTInstanceFunction(USR);
            }
            else
            {
                // 普通函数
                overload = new ASTOverloadFunction(USR);
            }

            overload->IsConst = (clang_CXXMethod_isConst(cxCursor) == 1);
            
            CXType cxType = clang_getCursorType(cxCursor);
            
            // 函数返回值
            CXType cxResultType = clang_getResultType(cxType);
            overload->RetType = toString(clang_getTypeSpelling(cxResultType));
            
            // 函数参数列表
            int32_t numArgs = clang_Cursor_getNumArguments(cxCursor);
            for (int32_t i = 0; i < numArgs; i++)
            {
                ASTFunctionParam param;
                // 参数名
                CXCursor cxArg = clang_Cursor_getArgument(cxCursor, i);
                param.Name = toString(clang_getCursorSpelling(cxArg));
                // 参数类型
                CXType cxArgType = clang_getArgType(cxType, i);
                param.Type = toString(clang_getTypeSpelling(cxArgType));
                overload->Params.push_back(param);
            }

            bool isGetter = false;
            String funcName;
            if (isProperty)
            {
                // 属性函数，用反射标签里面的名字
                bool found = false;
                const TList<Specifier> &specs = itrSpec->second;
                for (const auto &spec : specs)
                {
                    if (spec.name == kSpecName)
                    {
                        funcName = spec.value;
                        StringUtil::replaceAll(funcName, "\"", "");
                        found = true;
                    }
                    else if (spec.name == kSpecPropertyType)
                    {
                        if (spec.value == kSpecPropertyGetter)
                        {
                            isGetter = true;
                        }
                        else if (spec.value == kSpecPropertySetter)
                        {
                            isGetter = false;
                        }
                        else
                        {
                            found = false;
                            break;
                        }
                    }
                }

                if (!found)
                {
                    // 没有给名字，错误了
                    delete overload;
                    ret = T3D_ERR_RP_INVALID_SPECFIER;
                    RP_LOG_ERROR("Invalid specifier in TPROPERTY [%s:%u] !",
                        fileInfo.Path.c_str(), fileInfo.StartLine);
                    break;
                }
            }
            else
            {
                // 非属性函数，直接用函数名
                CXString cxName = clang_getCursorSpelling(cxCursor);
                funcName = toString(cxName);
            }
            
            // 获取父结点
            ASTNode *parent = getOrConstructParentNode(cxCursor);
            if (parent == nullptr)
            {
                
                ret = T3D_ERR_RP_AST_NO_PARENT;
                RP_LOG_ERROR("The parent is null [%s:%u] !", fileInfo.Path.c_str(), fileInfo.StartLine);
                break;
            }
            
            ASTNode *child = parent->getChild(funcName);
            if (child == nullptr)
            {
                // 没有函数结点，创建之
                ASTFunction *function = new ASTFunction(funcName);
                parent->addChild(funcName, function);
                parent = function;
                function->IsProperty = isProperty;
            }
            else
            {
                parent = child;   
            }

            ASTFunction *function = static_cast<ASTFunction *>(parent);
            if (function->IsProperty != isProperty)
            {
                // 函数类型不一致，存在同名函数
                delete overload;
                ret = T3D_ERR_RP_FUNCTION_NAME_CONFLICT;
                RP_LOG_ERROR("Function name (%s) conflict [%s:%u] !",
                    parent->getHierarchyName().c_str(), fileInfo.Path.c_str(), fileInfo.StartLine);
                break;
            }

            overload->FileInfo = std::move(fileInfo);
            overload->IsGetter = isGetter;
            
            if (!isConstructor && !isDestructor)
            {
                // 非构造函数、非析构函数才能有说明符
                overload->Specifiers = &itrSpec->second;
            }
            
            parent->addChild(USR, overload);

            if (cxParent.kind == CXCursor_Namespace || cxParent.kind == CXCursor_TranslationUnit)
            {
                // 非类和结构体函数
                insertSourceFiles(overload->FileInfo.Path, parent);       
            }
        } while (false);

        return ret;
    }
#else
    TResult ReflectionGenerator::processFunctionDeclaration(CXCursor cxCursor, CXCursor cxParent, bool isCXXMember, bool isConstructor, bool isDestructor)
    {
        TResult ret = T3D_OK;

        do
        {
            const std::function<void(int32_t,std::stringstream&)> formatTabs = [](int32_t tab, std::stringstream &ss)
            {
                for (int32_t i = 0; i < tab; i++)
                {
                    ss << "\t";
                }
            };

            const std::function<void(CXCursor, CXCursorKind, std::stringstream&)> formatCursorInfo = [this](CXCursor cxCursor, CXCursorKind cxKind, std::stringstream &ss)
            {
                CXString cxStrName = clang_getCursorUSR(cxCursor);//clang_getCursorSpelling(cxCursor);
                String name = toString(cxStrName);
                CXString cxStrKind = clang_getCursorKindSpelling(cxKind);
                String kind = toString(cxStrKind);
                ss << "Kind : " << kind << ", Name : " << name;
            };

            std::stringstream ss;
            int32_t tab = 0;

            CXCursor cxAncestor = cxCursor;//clang_getCursorSemanticParent(cxCursor);
            CXCursorKind cxAncestorKind = clang_getCursorKind(cxAncestor);
            while (!clang_equalCursors(cxAncestor, clang_getNullCursor()) && cxAncestorKind != CXCursor_TranslationUnit)
            {
                CXString cxAncestorName = clang_getCursorSpelling(cxAncestor);
                String ancestorName = toString(cxAncestorName);

                if (tab > 0)
                {
                    ss << std::endl;
                }

                formatTabs(tab, ss);
                formatCursorInfo(cxAncestor, cxAncestorKind, ss);
                tab++;
                
                cxAncestor = clang_getCursorSemanticParent(cxAncestor);
                cxAncestorKind = clang_getCursorKind(cxAncestor);
            }

            CXType cxType = clang_getCursorType(cxCursor);
            
            // Result Type
            CXType cxResultType = clang_getResultType(cxType);
            ss << std::endl;
            formatTabs(1, ss);
            ss << "Result Type : " << toString(clang_getTypeSpelling(cxResultType));
            
            // Arguments
            ss << std::endl;
            formatTabs(1, ss);
            tab = 2;
            ss << "Arguments : " << std::endl;
            int32_t numArgs = clang_Cursor_getNumArguments(cxCursor);
            for (int32_t i = 0; i < numArgs; i++)
            {
                CXCursor cxArg = clang_Cursor_getArgument(cxCursor, i);
                CXCursorKind cxArgKind = clang_getCursorKind(cxArg);
                formatTabs(tab, ss);
                formatCursorInfo(cxArg, cxArgKind, ss);
                
                CXType cxArgType = clang_getArgType(cxType, i);
                ss << ", Type : " << toString(clang_getTypeSpelling(cxArgType)); 
                
                if (i != numArgs - 1)
                {
                    ss << std::endl;
                }
            }
            
            RP_LOG_INFO("\n---- Declaration of Function ---- \n This function -> %s", ss.str().c_str());
        } while (false);

        return ret;
    }
#endif

    //-------------------------------------------------------------------------

    TResult ReflectionGenerator::processEnumDeclaration(CXCursor cxCursor, CXCursor cxParent)
    {
        TResult ret = T3D_OK;

        do
        {
            bool rval;
            String path;
            uint32_t start, end, col, offset;
            SpecifiersItr itrSpec;
            
            // 枚举是否打了反射标签
            CHECK_TAG_RET_SPEC(rval, itrSpec, cxCursor, enumerations, path, start, end, col, offset);
            if (!rval)
            {
                break;
            }

            ASTFileInfo fileInfo;
            fileInfo.Path = path;
            fileInfo.StartLine = start;
            fileInfo.EndLine = end;
            
            // 枚举属于 class 或 struct，看 class 或 struct 是否有打反射标签
            if (cxParent.kind == CXCursor_ClassDecl
                || cxParent.kind == CXCursor_ClassTemplate
                || cxParent.kind == CXCursor_ClassTemplatePartialSpecialization)
            {
                CHECK_TAG(rval, cxParent, classes, path, start, end, col, offset);
                if (!rval)
                {
                    break;
                }
            }
            else if (cxParent.kind == CXCursor_StructDecl)
            {
                CHECK_TAG(rval, cxParent, structs, path, start, end, col, offset);
                if (!rval)
                {
                    break;
                }
            }

            // 获取父结点
            ASTNode *parent = getOrConstructParentNode(cxCursor);
            if (parent == nullptr)
            {
                ret = T3D_ERR_RP_AST_NO_PARENT;
                RP_LOG_ERROR("The parent is null [%s:%u] !", fileInfo.Path.c_str(), fileInfo.StartLine);
                break;
            }

            // 构造枚举结点并加到父结点上
            CXString cxName = clang_getCursorSpelling(cxCursor);
            String name = toString(cxName);
            ASTEnum *enumeration = new ASTEnum(name);
            parent->addChild(name, enumeration);

            enumeration->FileInfo = std::move(fileInfo);
            enumeration->Specifiers = &itrSpec->second;
            
            if (cxParent.kind == CXCursor_Namespace || cxParent.kind == CXCursor_TranslationUnit)
            {
                // 非类和结构体函数
                insertSourceFiles(path, enumeration);       
            }
        } while (false);

        return ret;
    }

    //-------------------------------------------------------------------------

    TResult ReflectionGenerator::processEnumConstDeclaration(CXCursor cxCursor, CXCursor cxParent)
    {
        TResult ret = T3D_OK;

        do
        {
            bool rval;
            String path;
            uint32_t start, end, col, offset;
            
            // 枚举是否打了反射标签
            CHECK_TAG(rval, cxParent, enumerations, path, start, end, col, offset);
            if (!rval)
            {
                break;
            }

            // 获取父结点
            ASTNode *parent = getOrConstructParentNode(cxCursor);
            if (parent == nullptr)
            {
                ret = T3D_ERR_RP_AST_NO_PARENT;
                RP_LOG_ERROR("The parent is null [%s:%u] !", path.c_str(), start);
                break;
            }

            // 构造枚举常量结点并加到父结点上            
            CXString cxName = clang_getCursorSpelling(cxCursor);
            String name = toString(cxName);
            ASTEnumConstant *constant = new ASTEnumConstant(name);
            constant->Value = clang_getEnumConstantDeclUnsignedValue(cxCursor);
            parent->addChild(name, constant);
        } while (false);
        
        return ret;
    }

    //-------------------------------------------------------------------------

    TResult ReflectionGenerator::processVariableDeclaration(CXCursor cxCursor, CXCursor cxParent, bool isCXXMember)
    {
        TResult ret = T3D_OK;

        do
        {
            bool rval;
            String path;
            uint32_t start, end, col, offset;
            SpecifiersItr itrSpec;
            
            // 变量是否打了反射标签
            CHECK_TAG_RET_SPEC(rval, itrSpec, cxCursor, properties, path, start, end, col, offset);
            if (!rval)
            {
                break;
            }

            ASTFileInfo fileInfo;
            fileInfo.Path = path;
            fileInfo.StartLine = start;
            fileInfo.EndLine = end;
            
            // class 或 struct 的成员变量，看 class 或 struct 是否有打反射标签
            if (cxParent.kind == CXCursor_ClassDecl
                || cxParent.kind == CXCursor_ClassTemplate
                || cxParent.kind == CXCursor_ClassTemplatePartialSpecialization)
            {
                CHECK_TAG(rval, cxParent, classes, path, start, end, col, offset);
                if (!rval)
                {
                    break;
                }
            }
            else if (cxParent.kind == CXCursor_StructDecl)
            {
                CHECK_TAG(rval, cxParent, structs, path, start, end, col, offset);
                if (!rval)
                {
                    break;
                }
            }

            // 获取父结点
            ASTNode *parent = getOrConstructParentNode(cxCursor);
            if (parent == nullptr)
            {
                ret = T3D_ERR_RP_AST_NO_PARENT;
                RP_LOG_ERROR("The parent is null [%s:%u] !", fileInfo.Path.c_str(), fileInfo.StartLine);
                break;
            }

            // 构造属性结点并加到父结点上
            CXString cxName = clang_getCursorSpelling(cxCursor);
            String name = toString(cxName);
            ASTProperty *property = new ASTProperty(name);
            property->DataType = toString(clang_getTypeSpelling(clang_getCursorType(cxCursor)));
            property->Specifiers = &itrSpec->second;
            parent->addChild(name, property);

            if (cxParent.kind == CXCursor_Namespace || cxParent.kind == CXCursor_TranslationUnit)
            {
                // 非类和结构体函数
                insertSourceFiles(path, property);       
            }
            // RP_LOG_INFO("Variable -> Name : %s, Type : %s, CXXMember : %d (Parent Name : %s, Type : %s)",
            //     toString(clang_getCursorSpelling(cxCursor)).c_str(),
            //     toString(clang_getTypeSpelling(clang_getCursorType(cxCursor))).c_str(),//toString(clang_getCursorKindSpelling(cxCursor.kind)).c_str(),
            //     isCXXMember,
            //     toString(clang_getCursorSpelling(cxParent)).c_str(),
            //     toString(clang_getCursorKindSpelling(cxParent.kind)).c_str());
        } while (false);

        return ret;
    }

    //-------------------------------------------------------------------------

    // TResult ReflectionGenerator::processOverloadDeclaration(CXCursor cxCursor, CXCursor cxParent)
    // {
    //     TResult ret = T3D_OK;
    //
    //     do
    //     {
    //         const std::function<void(int32_t,std::stringstream&)> formatTabs = [](int32_t tab, std::stringstream &ss)
    //         {
    //             for (int32_t i = 0; i < tab; i++)
    //             {
    //                 ss << "\t";
    //             }
    //         };
    //
    //         const std::function<void(CXCursor, CXCursorKind, std::stringstream&)> formatCursorInfo = [this](CXCursor cxCursor, CXCursorKind cxKind, std::stringstream &ss)
    //         {
    //             CXString cxStrName = clang_getCursorSpelling(cxCursor);
    //             String name = toString(cxStrName);
    //             CXString cxStrKind = clang_getCursorKindSpelling(cxKind);
    //             String kind = toString(cxStrKind);
    //             ss << "Kind : " << kind << ", Name : " << name;
    //         };
    //
    //         std::stringstream ss;
    //         int32_t tab = 0;
    //
    //         CXCursor cxAncestor = cxCursor;//clang_getCursorSemanticParent(cxCursor);
    //         CXCursorKind cxAncestorKind = clang_getCursorKind(cxAncestor);
    //         while (!clang_equalCursors(cxAncestor, clang_getNullCursor()) && cxAncestorKind != CXCursor_TranslationUnit)
    //         {
    //             CXString cxAncestorName = clang_getCursorSpelling(cxAncestor);
    //             String ancestorName = toString(cxAncestorName);
    //
    //             if (tab > 0)
    //             {
    //                 ss << std::endl;
    //             }
    //
    //             formatTabs(tab, ss);
    //             formatCursorInfo(cxAncestor, cxAncestorKind, ss);
    //             tab++;
    //             
    //             cxAncestor = clang_getCursorSemanticParent(cxAncestor);
    //             cxAncestorKind = clang_getCursorKind(cxAncestor);
    //         }
    //
    //         tab = 1;
    //         ss << std::endl;
    //         formatTabs(tab, ss);
    //         tab = 2;
    //         uint32_t numOverloadDecls = clang_getNumOverloadedDecls(cxCursor);
    //         ss << "Overload [" << numOverloadDecls << "]";
    //         for (uint32_t i = 0; i < numOverloadDecls; i++)
    //         {
    //             CXCursor cxOverloadCursor = clang_getOverloadedDecl(cxCursor, i);
    //             ss << std::endl;
    //             formatTabs(tab, ss);
    //             formatCursorInfo(cxOverloadCursor, cxOverloadCursor.kind, ss);
    //         }
    //
    //         RP_LOG_INFO("\n---- Declaration of Overload ---- \n This function -> \n%s", ss.str().c_str());
    //     } while (false);
    //
    //     return ret;
    // }

    //-------------------------------------------------------------------------

    TResult ReflectionGenerator::processTemplateParameter(CXCursor cxCursor, CXCursor cxParent)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //-------------------------------------------------------------------------

    void ReflectionGenerator::dumpReflectionInfo(const String &path) const
    {
        FileDataStream stream;
        
        if (stream.open(path.c_str(), FileDataStream::EOpenMode::E_MODE_READ_WRITE|FileDataStream::EOpenMode::E_MODE_TEXT|FileDataStream::EOpenMode::E_MODE_TRUNCATE))
        {
            JsonStream os(stream);
            rapidjson::PrettyWriter<JsonStream> writer(os);
            writer.StartObject();
            mRoot->dump(writer);
            writer.EndObject();

            stream.close();
        }
    }

    //-------------------------------------------------------------------------

    void ReflectionGenerator::getASTNodeInfo(CXCursor cxCursor, String &filePath, uint32_t &start, uint32_t &end, uint32_t &column, uint32_t &offset) const
    {
        CXSourceRange cxRange = clang_getCursorExtent(cxCursor);
            
        CXFile cxFile;

        // 文件路径、起始行号，列号，文件中偏移
        CXSourceLocation cxStart = clang_getRangeStart(cxRange);
        clang_getFileLocation(cxStart, &cxFile, &start, &column, &offset);
        CXString cxPath = clang_File_tryGetRealPathName(cxFile);
        filePath = toString(cxPath);
        
        // 结束行号
        CXSourceLocation cxEnd = clang_getRangeEnd(cxRange);
        clang_getFileLocation(cxEnd, &cxFile, &end, &column, &offset);
    }

    //-------------------------------------------------------------------------

    ASTNode *ReflectionGenerator::createNode(const ASTNodeInfo& info) const
    {
        ASTNode *node = nullptr;
        switch (info.cxKind)
        {
        case CXCursor_Namespace:
            {
                node = new ASTNamespace(info.name);
            }
            break;
        case CXCursor_ClassDecl:
            {
                // node = new ASTClass(info.name);
                // node->HaveRTTI = true;
            }
            break;
        case CXCursor_StructDecl:
            {
                // node = new ASTStruct(info.name);
                // node->HaveRTTI = true;
            }
            break;
        case CXCursor_ClassTemplate:
            {
                
            }
            break;
        case CXCursor_ClassTemplatePartialSpecialization:
            {
                
            }
            break;
        default:
            {
                T3D_ASSERT(0);
            }
            break;
        }
        return node;
    }
    
    
    //-------------------------------------------------------------------------

    ASTNode *ReflectionGenerator::getOrConstructParentNode(CXCursor cxCursor)
    {
        TList<ASTNodeInfo> nodes;
        CXCursor cxAncestor = cxCursor;
        CXCursorKind cxAncestorKind = CXCursor_ClassDecl;
        while (!clang_equalCursors(cxAncestor, clang_getNullCursor()) && cxAncestorKind != CXCursor_TranslationUnit)
        {
            CXString cxAncestorName = clang_getCursorSpelling(cxAncestor);
            String ancestorName = toString(cxAncestorName);
            nodes.push_front({ancestorName, cxAncestorKind});

            T3D_ASSERT(cxAncestorKind == CXCursor_Namespace
                || cxAncestorKind == CXCursor_ClassTemplate
                || cxAncestorKind == CXCursor_ClassTemplatePartialSpecialization
                || cxAncestorKind == CXCursor_ClassDecl
                || cxAncestorKind == CXCursor_StructDecl
                || cxAncestorKind == CXCursor_EnumDecl);
                
            cxAncestor = clang_getCursorSemanticParent(cxAncestor);
            cxAncestorKind = clang_getCursorKind(cxAncestor);
        }
        
        // 逐级查找和创建对应的结点，直到新结点的父结点为止
        nodes.pop_back(); // 自己也在里面先把自己弹出来
        ASTNode *parent = mRoot;
        for (const auto &node : nodes)
        {
            ASTNode *child = parent->getChild(node.name);
            if (child == nullptr)
            {
                // 没有对应的结点，创建一个
                child = createNode(node);
            }
            if (child == nullptr)
            {
                parent = child;
                break;
            }
            parent->addChild(node.name, child);
            parent = child;
            T3D_ASSERT(parent->getType() == ASTNode::Type::kNamespace
                || parent->getType() == ASTNode::Type::kClassTemplate
                || parent->getType() == ASTNode::Type::kStruct
                || parent->getType() == ASTNode::Type::kClass
                || parent->getType() == ASTNode::Type::kEnum);
        }

        return parent;
    }

    //-------------------------------------------------------------------------

    void ReflectionGenerator::insertSourceFiles(const String &path, ASTNode *node)
    {
        // 查找该类是否当前工程的，如果是，放到里面
        auto itr = mSourceFiles.find(path);
        if (itr == mSourceFiles.end())
        {
            return;
        }

        String hierarchyName = node->getHierarchyName();
        auto it = itr->second.find(hierarchyName);
        if (it != itr->second.end())
        {
            // 该类已经在里面了，忽略之
            return;
        }

        itr->second.insert(ASTNodeMapValue(hierarchyName, node));
    }

    //-------------------------------------------------------------------------

    TResult ReflectionGenerator::generateSource(const String &generatedPath)
    {
        FileDataStream fs;
        
        for (const auto &val : mSourceFiles)
        {
            if (val.second.empty())
            {
                continue;
            }
            
            String dir, title, ext;
            Dir::parsePath(val.first, dir, title, ext);
            String path = generatedPath + Dir::getNativeSeparator() + title + ".generated.cpp";
                
            if (!fs.open(path.c_str(), FileDataStream::E_MODE_TEXT | FileDataStream::E_MODE_APPEND | FileDataStream::E_MODE_WRITE_ONLY))
            {
                // 文件打开失败
                break;
            }

            // 文件头注释
            fs << "// Copyright (C) 2015-2020  Answer Wong" << std::endl;
            fs << "// Generated code exported from ReflectionPreprocessor." << std::endl;
            fs << "// DO NOT modify this manually! Edit the corresponding .h files instead!" << std::endl;
                
            // 需要包含的头文件
            fs << std::endl;
            fs << "#include <rttr/registration>" << std::endl;
            fs << "#include \"" << title << ".h\"" << std::endl;

            // 开始注册类信息
            fs << std::endl << "RTTR_REGISTRATION" << std::endl;
            fs << "{" << std::endl;
            
            {
                fs << "\tusing namespace rttr;" << std::endl;
            
                for (const auto &value : val.second)
                {
                    value.second->generateSourceFile(fs);
                }
            }
            
            // 结束注册类信息
            fs << "}" << std::endl;
                
            // 是在这里打开的文件，就在这里关闭
            fs.close();
        }
        
        return T3D_OK;
    }

    //-------------------------------------------------------------------------

    void ReflectionGenerator::insertClassTemplate(const String &name, ASTClassTemplate *klass)
    {
        // 查找该类是否 cache 了，有则忽略
        auto itr = mClassTemplates.find(name);
        if (itr != mClassTemplates.end())
        {
            // 该类已经在里面了，忽略之
            return;
        }

       mClassTemplates.insert(ASTClassTemplateMapValue(name, klass));
    }
    
    //-------------------------------------------------------------------------
}
