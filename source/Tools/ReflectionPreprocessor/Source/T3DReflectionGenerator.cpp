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

#include <SDL_syswm.h>

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

    void ReflectionGenerator::setBuiltinClass(const StringList &whitelist)
    {
        for (const auto &str : whitelist)
        {
            StringArray names = StringUtil::split(str, "::");
            const String &name = names.back();
            mClassWhiteList.insert(ASTWhiteListValue(name, str));
        }
    }

    //-------------------------------------------------------------------------

    void ReflectionGenerator::parseProjectHeaderPath(const ClangArgs &args)
    {
        for (const auto &arg : args)
        {
            String str = arg;
            if (StringUtil::match(str, "-I*", true))
            {
                StringUtil::replaceAll(str, "-I", "");
                mIncludePathes.push_back(str);
            }
        }
    }

    //-------------------------------------------------------------------------

    void ReflectionGenerator::collectProjectHeaders(const String &path)
    {
        mSourceFiles.insert(SourceFilesMapValue(path, ASTNodeMap()));

        // String dir, title, ext;
        // Dir::parsePath(path, dir, title, ext);
        // mHeaderFiles.insert(HeaderFilesMapValue(title, dir));
    }

    //-------------------------------------------------------------------------

    TResult ReflectionGenerator::generateAST(const String &srcPath, const ClangArgs &args)
    {
        TResult ret = T3D_OK;

        do
        {
            RP_LOG_INFO("[%s] Generating reflection source file ...", srcPath.c_str());

            CXIndex cxIndex = clang_createIndex(0, 0);

            CXTranslationUnit cxUnit = clang_parseTranslationUnit(cxIndex, srcPath.c_str(),
                args.data(), args.size(), nullptr, 0,
                CXTranslationUnit_DetailedPreprocessingRecord);
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
                RP_LOG_INFO("> Diagnostics:");
                for (uint32_t i = 0; i < cxNumDiag; ++i)
                {
                    const auto cxDiag = clang_getDiagnostic(cxUnit, i);
                    CXDiagnosticSeverity cxSeverity = clang_getDiagnosticSeverity(cxDiag);
                    if (cxSeverity == CXDiagnostic_Error || cxSeverity == CXDiagnostic_Fatal)
                    {
                        hasErrors = true;
                        RP_LOG_ERROR(">>> %s", toString(clang_formatDiagnostic(cxDiag, clang_defaultDiagnosticDisplayOptions())).c_str());
                    }
                    else
                    {
                        RP_LOG_WARNING(">>> %s", toString(clang_formatDiagnostic(cxDiag, clang_defaultDiagnosticDisplayOptions())).c_str());
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

            ClientData data = {mRoot, this};
            
            clang_visitChildren(
                cxCursor,
                [](CXCursor cxCursor, CXCursor cxParent, CXClientData cxData)
                {
                    ClientData *data = static_cast<ClientData*>(cxData);
                    return data->generator->visitRootChildren(cxCursor, cxParent, data->parent);
                },
                &data);

            clang_disposeTranslationUnit(cxUnit);
            
            clang_disposeIndex(cxIndex);

            RP_LOG_INFO(">---- [%s] Completed ! ----", srcPath.c_str());
        } while (false);

        return ret;
    }

    //-------------------------------------------------------------------------

    CXChildVisitResult ReflectionGenerator::visitRootChildren(CXCursor cxCursor, CXCursor cxParent, ASTNode *parent)
    {
        CXChildVisitResult cxResult = CXChildVisit_Continue;

        TResult ret = T3D_OK;
        
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
                ret = processClassDeclaration(cxCursor, cxParent, false, false);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_ClassDecl:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                ret = processClassDeclaration(cxCursor, cxParent, true, false);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_EnumDecl:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                ret = processEnumDeclaration(cxCursor, cxParent);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_FunctionDecl:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                ret = processFunctionDeclaration(cxCursor, cxParent, false, false, false, false);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_VarDecl:
            {
                ret = processVariableDeclaration(cxCursor, cxParent, false);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_Namespace:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                // processNamespace(cxCursor, cxParent);
                cxResult = CXChildVisit_Recurse;
            }
            break;
        case CXCursor_FunctionTemplate:
            {
                // RP_LOG_INFO("CXCursor_FunctionTemplate %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
                ret = processFunctionDeclaration(cxCursor, cxParent, false, false, false, true);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_ClassTemplate:
            {
                // RP_LOG_INFO("CXCursor_ClassTemplate %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
                ret = processClassDeclaration(cxCursor, cxParent, true, true);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_ClassTemplatePartialSpecialization:
            {
                // RP_LOG_INFO("CXCursor_ClassTemplatePartialSpecialization %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
            }
            break;
        case CXCursor_CXXBaseSpecifier:
            {
                RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                //ret = processClassBaseSpecifier(cxCursor, cxParent, parent);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_TemplateRef:
            {
                RP_LOG_INFO("CXCursor_TemplateRef %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
            }
            break;
        case CXCursor_MacroExpansion:
            {
                ret = processMacroExpansion(cxCursor, cxParent);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_InclusionDirective:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                ret = processInclusionDirective(name, cxCursor, cxParent);
            }
            break;
        default:
            {
            }
            break;
        }

        if (T3D_RP_FATAL(ret))
        {
            cxResult = CXChildVisit_Break;
        }
        
        return cxResult;
    }

    //-------------------------------------------------------------------------

    CXChildVisitResult ReflectionGenerator::visitClassChildren(CXCursor cxCursor, CXCursor cxParent, ASTStruct *parent)
    {
        CXChildVisitResult cxResult = CXChildVisit_Continue;

        TResult ret = T3D_OK;
        
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
                ret = processClassDeclaration(cxCursor, cxParent, false, false);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_ClassDecl:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                ret = processClassDeclaration(cxCursor, cxParent, true, false);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_EnumDecl:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                ret = processEnumDeclaration(cxCursor, cxParent);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_FieldDecl:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                ret = processVariableDeclaration(cxCursor, cxParent, true);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_CXXMethod:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                ret = processFunctionDeclaration(cxCursor, cxParent, true, false, false, false);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_Constructor:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                ret = processFunctionDeclaration(cxCursor, cxParent, true, true, false, false);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_Destructor:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                ret = processFunctionDeclaration(cxCursor, cxParent, true, false, true, false);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_TemplateTypeParameter:
            {
                // RP_LOG_INFO("CXCursor_TemplateTypeParameter %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
                ret = processTemplateParameter(cxCursor, cxParent, parent);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_NonTypeTemplateParameter:
            {
                // RP_LOG_INFO("CXCursor_NonTypeTemplateParameter %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
                ret = processTemplateParameter(cxCursor, cxParent, parent);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_TemplateTemplateParameter:
            {
                // RP_LOG_INFO("CXCursor_TemplateTemplateParameter %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
                ret = processTemplateParameter(cxCursor, cxParent, parent);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_FunctionTemplate:
            {
                // RP_LOG_INFO("CXCursor_FunctionTemplate %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
                CXCursorKind cxTemplateKind = clang_getTemplateCursorKind(cxCursor);
                switch (cxTemplateKind)
                {
                case CXCursor_CXXMethod:
                    ret = processFunctionDeclaration(cxCursor, cxParent, true, false, false, true);
                    break;
                case CXCursor_Constructor:
                    ret = processFunctionDeclaration(cxCursor, cxParent, true, true, false, true);
                    break;
                case CXCursor_Destructor:
                    ret = processFunctionDeclaration(cxCursor, cxParent, true, false, true, true);
                    break;
                default:
                    ret = processFunctionDeclaration(cxCursor, cxParent, false, false, false, true);
                    break;
                }
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_ClassTemplate:
            {
                // RP_LOG_INFO("CXCursor_ClassTemplate %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
                ret = processClassDeclaration(cxCursor, cxParent, true, true);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_ClassTemplatePartialSpecialization:
            {
                // RP_LOG_INFO("CXCursor_ClassTemplatePartialSpecialization %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
            }
            break;
        case CXCursor_CXXBaseSpecifier:
            {
                // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
                ret = processClassBaseSpecifier(cxCursor, cxParent, parent);
                cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_TemplateRef:
            {
                // RP_LOG_INFO("CXCursor_TemplateRef %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
            }
            break;
        // case CXCursor_MacroExpansion:
        //     {
        //         ret = processMacroExpansion(cxCursor, cxParent);
        //         cxResult = CXChildVisit_Continue;
        //     }
        //     break;
        default:
            {
            }
            break;
        }

        if (T3D_RP_FATAL(ret))
        {
            cxResult = CXChildVisit_Break;
        }
        
        return cxResult;
    }

    //-------------------------------------------------------------------------

    CXChildVisitResult ReflectionGenerator::visitEnumChildren(CXCursor cxCursor, CXCursor cxParent, ASTEnum *parent)
    {
        CXChildVisitResult cxResult = CXChildVisit_Continue;

        TResult ret = T3D_OK;
        
        CXCursorKind cxKind = clang_getCursorKind(cxCursor);

        CXString cxName = clang_getCursorSpelling(cxCursor);
        String name = toString(cxName);
        CXString cxType = clang_getCursorKindSpelling(cxKind);
        String type = toString(cxType);

        // RP_LOG_INFO("%s : %s", type.c_str(), name.c_str());
        
        switch (cxKind)
        {
        case CXCursor_EnumConstantDecl:
            {
                processEnumConstDeclaration(cxCursor, cxParent, parent);
                cxResult = CXChildVisit_Continue;
            }
            break;
        // case CXCursor_MacroExpansion:
        //     {
        //         ret = processMacroExpansion(cxCursor, cxParent);
        //         cxResult = CXChildVisit_Continue;
        //     }
        //     break;
        default:
            {
            }
            break;
        }

        if (T3D_RP_FATAL(ret))
        {
            cxResult = CXChildVisit_Break;
        }
        
        return cxResult;
    }

    //-------------------------------------------------------------------------

    CXChildVisitResult ReflectionGenerator::visitFunctionChildren(CXCursor cxCursor, CXCursor cxParent, ASTFunction *parent)
    {
        CXChildVisitResult cxResult = CXChildVisit_Recurse;

        TResult ret = T3D_OK;
        
        CXCursorKind cxKind = clang_getCursorKind(cxCursor);

        String name = toString(clang_getCursorSpelling(cxCursor));
        String type = toString(clang_getCursorKindSpelling(cxKind));

        String parentName = toString(clang_getCursorSpelling(cxParent));
        String parentType = toString(clang_getCursorKindSpelling(cxParent.kind));
        // RP_LOG_INFO("FunctionChildren -- %s : %s (parent %s : %s)", type.c_str(), name.c_str(), parentType.c_str(), parentName.c_str());

        // if (parent != nullptr && cxParent.kind == CXCursor_ParmDecl && clang_isExpression(cxCursor.kind))
        // {
        //     // 处理函数默认参数
        //     ret = processDefaultArguments(cxCursor, cxParent, parent, parentName);
        // }
        
        switch (cxKind)
        {
        case CXCursor_TemplateTypeParameter:
            {
                if (parent != nullptr)
                {
                    // RP_LOG_INFO("CXCursor_TemplateTypeParameter %s : %s (parent %s : %s)", type.c_str(), name.c_str(), toString(clang_getCursorKindSpelling(cxParent.kind)).c_str(), toString(clang_getCursorSpelling(cxParent)).c_str());
                    ret = processTemplateParameter(cxCursor, cxParent, parent);
                    cxResult = CXChildVisit_Continue;
                }
            }
            break;
        case CXCursor_NonTypeTemplateParameter:
            {
                if (parent != nullptr)
                {
                    ret = processTemplateParameter(cxCursor, cxParent, parent);
                    cxResult = CXChildVisit_Continue;
                }
            }
            break;
        case CXCursor_TemplateTemplateParameter:
            {
                if (parent != nullptr)
                {
                    ret = processTemplateParameter(cxCursor, cxParent, parent);
                    cxResult = CXChildVisit_Continue;
                }
            }
            break;
        case CXCursor_TemplateRef:
            {
                // if (name == "XXfunction")
                // {
                //     ret = T3D_OK;
                // }
                // String parentName = toString(clang_getCursorSpelling(cxParent));
                // String parentType = toString(clang_getCursorKindSpelling(cxParent.kind));
                // // ret = instantiateTemplate(cxCursor);
                // ret = T3D_OK;
                // cxResult = CXChildVisit_Continue;
                // RP_LOG_INFO("%s : %s (parent %s : %s)", type.c_str(), name.c_str(), parentType.c_str(), parentName.c_str());
            }
            break;
        case CXCursor_VarDecl:
            {
                ret = instantiateClassTemplate(cxCursor, parent->FileInfo.Path);
            }
            break;
        case CXCursor_DeclRefExpr:
            {
                ret = instantiateFunctionTemplate(cxCursor);
                // cxResult = CXChildVisit_Continue;
            }
            break;
        case CXCursor_TypeRef:
            {
                if (parent != nullptr && (cxParent.kind == CXCursor_ParmDecl
                    || cxParent.kind == CXCursor_CXXMethod || cxParent.kind == CXCursor_FunctionDecl))
                {
                    // RP_LOG_INFO("FunctionChildren -- %s : %s (parent %s : %s)", type.c_str(), name.c_str(), parentType.c_str(), parentName.c_str());
                    CXType cxType = clang_getCursorType(cxCursor);
                    cxType = clang_getCanonicalType(cxType);
                    CXCursor cxParam = clang_getTypeDeclaration(cxType);
                    instantiateClassTemplate(cxParam, parent->FileInfo.Path);
                    // RP_LOG_INFO("FunctionChildren -- %s(%s) : %s (parent %s : %s)", type.c_str(), toString(clang_getCursorSpelling(cxParam)).c_str(), name.c_str(), parentType.c_str(), parentName.c_str());
                }
            }
            break;
        default:
            {
                
            }
            break;
        }
        
        if (T3D_RP_FATAL(ret))
        {
            cxResult = CXChildVisit_Break;
        }

        return cxResult;
    }

    //-------------------------------------------------------------------------

    CXChildVisitResult ReflectionGenerator::visitVariableChildren(CXCursor cxCursor, CXCursor cxParent, ASTProperty *parent)
    {
        CXChildVisitResult cxResult = CXChildVisit_Recurse;

        TResult ret = T3D_OK;
        
        CXCursorKind cxKind = clang_getCursorKind(cxCursor);

        CXString cxName = clang_getCursorSpelling(cxCursor);
        String name = toString(cxName);
        CXString cxType = clang_getCursorKindSpelling(cxKind);
        String type = toString(cxType);

        switch (cxKind)
        {
        case CXCursor_TemplateRef:
            {
                String parentName = toString(clang_getCursorSpelling(cxParent));
                String parentType = toString(clang_getCursorKindSpelling(cxParent.kind));
                // ret = instantiateClassTemplate(cxCursor);
                ret = T3D_OK;
            }
            break;
        default:
            {
            }
            break;
        }
        
        if (T3D_RP_FATAL(ret))
        {
            cxResult = CXChildVisit_Break;
        }

        return cxResult;
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
            else if (name == kRTTIFriend)
            {
                // 反射友元
                ret = processMacroFriend(name, cxCursor, cxParent);
            }
        } while (false);
        
        return ret;
    }

    //-------------------------------------------------------------------------

    TResult ReflectionGenerator::processMacroTags(const String &name, CXCursor cxCursor, CXCursor cxParent)
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

    TResult ReflectionGenerator::processMacroFriend(const String &name, CXCursor cxCursor, CXCursor cxParent)
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

            info->friends.insert(RTTIFriendsValue(start, true));
        } while (false);

        return ret;
    }

    //-------------------------------------------------------------------------

    TResult ReflectionGenerator::processInclusionDirective(const String &name, CXCursor cxCursor, CXCursor cxParent)
    {
        TResult ret = T3D_OK;

        do
        {
            CXTranslationUnit cxTU = clang_Cursor_getTranslationUnit(cxCursor);
            String unit = toString(clang_getTranslationUnitSpelling(cxTU));
            
            String path;
            uint32_t start, end, col, offset;
            getASTNodeInfo(cxCursor, path, start, end, col, offset);

            if (unit != path)
            {
                break;
            }

            String dir, title, ext;
            Dir::parsePath(path, dir, title, ext);
            if (ext != "cpp" && ext != "cxx" && ext != "c")
            {
                // 只有源码文件的头文件才有效
                break;
            }

            // 对应的头文件集合
            auto itrFile = mHeaderFiles.find(title);
            if (itrFile == mHeaderFiles.end())
            {
                auto rval = mHeaderFiles.insert(HeaderFilesMapValue(title, StringList()));
                itrFile = rval.first;
            }

            if (itrFile->first == "map" || itrFile->first == "T3DShader")
            {
                int a = 0;
            }
            itrFile->second.push_back(name);
        } while (false);
        
        return ret;
    }

    //-------------------------------------------------------------------------

    bool ReflectionGenerator::isRTTIFriend(FileReflectionInfoPtr info, uint32_t start, uint32_t end) const
    {
        bool isFriend = false;
        for (const auto &val : info->friends)
        {
            if (val.first >= start && val.first <= end)
            {
                isFriend = true;
                break;
            }
        }
        return isFriend;
    }

    //-------------------------------------------------------------------------

    TResult ReflectionGenerator::processClassDeclaration(CXCursor cxCursor, CXCursor cxParent, bool isClass, bool isTemplate)
    {
        TResult ret = T3D_OK;

        do
        {
            // 检测是否有打反射标签
            bool rval;
            FilesItr itrFile;
            SpecifiersItr itrSpec;
            String path;
            uint32_t start, end, column, offset;
            if (isClass)
            {
                CHECK_TAG_RET_FILE_SPEC(rval, itrFile, itrSpec, cxCursor, classes, path, start, end, column, offset);
            }
            else
            {
                CHECK_TAG_RET_FILE_SPEC(rval, itrFile, itrSpec, cxCursor, structs, path, start, end, column, offset);
            }

            // 获取类名
            //CXString cxName = clang_getCursorSpelling(clang_getCanonicalCursor(cxCursor));
            CXString cxName = clang_getCursorSpelling(cxCursor);
            String name = toString(cxName);
            
            // 获取父结点
            ASTNode *parent = getOrConstructParentNode(cxCursor);
            if (parent == nullptr)
            {
                // 父结点不存在
                ret = T3D_ERR_RP_AST_NO_PARENT;
                RP_LOG_WARNING("The parent of class %s is null [%s:%u] !",
                    name.c_str(), path.c_str(), start);
                break;
            }
            
            bool inWhitelist = false;
            auto itrWhitelist = mClassWhiteList.find(name);
            if (itrWhitelist != mClassWhiteList.end())
            {
                // 在白名单里，再看看命名空间是否对得上
                String hierarchyName = parent->getHierarchyName();
                hierarchyName = hierarchyName + "::" + name;
                inWhitelist = (hierarchyName == itrWhitelist->second);
            }

            bool RTTIEnabled = false;
            TList<String> *baseClasses = nullptr;
            
            if (!inWhitelist)
            {
                // class 没在自动反射白名单里
                if (!rval)
                {
                    break;
                }

                if (isClass)
                {
                    // 检测反射开关是否打开
                    FileReflectionInfoPtr info = itrFile->second;
                
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
                }
                else
                {
                    RTTIEnabled = true;
                }
            }
            else
            {
                // 结构体自动打开
                RTTIEnabled = true;
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
                    insertSourceFiles(path, node, false);
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
            klass->Specifiers = (!inWhitelist ? &itrSpec->second : nullptr);

            // 把自己加到父结点上
            parent->addChild(name, klass);

            // 放到源码文件缓存中
            if (isTemplate)
            {
                String USR = toString(clang_getCursorUSR(cxCursor));
                insertClassTemplate(USR, static_cast<ASTClassTemplate*>(klass));
            }
            else
            {
                insertSourceFiles(path, klass, false);
            }

            ClientData data = {klass, this};
            clang_visitChildren(cxCursor,
                [](CXCursor cxCursor, CXCursor cxParent, CXClientData cxData)
                {
                    auto *data = static_cast<ClientData*>(cxData);
                    auto *klass = static_cast<ASTStruct*>(data->parent);
                    return data->generator->visitClassChildren(cxCursor, cxParent, klass);
                },
                &data);

            // 如果是 Tiny3D::Object 派生类，默认就是构造出指针对象，否则看 specifiers 指定的标记来定
            bool isDerivedOfbject = klass->isDerivedOf("Tiny3D::Object");
            if (isDerivedOfbject)
            {
                klass->ConstructAsPointer = isDerivedOfbject;
            }
            else
            {
                bool constructAsPointer = false;

                if (!isTemplate)
                {
                    if (isClass)
                    {
                        // 类默认构造是指针返回
                        constructAsPointer = true;
                    }
                    else
                    {
                        // 结构体默认构造是普通对象返回
                        constructAsPointer = false;
                    }
                }

                if (!inWhitelist)
                {
                    for (const auto& spec : itrSpec->second)
                    {
                        // 如果有设置指示符构造返回类型，那就用指定的
                        if (spec.name == kSpecConstructAsPointer)
                        {
                            constructAsPointer = true;
                        }
                        else if (spec.name == kSpecConstructAsObject)
                        {
                            constructAsPointer = false;
                        }
                    }
                }
                else
                {
                    constructAsPointer = false;
                }

                klass->ConstructAsPointer = constructAsPointer;
            }

            if (klass->HasConstructor && !klass->HasDefaultConstructor)
            {
                // 有构造函数但是没有默认构造函数，需要报错
                ret = T3D_ERR_RP_NO_DEFAULT_CONSTRUCTOR;
                RP_LOG_ERROR("The class %s has constructor but no default constructor [%s:%u] !",
                    name.c_str(), path.c_str(), start);
                break;
            }
            
            if (!klass->HasConstructor && !clang_CXXRecord_isAbstract(cxCursor))
            {
                // 没有任何构造函数，并且非抽象类，添加默认构造函数
                ASTFunction *function = new ASTFunction(name);
                klass->addChild(name, function);
                ASTConstructor *constructor = new ASTConstructor(name);
                function->addChild(name, constructor);
            }
        } while (false);
        
        return ret;
    }

    //-------------------------------------------------------------------------

#if 1
    TResult ReflectionGenerator::processClassBaseSpecifier(CXCursor cxCursor, CXCursor cxParent, ASTNode *parent)
    {
        TResult ret = T3D_OK;

        do
        {
            const std::function<ASTNode*(CXCursor)> getASTStructNode = [this](CXCursor cxCursor) -> ASTNode*
            {
                StringList names;
                CXCursor cxAncestor = cxCursor;
                CXCursorKind cxAncestorKind = clang_getCursorKind(cxAncestor);
                while (!clang_equalCursors(cxAncestor, clang_getNullCursor())
                    && cxAncestorKind != CXCursor_TranslationUnit)
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
            node = parent;//getASTStructNode(cxParent);
            if (node == nullptr)
            {
                ret = T3D_ERR_NOT_FOUND;
                break;
            }
            
            auto derivedNode = dynamic_cast<ASTStruct*>(node);

            // 检测反射继承链是否跟实际父子类继承链相一致
            T3D_ASSERT(baseNode->RTTIEnabled,
                "The reflection hierarchy is not correspondent with the class derived hierarchy !");
            auto itr = std::find(derivedNode->RTTIBaseClasses->begin(), derivedNode->RTTIBaseClasses->end(), baseNode->getName());
            if (itr == derivedNode->RTTIBaseClasses->end())
            {
                // 父类没有在使用 RTTR_ENABLE 打开反射功能
                ret = T3D_ERR_RP_BASE_CLASS_NO_RTTI;
                RP_LOG_ERROR("Base class of class %s has not enabled RTTI feature [%s:%u] ! "
                    "Please use TRTTI_ENABLE() in class declaration to enable this feature.",
                    derivedNode->getName().c_str(), baseNode->FileInfo.Path.c_str(), baseNode->FileInfo.StartLine);
                break;
            }

            // 都合法，放到基类列表中
            derivedNode->BaseClasses.insert(ASTBaseClassesValue(baseNode->getName(), baseNode));
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
    TResult ReflectionGenerator::processFunctionDeclaration(CXCursor cxCursor, CXCursor cxParent, bool isCXXMember, bool isConstructor, bool isDestructor, bool isTemplate)
    {
        TResult ret = T3D_OK;

        ASTNode *node = nullptr;
        
        do
        {
            bool isFriend = false;    // 是否对RTTR开启友元访问权限
            bool isProperty = false;
            bool asConstructor = false; // 是否作为构造函数使用
            bool rval;
            SpecifiersItr itrSpec;
            bool inWhitelist = false;
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
                    
                    // 属性函数
                    isProperty = true;
                }
                
                fileInfo.Path = path;
                fileInfo.StartLine = start;
                fileInfo.EndLine = end;

                SpecifiersItr itrClsSpec;
                FilesItr itrFile;
                
                if (cxParent.kind == CXCursor_ClassDecl
                    || cxParent.kind == CXCursor_ClassTemplate
                    || cxParent.kind == CXCursor_ClassTemplatePartialSpecialization
                    || cxParent.kind == CXCursor_StructDecl)
                {
                    // 是类函数，还需要类有打反射标签
                    CHECK_TAG_RET_FILE_SPEC(rval, itrFile, itrClsSpec, cxParent, classes, path, start, end, column, offset);
                    if (!rval)
                    {
                        // 结构体成员函数，需要结构体有打反射标签
                        CHECK_TAG_RET_FILE_SPEC(rval, itrFile, itrClsSpec, cxParent, structs, path, start, end, column, offset);
                        if (!rval)
                        {
                            break;
                        }
                    }
                    
                    isFriend = isRTTIFriend(itrFile->second, start, end);
                    // isFriend = itrFile->second->isFriend;
                }
                // else if (cxParent.kind == CXCursor_StructDecl)
                // {
                //     // 结构体成员函数，需要结构体有打反射标签
                //     CHECK_TAG_RET_FILE_SPEC(rval, itrFile, itrClsSpec, cxParent, structs, path, start, end, column, offset);
                //     if (!rval)
                //     {
                //         break;
                //     }
                //     
                //     isFriend = isRTTIFriend(itrFile->second, start, end);
                //     // isFriend = itrFile->second->isFriend;
                // }

                // 查找是否有打标签，作为构造函数用
                for (const auto &spec : itrSpec->second)
                {
                    if (spec.name == kSpecAsConstructor)
                    {
                        asConstructor = true;
                    }
                }
            }
            else
            {
                FilesItr itrFile;

                if (clang_CXXRecord_isAbstract(cxParent))
                {
                    // 抽象类不能反射构造，要跳过
                    break;
                }
                
                // 构造函数和析构函数不用打标签，只要类打了标签就可以反射
                String path;
                uint32_t start, end, column, offset;
                if (cxParent.kind == CXCursor_ClassDecl
                    || cxParent.kind == CXCursor_ClassTemplate
                    || cxParent.kind == CXCursor_ClassTemplatePartialSpecialization
                    || cxParent.kind == CXCursor_StructDecl)
                {
                    // 类函数，需要类有打反射标签
                    CHECK_TAG_RET_FILE_SPEC(rval, itrFile, itrSpec, cxParent, classes, path, start, end, column, offset);

                    // 获取类名
                    CXString cxClassName = clang_getCursorSpelling(cxParent);
                    String className = toString(cxClassName);

                    // 获取父结点
                    ASTNode *parentn = getOrConstructParentNode(cxParent);
                    if (parentn == nullptr)
                    {
                        // 父结点不存在
                        ret = T3D_ERR_RP_AST_NO_PARENT;
                        RP_LOG_ERROR("The parent of class %s is null [%s:%u] !",
                            className.c_str(), path.c_str(), start);
                        break;
                    }

                    auto itrWhitelist = mClassWhiteList.find(className);
                    if (itrWhitelist != mClassWhiteList.end())
                    {
                        // 在白名单里，再看看命名空间是否对得上
                        String hierarchyName = parentn->getHierarchyName();
                        hierarchyName = hierarchyName + "::" + className;
                        inWhitelist = (hierarchyName == itrWhitelist->second);
                    }
                    
                    if (!inWhitelist && !rval)
                    {
                        // 结构体函数，需要结构体有打反射标签
                        CHECK_TAG_RET_FILE_SPEC(rval, itrFile, itrSpec, cxParent, structs, path, start, end, column, offset);
                        if (!rval)
                        {
                            break;
                        }
                    }
                    else if (inWhitelist && !clang_CXXConstructor_isMoveConstructor(cxCursor)
                        && !clang_CXXConstructor_isCopyConstructor(cxCursor)
                        && !clang_CXXConstructor_isDefaultConstructor(cxCursor))
                    {
                        // 白名单类里面的非默认构造、非拷贝构造、非赋值构造，均不自动反射
                        break;
                    }
                }
                else
                {
                    break;
                }

                if (inWhitelist)
                {
                    isFriend = false;
                }
                else
                {
                    isFriend = isRTTIFriend(itrFile->second, start, end);
                }
                
                getASTNodeInfo(cxCursor, path, start, end, column, offset);
                fileInfo.Path = path;
                fileInfo.StartLine = start;
                fileInfo.EndLine = end;
                
                // isFriend = itrFile->second->isFriend;
            }

            auto cxxAccess = clang_getCXXAccessSpecifier(cxCursor);
            if ((cxxAccess == CX_CXXPrivate || cxxAccess == CX_CXXProtected)
                && !isFriend)
            {
                // 私有、保护访问，并且没有开启友元，忽略该反射
                break;
            }
            
            // 创建函数重载结点，哪怕没有函数重载，函数的声明都在该结点上，ASTFunction 只是函数入口结点
            CXString cxStrUSR = clang_getCursorUSR(cxCursor);
            String USR, name;
            if (isProperty)
            {
                // 属性函数
                CXString cxName = clang_getCursorSpelling(cxCursor);
                name = toString(cxName);
                USR = toString(cxStrUSR);
            }
            else
            {
                // 函数
                USR = toString(cxStrUSR);
                name = USR;
            }
            
            ASTOverloadFunction *overload = nullptr;

            if (isConstructor || asConstructor)
            {
                // 构造函数
                overload = new ASTConstructor(name);

                if (asConstructor)
                {
                    ASTConstructor *constructor = static_cast<ASTConstructor *>(overload);
                    constructor->IsNormal = false;
                }
            }
            else if (isDestructor)
            {
                // 析构函数
                break;
            }
            else if (clang_CXXMethod_isStatic(cxCursor))
            {
                // 静态函数
                overload = new ASTStaticFunction(name);
            }
            else if (isCXXMember)
            {
                // 成员函数
                overload = new ASTInstanceFunction(name);
            }
            else
            {
                // 普通函数
                overload = new ASTOverloadFunction(name);
            }

            overload->IsConst = (clang_CXXMethod_isConst(cxCursor) == 1);

            // 获取父结点
            ASTNode *parent = getOrConstructParentNode(cxCursor);
            if (parent == nullptr)
            {
                delete overload;
                ret = T3D_ERR_RP_AST_NO_PARENT;
                RP_LOG_ERROR("The parent is null [%s:%u] !", fileInfo.Path.c_str(), fileInfo.StartLine);
                break;
            }

            if (isConstructor)
            {
                T3D_ASSERT(parent->getType() == ASTNode::Type::kClass
                    || parent->getType() == ASTNode::Type::kStruct
                    || parent->getType() == ASTNode::Type::kClassTemplate,
                    "The type of parent node in ast is invalid !");
                ASTClass *klass = static_cast<ASTClass *>(parent);
                klass->HasConstructor = true;

                if (clang_CXXConstructor_isDefaultConstructor(cxCursor))
                {
                    klass->HasDefaultConstructor = true;
                }
            }
            
            CXType cxType = clang_getCursorType(cxCursor);

            if (parent->getType() == ASTNode::Type::kClassTemplate
                && isConstructor && clang_CXXConstructor_isCopyConstructor(cxCursor))
            {
                // 函数参数，就是自己的引用
                String paramType = parent->getHierarchyName();
                int32_t numArgs = clang_Cursor_getNumArguments(cxCursor);
                for (int32_t i = 0; i < numArgs; i++)
                {
                    ASTFunctionParam param;
                    // 参数名
                    CXCursor cxArg = clang_Cursor_getArgument(cxCursor, i);
                    param.Name = toString(clang_getCursorSpelling(cxArg));
                    // 参数类型
                    param.Type = "const " + paramType + "<";
                    ASTClassTemplate *klassTemplate = static_cast<ASTClassTemplate*>(parent);
                    size_t j = 0;
                    for (const auto &temp : klassTemplate->TemplateParams)
                    {
                        param.Type += temp.type;
                        if (j != klassTemplate->TemplateParams.size() - 1)
                        {
                            param.Type += ", ";
                        }
                        j++;
                    }
                    param.Type += "> &";
                    overload->Params.push_back(param);
                }
            }
            else if (parent->getType() == ASTNode::Type::kClassTemplate
                && isConstructor && clang_CXXConstructor_isMoveConstructor(cxCursor))
            {
                // 函数参数，就是自己的引用
                String paramType = parent->getHierarchyName();
                int32_t numArgs = clang_Cursor_getNumArguments(cxCursor);
                for (int32_t i = 0; i < numArgs; i++)
                {
                    ASTFunctionParam param;
                    // 参数名
                    CXCursor cxArg = clang_Cursor_getArgument(cxCursor, i);
                    param.Name = toString(clang_getCursorSpelling(cxArg));
                    // 参数类型
                    param.Type = paramType + "<";
                    ASTClassTemplate *klassTemplate = static_cast<ASTClassTemplate*>(parent);
                    size_t j = 0;
                    for (const auto &temp : klassTemplate->TemplateParams)
                    {
                        param.Type += temp.type;
                        if (j != klassTemplate->TemplateParams.size() - 1)
                        {
                            param.Type += ", ";
                        }
                        j++;
                    }
                    param.Type += "> &&";
                    overload->Params.push_back(param);
                }
            }
            else
            {
                // 函数返回值
                CXType cxResultType = clang_getResultType(cxType);
                overload->RetType = toString(clang_getTypeSpelling(cxResultType));

                // CXCursor cxResultCursor = clang_getTypeDeclaration(cxResultType);
                // instantiateClassTemplate(cxResultCursor);
            
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

                    // cxArgType = clang_getCursorType(cxArg);
                    // CXCursor cxArgCursor = clang_getTypeDeclaration(cxArgType);
                    // instantiateClassTemplate(cxArgCursor);
                }
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

                if (isTemplate)
                {
                    funcName = "#TEMPLATE#" + funcName;
                }
            }

            ASTNode *child = parent->getChild(funcName);
            if (child == nullptr
                || (isTemplate && parent->getType() != ASTNode::Type::kFunctionTemplate))
            {
                // 没有函数结点，创建之
                ASTFunction *function = nullptr;
                if (isTemplate)
                {
                    // 模板函数
                    function = new ASTFunctionTemplate(funcName);
                }
                else
                {
                    // 非模板函数
                    function = new ASTFunction(funcName);
                }
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

            node = overload;
            
            if ((cxParent.kind == CXCursor_Namespace
                || cxParent.kind == CXCursor_TranslationUnit)
                && !isTemplate)
            {
                // 非类和结构体成员函数、并且非模板函数
                insertSourceFiles(overload->FileInfo.Path, parent, false);
            }
            else if (cxParent.kind != CXCursor_ClassTemplate && isTemplate)
            {
                // 非类的成员模板函数
                insertFunctionTemplate(USR, overload);
            }
        } while (false);

        ClientData data = {node, this};
        clang_visitChildren(cxCursor,
            [](CXCursor cxCursor, CXCursor cxParent, CXClientData cxData)
            {
                auto *data = static_cast<ClientData*>(cxData);
                auto *parent = static_cast<ASTFunction*>(data->parent);
                return data->generator->visitFunctionChildren(cxCursor, cxParent, parent);
            },
            &data);
        
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

            FilesItr itrFile;
            SpecifiersItr itrClsSpec;
            bool isFriend = false;
            
            // 枚举属于 class 或 struct，看 class 或 struct 是否有打反射标签
            if (cxParent.kind == CXCursor_ClassDecl
                || cxParent.kind == CXCursor_ClassTemplate
                || cxParent.kind == CXCursor_ClassTemplatePartialSpecialization
                || cxParent.kind == CXCursor_StructDecl)
            {
                CHECK_TAG_RET_FILE_SPEC(rval, itrFile, itrClsSpec, cxParent, classes, path, start, end, col, offset);
                if (!rval)
                {
                    CHECK_TAG_RET_FILE_SPEC(rval, itrFile, itrClsSpec, cxParent, structs, path, start, end, col, offset);
                    if (!rval)
                    {
                        break;
                    }
                }

                isFriend = isRTTIFriend(itrFile->second, start, end);
            }
            // else if (cxParent.kind == CXCursor_StructDecl)
            // {
            //     CHECK_TAG_RET_FILE_SPEC(rval, itrFile, itrClsSpec, cxParent, structs, path, start, end, col, offset);
            //     if (!rval)
            //     {
            //         break;
            //     }
            //     
            //     isFriend = isRTTIFriend(itrFile->second, start, end);
            // }

            auto cxxAccess = clang_getCXXAccessSpecifier(cxCursor);
            if ((cxxAccess == CX_CXXPrivate || cxxAccess == CX_CXXProtected)
                && !isFriend)
            {
                // 私有、保护访问，并且没有开启友元，忽略该反射
                break;
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
            
            if (cxParent.kind == CXCursor_Namespace
                || cxParent.kind == CXCursor_TranslationUnit)
            {
                // 非类和结构体函数
                insertSourceFiles(path, enumeration, false);
            }

            ClientData data = {enumeration, this};
            clang_visitChildren(cxCursor,
                [](CXCursor cxCursor, CXCursor cxParent, CXClientData cxData)
                {
                    auto *data = static_cast<ClientData*>(cxData);
                    auto *enumeration = static_cast<ASTEnum*>(data->parent);
                    return data->generator->visitEnumChildren(cxCursor, cxParent, enumeration);
                },
                &data);
        } while (false);

        return ret;
    }

    //-------------------------------------------------------------------------

    TResult ReflectionGenerator::processEnumConstDeclaration(CXCursor cxCursor, CXCursor cxParent, ASTNode *parent)
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
            // ASTNode *parent = getOrConstructParentNode(cxCursor);
            // if (parent == nullptr)
            // {
            //     ret = T3D_ERR_RP_AST_NO_PARENT;
            //     RP_LOG_ERROR("The parent is null [%s:%u] !", path.c_str(), start);
            //     break;
            // }

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
            
            FilesItr itrFile;
            SpecifiersItr itrClsSpec;
            bool isFriend = false;
            
            // 枚举属于 class 或 struct，看 class 或 struct 是否有打反射标签
            if (cxParent.kind == CXCursor_ClassDecl
                || cxParent.kind == CXCursor_ClassTemplate
                || cxParent.kind == CXCursor_ClassTemplatePartialSpecialization
                || cxParent.kind == CXCursor_StructDecl)
            {
                CHECK_TAG_RET_FILE_SPEC(rval, itrFile, itrClsSpec, cxParent, classes, path, start, end, col, offset);
                if (!rval)
                {
                    CHECK_TAG_RET_FILE_SPEC(rval, itrFile, itrClsSpec, cxParent, structs, path, start, end, col, offset);
                    if (!rval)
                    {
                        break;
                    }
                }

                isFriend = isRTTIFriend(itrFile->second, start, end);
            }
            // else if (cxParent.kind == CXCursor_StructDecl)
            // {
            //     CHECK_TAG_RET_FILE_SPEC(rval, itrFile, itrClsSpec, cxParent, structs, path, start, end, col, offset);
            //     if (!rval)
            //     {
            //         break;
            //     }
            //     isFriend = isRTTIFriend(itrFile->second, start, end);
            // }

            auto cxxAccess = clang_getCXXAccessSpecifier(cxCursor);
            if ((cxxAccess == CX_CXXPrivate || cxxAccess == CX_CXXProtected)
                && !isFriend)
            {
                // 私有、保护访问，并且没有开启友元，忽略该反射
                break;
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

            if (cxParent.kind == CXCursor_Namespace
                || cxParent.kind == CXCursor_TranslationUnit)
            {
                // 非类和结构体函数
                insertSourceFiles(path, property, false);       
            }

            instantiateClassTemplate(cxCursor, path);
            
            // RP_LOG_INFO("Variable -> Name : %s, Type : %s, CXXMember : %d (Parent Name : %s, Type : %s)",
            //     toString(clang_getCursorSpelling(cxCursor)).c_str(),
            //     toString(clang_getTypeSpelling(clang_getCursorType(cxCursor))).c_str(),//toString(clang_getCursorKindSpelling(cxCursor.kind)).c_str(),
            //     isCXXMember,
            //     toString(clang_getCursorSpelling(cxParent)).c_str(),
            //     toString(clang_getCursorKindSpelling(cxParent.kind)).c_str());
        } while (false);

        // ClientData data = {nullptr, this};
        // clang_visitChildren(cxCursor,
        //     [](CXCursor cxCursor, CXCursor cxParent, CXClientData cxData)
        //     {
        //         auto *data = static_cast<ClientData*>(cxData);
        //         return data->generator->visitVariableChildren(cxCursor, cxParent, nullptr);
        //     },
        //     &data);
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

    TResult ReflectionGenerator::processTemplateParameter(CXCursor cxCursor, CXCursor cxParent, ASTNode *parent)
    {
        TResult ret = T3D_OK;

        do
        {
            String path;
            uint32_t start, end, col, offset;
            getASTNodeInfo(cxCursor, path, start, end, col, offset);
            
            // T3D_ASSERT(parent->getType() == ASTNode::Type::kClassTemplate);
            
            String name = toString(clang_getCursorSpelling(cxCursor));

            ASTTemplate *temp = nullptr;

            switch (parent->getType())
            {
            case ASTNode::Type::kClassTemplate:
                temp = static_cast<ASTClassTemplate*>(parent);
                break;
            case ASTNode::Type::kOverloadFunction:
            case ASTNode::Type::kStaticFunction:
            case ASTNode::Type::kInstanceFunction:
            case ASTNode::Type::kConstructor:
                T3D_ASSERT(parent->getParent()->getType() == ASTNode::Type::kFunctionTemplate,
                    "The type of parent node in ast is invalid !");
                temp = static_cast<ASTFunctionTemplate*>(parent->getParent());
                break;
            }

            if (temp == nullptr)
            {
                break;
            }

            // 看是否已经添加过，如果添加过，则不添加该参数
            bool found = false;
            for (const auto &param : temp->TemplateParams)
            {
                if (param.name == name)
                {
                    found = true;
                    break;
                }
            }

            if (found)
            {
                break;
            }

            CXType cxType = clang_getCursorType(cxCursor);
            String type = toString(clang_getTypeSpelling(cxType));
            ASTTemplateParam param;
            param.type = type;
            param.name = name;

            switch (cxCursor.kind)
            {
            case CXCursor_TemplateTypeParameter:
                {
                    param.kind = ASTTemplateParam::Kind::kTemplateType;
                }
                break;
            case CXCursor_NonTypeTemplateParameter:
                {
                    param.kind = ASTTemplateParam::Kind::kNonType;
                }
                break;
            case CXCursor_TemplateTemplateParameter:
                {
                    param.kind = ASTTemplateParam::Kind::kTemplateTemplate;
                }
                break;
            default:
                break;
            }

            temp->TemplateParams.push_back(std::move(param));
        } while (false);

        return ret;
    }

    //-------------------------------------------------------------------------

    TResult ReflectionGenerator::processDefaultArguments(CXCursor cxCursor, CXCursor cxParent, ASTFunction *parent, const String &defaultName)
    {
        TResult ret = T3D_OK;

        do
        {
            CXSourceRange cxRange = clang_getCursorExtent(cxCursor);
            CXSourceLocation cxStart = clang_getRangeStart(cxRange);
            CXTranslationUnit cxUnit = clang_Cursor_getTranslationUnit(cxCursor);
            CXToken *cxTokens;
            uint32_t cxNumTokens = 0;
            clang_tokenize(cxUnit, cxRange, &cxTokens, &cxNumTokens);
            String token;
            for (uint32_t i = 0; i < cxNumTokens; ++i)
            {
                CXString cxStr = clang_getTokenSpelling(cxUnit, cxTokens[i]);
                token += toString(cxStr);
            }
            // RP_LOG_INFO("FunctionChildren -- [%s] %s : %s (parent %s : %s)", toString(cxStrToken).c_str(), type.c_str(), name.c_str(), parentType.c_str(), parentName.c_str());
            ASTOverloadFunction *function = static_cast<ASTOverloadFunction*>(parent);
            for (auto &param : function->Params)
            {
                if (param.Name == defaultName)
                {
                    // ASTNode *klass = nullptr;
                    // ASTNode *node = parent;
                    // while (node != nullptr)
                    // {
                    //     if (node->getType() == ASTNode::Type::kClass || node->getType() == ASTNode::Type::kStruct
                    //         || node->getType() == ASTNode::Type::kClassTemplate)
                    //     {
                    //         klass = node;
                    //         break;
                    //     }
                    //     node = node->getParent();
                    // }
                    //
                    // if (klass != nullptr)
                    // {
                    //     node = klass->findChild(token);
                    //     if (node != nullptr)
                    //     {
                    //         token = node->getHierarchyName();
                    //     }
                    // }
                    
                    param.Default = token;
                    break;
                }
            }
        } while (false);

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
                T3D_ASSERT(0, "Invlid kind of CXCursor !");
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
        CXCursorKind cxAncestorKind = cxCursor.kind;
        while (!clang_equalCursors(cxAncestor, clang_getNullCursor())
            && cxAncestorKind != CXCursor_TranslationUnit)
        {
            CXString cxAncestorName = clang_getCursorSpelling(cxAncestor);
            String ancestorName = toString(cxAncestorName);
            nodes.push_front({ancestorName, cxAncestorKind});

            // T3D_ASSERT(cxAncestorKind == CXCursor_Namespace
            //     || cxAncestorKind == CXCursor_ClassTemplate
            //     || cxAncestorKind == CXCursor_ClassTemplatePartialSpecialization
            //     || cxAncestorKind == CXCursor_ClassDecl
            //     || cxAncestorKind == CXCursor_StructDecl
            //     || cxAncestorKind == CXCursor_EnumDecl
            //     || cxAncestorKind == CXCursor_FunctionTemplate);
                
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
                || parent->getType() == ASTNode::Type::kEnum,
                "The type of parent node in ast is invalid !");
        }

        return parent;
    }

    //-------------------------------------------------------------------------

    void ReflectionGenerator::insertSourceFiles(const String &path, ASTNode *node, bool isTemplate, const StringList &pathes)
    {
        do
        {
            // 查找该类是否当前工程的，如果是，放到里面
            auto itr = mSourceFiles.find(path);
            if (itr == mSourceFiles.end())
            {
                if (isTemplate)
                {
                    // 模板实例化，那加到源码文件里，避免模板没有实例化
                    auto ret = mSourceFiles.insert(SourceFilesMapValue(path, ASTNodeMap()));
                    itr = ret.first;

                    // 提取头文件作为 cpp 文件中包含的头文件
                    String dir, title, ext;
                    Dir::parsePath(path, dir, title, ext);

                    auto rval = mHeaderFiles.insert(HeaderFilesMapValue(title, StringList()));

                    // 根据包含路径，搜索到对应路径，并提取头文件
                    for (const auto &includePath : mIncludePathes)
                    {
                        String pattern = includePath + "*";
                        String headerPath = path;
                        if (StringUtil::match(headerPath, pattern, false))
                        {
                            StringUtil::replaceAll(headerPath, includePath, "");
                            if (headerPath[0] == Dir::getNativeSeparator())
                            {
                                headerPath.erase(0, 1);
                                rval.first->second.push_back(headerPath);
                            }
                        }

                        for (const auto &str : pathes)
                        {
                            headerPath = str;
                            if (StringUtil::match(headerPath, pattern, false))
                            {
                                StringUtil::replaceAll(headerPath, includePath, "");
                                if (headerPath[0] == Dir::getNativeSeparator())
                                {
                                    headerPath.erase(0, 1);
                                    auto it = std::find(rval.first->second.begin(), rval.first->second.end(), headerPath);
                                    if (it == rval.first->second.end())
                                    {
                                        rval.first->second.push_back(headerPath);
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    break;
                }
            }
            else if (!isTemplate)
            {
                // 非模板，那必须是工程文件才可以放入导出文件列表
                String pattern = mProjectPath + "*";
                if (!StringUtil::match(path, pattern, false))
                {
                    break;
                }
            }
            else if (isTemplate && !pathes.empty())
            {
                // 打个 patch 补已经加过源码文件，但没有头文件的问题

                // 提取头文件作为 cpp 文件中包含的头文件
                String dir, title, ext;
                Dir::parsePath(path, dir, title, ext);

                auto itr = mHeaderFiles.find(title);

                // 根据包含路径，搜索到对应路径，并提取头文件
                for (const auto &includePath : mIncludePathes)
                {
                    String pattern = includePath + "*";
                    String headerPath = path;
                    if (StringUtil::match(headerPath, pattern, false))
                    {
                        StringUtil::replaceAll(headerPath, includePath, "");
                        if (headerPath[0] == Dir::getNativeSeparator())
                        {
                            headerPath.erase(0, 1);
                            auto it = std::find(itr->second.begin(), 
                                itr->second.end(), 
                                headerPath);
                            if (it == itr->second.end())
                            {
                                itr->second.push_back(headerPath);
                            }
                        }
                    }

                    for (const auto &str : pathes)
                    {
                        headerPath = str;
                        if (StringUtil::match(headerPath, pattern, false))
                        {
                            StringUtil::replaceAll(headerPath, includePath, "");
                            if (headerPath[0] == Dir::getNativeSeparator())
                            {
                                headerPath.erase(0, 1);
                                auto it = std::find(itr->second.begin(),
                                                    itr->second.end(),
                                                    headerPath);
                                if (it == itr->second.end())
                                {
                                    itr->second.push_back(headerPath);
                                }
                            }
                        }
                    }
                }
            }

            String hierarchyName = node->getHierarchyName();
            auto it = itr->second.find(hierarchyName);
            if (it != itr->second.end())
            {
                // 该类已经在里面了，忽略之
                break;
            }

            itr->second.insert(ASTNodeMapValue(hierarchyName, node));
        } while (false);
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

            // 构造需要的头文件
            auto itr = mHeaderFiles.find(title);
            if (itr == mHeaderFiles.end())
            {
                continue;
            }

            // String headerPath = itr->second;
            // for (const auto &includePath : mIncludePathes)
            // {
            //     String pattern = includePath + "*";
            //     if (StringUtil::match(headerPath, pattern, false))
            //     {
            //         StringUtil::replaceAll(headerPath, includePath, "");
            //         if (headerPath[0] == Dir::getNativeSeparator())
            //         {
            //             headerPath.erase(0, 1);
            //         }
            //     }
            // }
            
            if (!fs.open(path.c_str(), FileDataStream::E_MODE_TEXT | FileDataStream::E_MODE_TRUNCATE | FileDataStream::E_MODE_READ_WRITE))
            {
                // 文件打开失败
                continue;
            }
            
            // 文件头注释
            fs << "// Copyright (C) 2015-2020  Answer Wong" << std::endl;
            fs << "// Generated code exported from ReflectionPreprocessor." << std::endl;
            fs << "// DO NOT modify this manually! Edit the corresponding .h files instead!" << std::endl;
                
            // 需要包含的头文件
            fs << std::endl;
            fs << "#include <rttr/registration>" << std::endl;
            // fs << "#include \"" << headerPath << (headerPath.empty() ? "" : "/") << title << ".h\"" << std::endl;
            for (const auto &header : itr->second)
            {
                fs << "#include \"" << header << "\"" << std::endl;
            }

            // 开始注册类信息
            fs << std::endl << "RTTR_REGISTRATION" << std::endl;
            fs << "{" << std::endl;
            
            {
                fs << "\tusing namespace rttr;" << std::endl;
            
                for (const auto &value : val.second)
                {
                    ASTNode *parent = value.second->getParent();
                    bool hasNS = false, first = false;
                    if (parent != nullptr && parent->getType() == ASTNode::Type::kNamespace)
                    {
                        hasNS = true;
                        first = true;
                        fs << std::endl << "\tusing namespace ";
                    }
                    while (parent != nullptr && parent->getType() == ASTNode::Type::kNamespace)
                    {
                        if (!first)
                        {
                            fs << "::";
                        }
                        fs << parent->getName();
                        parent = parent->getParent();
                    }
                    if (hasNS)
                    {
                        fs << ";" << std::endl;
                    }

                    value.second->generateSourceFile(fs);
                }
            }
            
            // 结束注册类信息
            fs << "}" << std::endl;
                
            // 是在这里打开的文件，就在这里关闭
            fs.close();
        }

        if (fs.isOpened())
        {
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

    void ReflectionGenerator::insertFunctionTemplate(const String &name, ASTOverloadFunction *function)
    {
        // 查找该函数是否 cache 了，有则忽略
        auto itr = mFunctionTemplates.find(name);
        if (itr != mFunctionTemplates.end())
        {
            // 该类已经在里面了，忽略之
            return;
        }

        mFunctionTemplates.insert(ASTFunctionTemplateMapValue(name, function));
    }

    //-------------------------------------------------------------------------

    TResult ReflectionGenerator::instantiateClassTemplate(CXCursor cxCursor, const String &headerPath)
    {
        TResult ret = T3D_OK;

        do
        {
            // CXCursor cxTemplateInst = clang_getSpecializedCursorTemplate(cxCursor);
            // String inst = toString(clang_getCursorSpelling(cxTemplateInst));
            // int numOfInstParams = clang_Cursor_getNumTemplateArguments(cxCursor);

            CXType cxVarType = clang_getCursorType(cxCursor);
            int numOfTemplateArg = clang_Type_getNumTemplateArguments(cxVarType);
            
            if (numOfTemplateArg == -1)
            {
                // 不是模板，跳过
                break;
            }
            
            String varType = toString(clang_getTypeSpelling(clang_getCanonicalType(cxVarType)));
            //String varType = toString(clang_getTypeSpelling(cxVarType));
            // CXType cxCanonicalType = clang_getCanonicalType(cxVarType);
            // String canonicalType = toString(clang_getTypeSpelling(cxCanonicalType));
            CXCursor cxCursorDecl = clang_getTypeDeclaration(cxVarType);
            CXCursor cxTemplateCursor = clang_getSpecializedCursorTemplate(cxCursorDecl);
            String templateName = toString(clang_getCursorUSR(cxTemplateCursor));
            // CXType cxType = clang_getCursorType(cxCursorDecl);
            // String typeName = toString(clang_getTypeSpelling(cxType));
            String USR = toString(clang_getCursorUSR(cxCursorDecl));
            String declName = toString(clang_getCursorSpelling(cxCursorDecl));

            auto itrTemplate = mClassTemplates.find(templateName);
            if (itrTemplate == mClassTemplates.end())
            {
                // 居然没有对应模板，那就跳开吧
                break;
            }

            // const auto tnames = StringUtil::split2(varType, "::");
            StringList names = split(varType);
            const String &name = names.back();
            // const String &name = declName;

            StringArray actualParams;
            actualParams.reserve(numOfTemplateArg);
            // 收集模板实参
            uint32_t i = 0;
            for (i = 0; i < numOfTemplateArg; i++)
            {
                CXType cxArgType = clang_Type_getTemplateArgumentAsType(cxVarType, i);
                String argName = toString(clang_getTypeSpelling(cxArgType));
                actualParams.push_back(argName);
                // RP_LOG_INFO("Template argument [%u] type : %s", i, argName.c_str());
            }
            
            // 创建一个 class template instance 出来，复制属性并加到 AST 上
            ASTClassTemplate *klassTemplate = itrTemplate->second;
            ASTNode *parent = klassTemplate->getParent();
            if (parent->getChild(varType) != nullptr)
            {
                // 已经实例化过了，就不实例化了
                break;
            }

            // 收集形参
            StringArray formalParams;
            for (const auto &param : klassTemplate->TemplateParams)
            {
                switch (param.kind)
                {
                case ASTTemplateParam::Kind::kTemplateType:
                    {
                        formalParams.push_back(param.type);
                    }
                    break;
                case ASTTemplateParam::Kind::kNonType:
                    {
                        formalParams.push_back("");
                    }
                    break;
                case ASTTemplateParam::Kind::kTemplateTemplate:
                    {
                        formalParams.push_back(param.name);
                    }
                    break;
                default:
                    break;
                }
            }

            String path;
            uint32_t start, end, col, offset;
            getASTNodeInfo(cxCursor, path, start, end, col, offset);
            
            ASTClassTemplate *templateInstance = static_cast<ASTClassTemplate *>(klassTemplate->clone());
            parent->addChild(name, templateInstance);
            templateInstance->setName(name);

            // 替换模板实参，变成模板类
            templateInstance->replaceTemplateParams(formalParams, actualParams);

            // 加入到导出文件
            String decl = name;
            StringUtil::replaceAll(decl, "<", "_");
            StringUtil::replaceAll(decl, ",", "_");
            StringUtil::replaceAll(decl, ">", "_");
            StringUtil::replaceAll(decl, " ", "");
            StringList pathes;
            pathes.push_back(path);
            pathes.push_back(headerPath);
            insertSourceFiles(templateInstance->FileInfo.Path, templateInstance, true, pathes);
        } while (false);
        
        return ret;
    }

    //-------------------------------------------------------------------------

    TResult ReflectionGenerator::instantiateFunctionTemplate(CXCursor cxCursor)
    {
        TResult ret = T3D_OK;

        do
        {
            CXType cxType = clang_getCursorType(cxCursor);
            if (cxType.kind != CXType_FunctionProto)
            {
                // 不是函数
                break;
            }

            CXCursor cxDeclCursor = clang_getCursorReferenced(cxCursor);
            CXCursor cxTemplateCursor = clang_getSpecializedCursorTemplate(cxDeclCursor);
            if (cxTemplateCursor.kind != CXCursor_FunctionTemplate)
            {
                // 不是函数模板
                break;
            }

            // 函数模板的名字和USR
            String strTemplate = toString(clang_getCursorSpelling(cxTemplateCursor));
            String strTemplateUSR = toString(clang_getCursorUSR(cxTemplateCursor));
            
            auto itr = mFunctionTemplates.find(strTemplateUSR);
            if (itr == mFunctionTemplates.end())
            {
                // 没有注册反射
                break;
            }

            // 函数模板实例化的名字和USR
            String strDecl = toString(clang_getCursorSpelling(cxDeclCursor));
            String strUSR = toString(clang_getCursorUSR(cxDeclCursor));

            int32_t numOfTemplateArgs = clang_Cursor_getNumTemplateArguments(cxDeclCursor);
            if (numOfTemplateArgs == -1)
            {
                break;
            }
            
            // CXCursor cxDeclParent = clang_getCursorSemanticParent(cxDeclCursor);
            // String strParent = toString(clang_getCursorSpelling(cxDeclParent));

            ASTOverloadFunction *functionTemplate = itr->second;

            // 收集模板形参
            // StringArray formalParams;
            // for (const auto &param : functionTemplate->TemplateParams)
            // {
            //     switch (param.kind)
            //     {
            //     case ASTTemplateParam::Kind::kTemplateType:
            //         {
            //             formalParams.push_back(param.type);
            //         }
            //         break;
            //     case ASTTemplateParam::Kind::kNonType:
            //         {
            //             formalParams.push_back("");
            //         }
            //         break;
            //     case ASTTemplateParam::Kind::kTemplateTemplate:
            //         {
            //             formalParams.push_back(param.name);
            //         }
            //         break;
            //     default:
            //         break;
            //     }
            // }

            ASTNode *parent = functionTemplate->getParent()->getParent();
            if (parent->getChild(strUSR))
            {
                // 实例化过了
                break;
            }

            ASTOverloadFunction *functionInstance = static_cast<ASTOverloadFunction *>(functionTemplate->clone());
            
            String name = toString(clang_getCursorSpelling(cxDeclCursor));
            // StringArray actualParams;
            // actualParams.reserve(numOfTemplateArgs);
            name += "<";
            // 收集模板实参
            int32_t i = 0;
            for (i = 0; i < numOfTemplateArgs; i++)
            {
                CXType cxArgType = clang_Cursor_getTemplateArgumentType(cxDeclCursor, i);
                String argName = toString(clang_getTypeSpelling(cxArgType));
                // actualParams.push_back(argName);
                name += argName;
                if (i != numOfTemplateArgs - 1)
                {
                    name += ", ";
                }
                RP_LOG_INFO("Template argument [%u] type : %s", i, argName.c_str());
            }
            name += ">";
            
            ASTFunctionTemplate *function = new ASTFunctionTemplate(name);
            function->addChild(strUSR, functionInstance);
            parent->addChild(strUSR, function);

            // 实例化函数模板，原函数模板是没有参数的
            int32_t numOfArgs = clang_Cursor_getNumArguments(cxDeclCursor);
                
            // 参数
            for (int32_t i = 0; i < numOfArgs; i++)
            {
                ASTFunctionParam param;
                // 参数名
                CXCursor cxArg = clang_Cursor_getArgument(cxCursor, i);
                param.Name = toString(clang_getCursorSpelling(cxArg));
                // 参数类型
                CXType cxArgType = clang_getArgType(cxType, i);
                param.Type = toString(clang_getTypeSpelling(cxArgType));
                functionInstance->Params.push_back(param);
            }

            // 返回值
            CXType cxResultType = clang_getResultType(cxType);
            functionInstance->RetType = toString(clang_getTypeSpelling(cxResultType));

            function->HasSpecialization = true;
            
            // 替换模板实参，变成模板函数
            // functionTemplate->replaceTemplateParams(formalParams, actualParams);

            // 加入到导出文件
            insertSourceFiles(functionInstance->FileInfo.Path, function, true);
        } while (false);
        
        return ret;
    }

    //-------------------------------------------------------------------------

    StringList ReflectionGenerator::split(const String &name) const
    {
        StringList names;
        String str;
        int inBracket = 0;
        String::size_type pos = 0;
        while (pos < name.length())
        {
            if (name[pos] == ':' && name[pos+1] == ':')
            {
                if (inBracket == 0)
                {
                    names.push_back(str);
                    str.clear();
                }
                else
                {
                    str.push_back(name[pos]);
                    str.push_back(name[pos+1]);
                }
                pos++;
            }
            else if (name[pos] == '<')
            {
                str.push_back(name[pos]);
                inBracket++;
            }
            else if (name[pos] == '>')
            {
                str.push_back(name[pos]);
                inBracket--;
            }
            else
            {
                str.push_back(name[pos]);
            }
            pos++;
            if (pos == name.length())
            {
                names.push_back(str);
            }
        }

        return names;
    }

    //-------------------------------------------------------------------------
}
