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


#include "T3DAbstractSyntaxTree.h"

#include <SDL_syswm.h>

#include "T3DRPErrorCode.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    
    ASTNode::ASTNode(const String &name)
        : Specifiers(nullptr)
        , mName(name)
        , mParent(nullptr)
    {
        
    }

    //--------------------------------------------------------------------------
    
    ASTNode::~ASTNode()
    {
        removeAllChildren();
    }
    
    //--------------------------------------------------------------------------
    
    void ASTNode::removeAllChildren()
    {
        for (const auto &val : mChildren)
        {
            ASTNode *child = val.second;
            child->removeAllChildren();
            delete child;
        }
        mChildren.clear();
    }

    //--------------------------------------------------------------------------

    String ASTNode::getHierarchyName() const
    {
        String name = getName();
        const ASTNode *node = getParent();
        while (node != nullptr)
        {
            if (node->getType() != Type::kNull)
            {
                name = node->getName() + "::" + name;    
            }
            node = node->getParent();
        }
        return name;
    }
    
    //--------------------------------------------------------------------------
    
    void ASTNode::dump(rapidjson::PrettyWriter<JsonStream> &writer) const
    {
        // Name
        writer.String(mName);

        {
            writer.StartObject();
        
            // Type
            writer.Key("Type");
            String str = getTypeString();
            writer.String(str);

            dumpProperties(writer);

            writer.Key("Children");

            {
                writer.StartObject();
                
                for (const auto &val : mChildren)
                {
                    val.second->dump(writer);
                }

                writer.EndObject();
            }            
        
            writer.EndObject();
        }
        
    }

    //--------------------------------------------------------------------------
    
    ASTNode *ASTNode::getChildRecursively(const StringList &names) const
    {
        ASTNode *child = nullptr;

        if (!names.empty())
        {
            auto itr = names.begin();
            child = getChildDirectly(*itr);
            ++itr;
        
            while (itr != names.end() && child != nullptr)
            {
                child = child->getChildDirectly(*itr);
                ++itr;
            }
        }
        
        return child;
    }
    
    //--------------------------------------------------------------------------

    void ASTNode::dumpProperties(rapidjson::PrettyWriter<JsonStream>& writer) const
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ASTNode::generateSourceFile(FileDataStream& fs) const
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ASTNode::generateMetaInfo(FileDataStream &fs, int32_t indents) const
    {
        TResult ret = T3D_OK;

        do
        {
            if (Specifiers == nullptr || Specifiers->empty())
            {
                break;
            }

            TList<Specifier> specifiers;
            for (const auto &spec : *Specifiers)
            {
                if (!StringUtil::startsWith(spec.name, "RTTR", false))
                {
                    specifiers.push_back(spec);
                }
            }

            if (specifiers.empty())
            {
                break;
            }
        
            const std::function<void(FileDataStream&, int32_t)> generateIndent = [](FileDataStream &fs, int32_t indents)
            {
                int32_t i = 0;
                for (i = 0; i < indents; i++)
                {
                    fs << "\t";
                }
            };

            fs << std::endl;
            int32_t tabs = indents;
            generateIndent(fs, tabs);
            fs << "(" << std::endl;

            {
                tabs++;

                size_t i = 0;
                for (const auto &spec : specifiers)
                {
                    generateIndent(fs, tabs);
                    if (spec.value.empty())
                    {
                        fs << "metadata(" << spec.name << ", true)";
                    }
                    else
                    {
                        fs << "metadata(" << spec.name << ", " << spec.value << ")";
                    }
                    if (i != specifiers.size() - 1)
                    {
                        fs << ",";
                    }
                    fs << std::endl;
                    i++;
                }
                
                tabs--;
            }
        
            generateIndent(fs, tabs);
            fs << ")";
        } while (false);
        
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    ASTNode *ASTNode::clone() const
    {
        ASTNode *node = new ASTNode(getName());
        cloneProperties(node);
        cloneChildren(node);
        return node;
    }

    //--------------------------------------------------------------------------

    void ASTNode::cloneProperties(ASTNode *newNode) const
    {
        newNode->Specifiers = Specifiers;
    }

    //--------------------------------------------------------------------------

    void ASTNode::cloneChildren(ASTNode *newNode) const
    {
        for (const auto &val : mChildren)
        {
            ASTNode *child = val.second->clone();
            newNode->addChild(val.first, child);
        }
    }

    //--------------------------------------------------------------------------

    void ASTNode::replaceTemplateParams(const StringArray &formals, const StringArray &actuals)
    {
        replaceChildrenTemplateParams(formals, actuals);
    }

    //--------------------------------------------------------------------------

    void ASTNode::replaceChildrenTemplateParams(const StringArray &formals, const StringArray &actuals)
    {
        for (const auto &val : mChildren)
        {
            val.second->replaceTemplateParams(formals, actuals);
        }
    }

    //--------------------------------------------------------------------------

    ASTNode *ASTNamespace::clone() const
    {
        ASTNode *node = new ASTNamespace(getName());
        cloneProperties(node);
        cloneChildren(node);
        return node;
    }

    //--------------------------------------------------------------------------

    void ASTStruct::dumpProperties(rapidjson::PrettyWriter<JsonStream>& writer) const
    {
        // Base Classes
        writer.Key("Base Classes");
        {
            writer.StartArray();
            for (const auto &val : BaseClasses)
            {
                writer.StartObject();
                writer.Key(val.second->getHierarchyName());
                writer.String(val.second->getTypeString());
                writer.EndObject();
            }
            writer.EndArray();    
        }
        
        // Specifiers
        writer.Key("Specifiers");
        {
            writer.StartArray();
            if (Specifiers != nullptr)
            {
                for (const auto &val : *Specifiers)
                {
                    writer.StartObject();
                    writer.Key(val.name);
                    writer.String(val.value);
                    writer.EndObject();
                }
            }
            writer.EndArray();
        }
        
        // File info
        writer.Key("File Info");
        {
            writer.StartObject();
            writer.Key("Path");
            writer.String(FileInfo.Path);
            writer.Key("Start Line");
            writer.Uint(FileInfo.StartLine);
            writer.Key("End Line");
            writer.Uint(FileInfo.EndLine);
            writer.EndObject();   
        }
    }

    //--------------------------------------------------------------------------

    TResult ASTStruct::generateSourceFile(FileDataStream& fs) const
    {
        // 构造类名
        String name = getHierarchyName();
            
        // 写类名
        fs << std::endl << "\tregistration::class_<" << name << ">(\"" << name << "\")";

        // 交给子结点写
        if (mChildren.empty())
        {
            generateMetaInfo(fs, 2);
            fs << ";" << std::endl;
        }
        else
        {
            fs << std::endl;
            
            size_t i = 0;
            for (const auto &child : mChildren)
            {
                if (child.second->getType() == Type::kEnum
                    || child.second->getType() == Type::kFunction
                    || child.second->getType() == Type::kProperty)
                {
                    // 类的构造函数、析构函数、函数、枚举、属性，可以直接写
                    child.second->generateSourceFile(fs);
                    if (i != mChildren.size() - 1)
                    {
                        fs << std::endl;
                    }
                }
                
                i++;
            }
            
            generateMetaInfo(fs, 2);
            
            fs << ";" << std::endl;
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    ASTNode *ASTStruct::clone() const
    {
        ASTNode *node = new ASTStruct(getName());
        cloneProperties(node);
        cloneChildren(node);
        return node;
    }

    //--------------------------------------------------------------------------

    void ASTStruct::cloneProperties(ASTNode *newNode) const
    {
        ASTNode::cloneProperties(newNode);
        
        ASTStruct *strct = static_cast<ASTStruct*>(newNode);
        strct->BaseClasses = BaseClasses;
        strct->FileInfo = FileInfo;
        strct->RTTIEnabled = RTTIEnabled;
        strct->RTTIBaseClasses = RTTIBaseClasses;
    }
    
    //--------------------------------------------------------------------------

    ASTNode *ASTClass::clone() const
    {
        ASTNode *node = new ASTClass(getName());
        cloneProperties(node);
        cloneChildren(node);
        return node;
    }
    
    //--------------------------------------------------------------------------

    TResult ASTFunction::generateSourceFile(FileDataStream &fs) const
    {
        TResult ret;
        
        if (IsProperty)
        {
           // 属性
            ret = generateSourceFileForProperty(fs);
        }
        else
        {
           // 函数
           ret = generateSourceFileForFunction(fs);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ASTFunction::generateSourceFileForFunction(FileDataStream &fs) const
    {
        bool isGlobal = (getParent()->getType() == Type::kNamespace || getParent()->getType() == Type::kNull);
        
        TResult ret = T3D_OK;

        if (isGlobal)
        {
            fs << std::endl << "\tregistration";
        }

        // 构造函数名
        String name = getHierarchyName();

        size_t idx = 0;

        bool hasOverload = (mChildren.size() > 1);
        
        for (const auto &child : mChildren)
        {
            switch (child.second->getType())
            {
            case Type::kOverloadFunction:
            case Type::kInstanceFunction:
            case Type::kStaticFunction:
                {
                    // 普通可重载函数 & 成员函数
                    if (isGlobal && idx == 0)
                    {
                        fs << "::";
                    }
                    else
                    {
                        fs << "\t\t.";
                    }
                    
                    if (hasOverload)
                    {
                        ASTOverloadFunction *overload = static_cast<ASTOverloadFunction*>(child.second);

                        // 返回值
                        const String &retType = overload->RetType;

                        // 构造参数列表
                        String params;
                        size_t i = 0;
                        for (const auto &val : overload->Params)
                        {
                            params += val.Type;
                            if (i != overload->Params.size() - 1)
                            {
                                params += ", ";
                            }
                            i++;
                        }

                        if (overload->IsConst)
                        {
                            fs << "method(\"" << getName() << "\", select_const<" << retType << "(" << params << ")>(&" << name << "))";
                        }
                        else
                        {
                            fs << "method(\"" << getName() << "\", select_overload<" << retType << "(" << params << ")>(&" << name << "))";
                        }

                        overload->generateMetaInfo(fs, 2);
                        
                        if (idx != mChildren.size() - 1)
                        {
                            fs << std::endl;
                        }
                    }
                    else
                    {
                        fs << "method(\"" << getName() << "\", &" << name << ")";
                        child.second->generateMetaInfo(fs, 2);
                    }
                }
                break;
            case Type::kConstructor:
                {
                    // 构造函数
                    ASTConstructor *constructor = static_cast<ASTConstructor*>(child.second);
                    String params;
                    size_t i = 0;
                    for (const auto &val : constructor->Params)
                    {
                        params += val.Type;
                        if (i != constructor->Params.size() - 1)
                        {
                            params += ", ";
                        }
                    }

                    if (constructor->IsNormal)
                    {
                        fs << "\t\t.constructor<" << params << ">()";
                    }
                    else
                    {
                        fs << "\t\t.constructor<" << params << ">(&" << name << ")"; 
                    }

                    constructor->generateMetaInfo(fs, 2);
                    
                    if (hasOverload && idx != mChildren.size() - 1)
                    {
                        fs << std::endl;
                    }
                }
                break;
            case Type::kDestructor:
                {
                    // 析构函数
                }
                break;
            }

            idx++;
        }

        if (isGlobal)
        {
            fs << ";" << std::endl;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ASTFunction::generateSourceFileForProperty(FileDataStream &fs) const
    {
        TResult ret = T3D_OK;

        do
        {
            bool isGlobal = (getParent()->getType() == Type::kNamespace || getParent()->getType() == Type::kNull);

            ASTOverloadFunction *setter = nullptr, *getter = nullptr;
            
            if (mChildren.size() == 1)
            {
                // 只有 getter
                const auto itr = mChildren.begin();
                ASTOverloadFunction *overload = static_cast<ASTOverloadFunction*>(itr->second);
                if (overload->IsConst)
                {
                    // getter
                    getter = overload;
                }
                else
                {
                    // 错误了，只有一个属性函数的时候，只能是 getter 函数
                }
            }
            else if (mChildren.size() == 2)
            {
                // 有 getter 和 setter
                for (const auto &child : mChildren)
                {
                    ASTOverloadFunction *overload = static_cast<ASTOverloadFunction *>(child.second);
                    if (overload->IsConst)
                    {
                        getter = overload;
                    }
                    else
                    {
                        setter = overload;
                    }
                }
            }
            else
            {
                // 错误了
                ret = T3D_ERR_RP_INVALID_NUM_PROPERTY_FUNC;
                break;
            }

            if (isGlobal)
            {
                fs << std::endl << "\tregistration::";
            }
            else
            {
                fs << "\t\t.";
            }

            if (getter != nullptr && setter == nullptr)
            {
                fs << "property_readonly(\"" << getName() << "\", &" << getter->getPropertyFunctionName() << ")";
            }
            else
            {
                fs << "property(\"" << getName() << "\", &" << getter->getPropertyFunctionName() << ", &" << setter->getPropertyFunctionName() << ")"; 
            }

            getter->generateMetaInfo(fs, 2);
            
            if (isGlobal)
            {
                fs << ";" << std::endl;
            }
        } while (false);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    ASTNode *ASTFunction::clone() const
    {
        ASTNode *node = new ASTFunction(getName());
        cloneProperties(node);
        cloneChildren(node);
        return node;
    }

    //--------------------------------------------------------------------------

    void ASTFunction::cloneProperties(ASTNode *newNode) const
    {
        ASTNode::cloneProperties(newNode);
        
        ASTFunction *function = static_cast<ASTFunction*>(newNode);
        function->IsProperty = IsProperty;
        function->IsGetter = IsGetter;
        function->FileInfo = FileInfo;
    }

    //--------------------------------------------------------------------------

    void ASTSpecifierFunction::dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const
    {
        // Specifiers
        writer.Key("Specifiers");
        {
            writer.StartArray();
            if (Specifiers != nullptr)
            {
                for (const auto &val : *Specifiers)
                {
                    writer.StartObject();
                    writer.Key(val.name);
                    writer.String(val.value);
                    writer.EndObject();
                }
            }
            writer.EndArray();            
        }
    }

    //--------------------------------------------------------------------------

    void ASTParameterFunction::dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const
    {
        // Parameters
        writer.Key("Parameters");
        {
            writer.StartArray();
            for (const auto &val : Params)
            {
                writer.StartObject();
                writer.Key("Name");
                writer.String(val.Name);
                writer.Key("Type");
                writer.String(val.Type);
                writer.EndObject();
            }
            writer.EndArray();
        }
        
        ASTSpecifierFunction::dumpProperties(writer);
    }

    //--------------------------------------------------------------------------

    void ASTParameterFunction::cloneProperties(ASTNode *newNode) const
    {
        ASTSpecifierFunction::cloneProperties(newNode);
        
        ASTParameterFunction *function = static_cast<ASTParameterFunction*>(newNode);
        function->Params = Params;
    }

    //--------------------------------------------------------------------------

    void ASTOverloadFunction::dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const
    {
        // Is constant function
        writer.Key("Is Constant");
        writer.Bool(IsConst);

        // Is property function
        writer.Key("Is Property");
        writer.Bool(IsProperty);

        // Is getter
        writer.Key("Is Getter");
        writer.Bool(IsGetter);
        
        // File Info
        writer.Key("File Info");
        {
            writer.StartObject();
            writer.Key("Path");
            writer.String(FileInfo.Path);
            writer.Key("Start Line");
            writer.Uint(FileInfo.StartLine);
            writer.Key("End Line");
            writer.Uint(FileInfo.EndLine);
            writer.EndObject();   
        }
        
        // Return Type
        writer.Key("Return Type");
        writer.String(RetType);
        
        ASTParameterFunction::dumpProperties(writer);
    }

    //--------------------------------------------------------------------------

    String ASTOverloadFunction::getPropertyFunctionName() const
    {
        String name = getName();
        
        const ASTNode *node = getParent();

        // 跳开父结点，因为这个不是函数名字
        if (node != nullptr)
        {
            node = node->getParent();
        }
        
        while (node != nullptr)
        {
            if (node->getType() != Type::kNull)
            {
                name = node->getName() + "::" + name;    
            }
            node = node->getParent();
        }
        
        return name;
    }

    //--------------------------------------------------------------------------

    ASTNode *ASTOverloadFunction::clone() const
    {
        ASTNode *node = new ASTOverloadFunction(getName());
        cloneProperties(node);
        cloneChildren(node);
        return node;
    }

    //--------------------------------------------------------------------------

    void ASTOverloadFunction::cloneProperties(ASTNode *newNode) const
    {
        ASTParameterFunction::cloneProperties(newNode);
        
        ASTOverloadFunction *function = static_cast<ASTOverloadFunction *>(newNode);
        function->RetType = RetType;
        function->IsConst = IsConst;
    }

    //--------------------------------------------------------------------------

    void ASTOverloadFunction::replaceTemplateParams(const StringArray &formals, const StringArray &actuals)
    {
        // size_t i = 0;
        // for (i = 0; i < formals.size(); i++)
        // {
        //     const String &formal = formals[i];
        //     if (!formal.empty())
        //     {
        //         // 查参数
        //         for (auto &param : Params)
        //         {
        //             if (param.Type == formal)
        //             {
        //                 param.Type = actuals[i];
        //             }
        //         }
        //
        //         // 查返回值
        //         if (RetType == formal)
        //         {
        //             RetType = actuals[i];
        //         }
        //     }
        // }

        size_t i = 0;
        for (const auto &arg : formals)
        {
            if (!arg.empty())
            {
                String pattern = "* " + arg + " *";
                String formal = " " + arg + " ";
                String actual = " " + actuals[i] + " ";
                
                // 查找参数里面的模板类型并替换
                for (auto &param : Params)
                {
                    if (StringUtil::match(param.Type, pattern, true))
                    {
                        StringUtil::replaceAll(param.Type, formal, actual);
                    }
                }

                // 查找返回值里面的模板类型并替换
                if (StringUtil::match(RetType, pattern, true))
                {
                    StringUtil::replaceAll(RetType, formal, actual);
                }
            }
            
            i++;
        }

        replaceChildrenTemplateParams(formals, actuals);
    }

    //--------------------------------------------------------------------------

    ASTNode *ASTStaticFunction::clone() const
    {
        ASTNode *node = new ASTStaticFunction(getName());
        cloneProperties(node);
        cloneChildren(node);
        return node;
    }

    //--------------------------------------------------------------------------

    ASTNode *ASTInstanceFunction::clone() const
    {
        ASTNode *node = new ASTInstanceFunction(getName());
        cloneProperties(node);
        cloneChildren(node);
        return node;
    }

    //--------------------------------------------------------------------------

    void ASTConstructor::dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const
    {
        ASTOverloadFunction::dumpProperties(writer);
        
        // Is normal constructor
        writer.Key("IsNormal");
        writer.Bool(IsNormal);
    }

    //--------------------------------------------------------------------------

    ASTNode *ASTConstructor::clone() const
    {
        ASTNode *node = new ASTConstructor(getName());
        cloneProperties(node);
        cloneChildren(node);
        return node;
    }

    //--------------------------------------------------------------------------

    ASTNode *ASTDestructor::clone() const
    {
        ASTNode *node = new ASTDestructor(getName());
        cloneProperties(node);
        cloneChildren(node);
        return node;
    }

    //--------------------------------------------------------------------------

    
    //--------------------------------------------------------------------------

    void ASTProperty::dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const
    {
        // Type
        writer.Key("Data Type");
        writer.String(DataType);

        // Specifiers
        writer.Key("Specifiers");
        {
            writer.StartArray();
            if (Specifiers != nullptr)
            {
                for (const auto &val : *Specifiers)
                {
                    writer.StartObject();
                    writer.Key(val.name);
                    writer.String(val.value);
                    writer.EndObject();
                }
            }
            writer.EndArray();            
        }
    }

    //--------------------------------------------------------------------------

    TResult ASTProperty::generateSourceFile(FileDataStream &fs) const
    {
        bool isGlobal = (getParent()->getType() == Type::kNamespace || getParent()->getType() == Type::kNull);

        String name = getHierarchyName();
        
        if (isGlobal)
        {   
            fs << std::endl << "\tregistration::property(\"" << name << "\", &" << name << ");" << std::endl;
        }
        else
        {
            fs << "\t\t.property(\"" << getName() << "\", &" << name << ")";
        }

        generateMetaInfo(fs, 2);
        // fs << "property(\"" << name << "\", &" << name << ")";
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    ASTNode *ASTProperty::clone() const
    {
        ASTNode *node = new ASTProperty(getName());
        cloneProperties(node);
        cloneChildren(node);
        return node;
    }

    //--------------------------------------------------------------------------

    void ASTProperty::cloneProperties(ASTNode *newNode) const
    {
        ASTNode::cloneProperties(newNode);
        
        ASTProperty *property = static_cast<ASTProperty *>(newNode);
        property->DataType = DataType;
        property->FileInfo = FileInfo;
    }

    //--------------------------------------------------------------------------

    void ASTProperty::replaceTemplateParams(const StringArray &formals, const StringArray &actuals)
    {
        for (size_t i = 0; i < formals.size(); i++)
        {
            const String &formal = formals[i];
        
            if (!formal.empty() && formal == DataType)
            {
                DataType = actuals[i];
            }
        }
        
        replaceChildrenTemplateParams(formals, actuals);
    }

    //--------------------------------------------------------------------------

    TResult ASTEnum::generateSourceFile(FileDataStream &fs) const
    {
        bool isGlobal = (getParent()->getType() == Type::kNamespace || getParent()->getType() == Type::kNull);
        
        // 枚举名称
        String name = getHierarchyName();

        if (isGlobal)
        {
            fs << std::endl << "\tregistration::enumeration<" << name << ">(\"" << name << "\")" << std::endl;
        }
        else
        {
            fs << "\t\t.enumeration<" << name << ">(\"" << name << "\")" << std::endl;
        }

        fs << "\t\t(" << std::endl;

        {
            size_t i = 0;
            for (const auto &child : mChildren)
            {
                ASTEnumConstant *constant = static_cast<ASTEnumConstant*>(child.second);

                fs << "\t\t\tvalue(\"" << child.second->getName() << "\", " << child.second->getHierarchyName() << ")";

                if (i != mChildren.size() - 1)
                {
                    fs << ",";
                }
                
                fs << std::endl;

                i++;
            }
        }

        fs << "\t\t)";

        generateMetaInfo(fs, 2);
        
        if (isGlobal)
        {
            fs << ";" << std::endl;
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    ASTNode *ASTEnum::clone() const
    {
        ASTNode *node = new ASTEnum(getName());
        cloneProperties(node);
        cloneChildren(node);
        return node;
    }

    //--------------------------------------------------------------------------

    void ASTEnum::cloneProperties(ASTNode *newNode) const
    {
        ASTNode::cloneProperties(newNode);
        
        ASTEnum *enumerate = static_cast<ASTEnum *>(newNode);
        enumerate->FileInfo = FileInfo;
    }

    //--------------------------------------------------------------------------

    void ASTEnumConstant::dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const
    {
        // Value
        writer.Key("Value");
        writer.Uint64(Value);
    }

    //--------------------------------------------------------------------------

    ASTNode *ASTEnumConstant::clone() const
    {
        ASTNode *node = new ASTEnumConstant(getName());
        cloneProperties(node);
        cloneChildren(node);
        return node;
    }

    //--------------------------------------------------------------------------

    void ASTEnumConstant::cloneProperties(ASTNode *newNode) const
    {
        ASTNode::cloneProperties(newNode);

        ASTEnumConstant *constant = static_cast<ASTEnumConstant *>(newNode);
        constant->Value = Value;
    }

    //--------------------------------------------------------------------------

    void ASTTemplate::instantiateTemplate(const StringArray &actuals)
    {
        if (actuals.size() != TemplateParams.size())
            return;
        
        size_t i = 0;
        for (auto &param : TemplateParams)
        {
            if (!actuals[i].empty())
            {
                param.type = actuals[i];
                param.kind = ASTTemplateParam::Kind::kNonType;
            }
            
            i++;
        }

        IsSpecialization = true;
    }

    //--------------------------------------------------------------------------

    void ASTTemplate::cloneTemplateProperties(ASTTemplate *newTemplate) const
    {
        newTemplate->TemplateParams = TemplateParams;
        newTemplate->IsSpecialization = IsSpecialization;
    }

    //--------------------------------------------------------------------------

    void ASTTemplate::dumpTemplateProperties(rapidjson::PrettyWriter<JsonStream> &writer) const
    {
        const std::function<String(ASTTemplateParam::Kind)> getParamTypeString = [](ASTTemplateParam::Kind kind)
        {
            switch (kind)
            {
            case ASTTemplateParam::Kind::kTemplateType:
                return "Template Type";
            case ASTTemplateParam::Kind::kNonType:
                return "Non Type";
            case ASTTemplateParam::Kind::kTemplateTemplate:
                return "Template Template";
            default:
                return "None";
            }
        };
        
        // Template parameters
        writer.Key("Template Parameters");
        {
            writer.StartArray();

            for (const auto &param : TemplateParams)
            {
                writer.StartObject();
                writer.Key("Type");
                writer.String(param.type);
                writer.Key("Name");
                writer.String(param.name);
                writer.Key("Kind");
                writer.String(getParamTypeString(param.kind));
                writer.EndObject();
            }
            
            writer.EndArray();            
        }

        // Specialization
        writer.Key("Specialization");
        writer.Bool(IsSpecialization);
    }

    //--------------------------------------------------------------------------

    void ASTClassTemplate::dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const
    {
        ASTStruct::dumpProperties(writer);
        dumpTemplateProperties(writer);
    }

    //--------------------------------------------------------------------------

    ASTNode *ASTClassTemplate::clone() const
    {
        ASTNode *node = new ASTClassTemplate(getName());
        cloneProperties(node);
        cloneChildren(node);
        return node;
    }

    //--------------------------------------------------------------------------

    void ASTClassTemplate::cloneProperties(ASTNode *newNode) const
    {
        ASTStruct::cloneProperties(newNode);
        ASTClassTemplate *klass = static_cast<ASTClassTemplate*>(newNode);
        cloneTemplateProperties(klass);
    }

    //--------------------------------------------------------------------------

    void ASTClassTemplate::replaceTemplateParams(const StringArray &formals, const StringArray &actuals)
    {
        instantiateTemplate(actuals);
        replaceChildrenTemplateParams(formals, actuals);
    }
    
    //--------------------------------------------------------------------------

    void ASTFunctionTemplate::dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const
    {
        ASTFunction::dumpProperties(writer);

        // Specialization
        writer.Key("Specialization");
        writer.Bool(HasSpecialization);
    }

    //--------------------------------------------------------------------------

    TResult ASTFunctionTemplate::generateSourceFile(FileDataStream &fs) const
    {
        TResult ret = T3D_OK;

        if (HasSpecialization)
        {
            if (IsProperty)
            {
                // 属性
                ret = generateSourceFileForProperty(fs);
            }
            else
            {
                // 函数
                ret = generateSourceFileForFunction(fs);
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ASTFunctionTemplate::generateSourceFileForFunction(FileDataStream &fs) const
    {
        bool isGlobal = (getParent()->getType() == Type::kNamespace || getParent()->getType() == Type::kNull);
        
        TResult ret = T3D_OK;

        if (isGlobal)
        {
            fs << std::endl << "\tregistration";
        }

        // 构造函数名
        String name = getHierarchyName();

        size_t idx = 0;

        bool hasOverload = (mChildren.size() > 1);
        
        for (const auto &child : mChildren)
        {
            switch (child.second->getType())
            {
            case Type::kOverloadFunction:
            case Type::kInstanceFunction:
            case Type::kStaticFunction:
                {
                    // 普通可重载函数 & 成员函数
                    if (isGlobal && idx == 0)
                    {
                        fs << "::";
                    }
                    else
                    {
                        fs << "\t\t.";
                    }
                    
                    if (hasOverload)
                    {
                        ASTOverloadFunction *overload = static_cast<ASTOverloadFunction*>(child.second);

                        // 返回值
                        const String &retType = overload->RetType;

                        // 构造参数列表
                        String params;
                        size_t i = 0;
                        for (const auto &val : overload->Params)
                        {
                            params += val.Type;
                            if (i != overload->Params.size() - 1)
                            {
                                params += ", ";
                            }
                            i++;
                        }

                        if (overload->IsConst)
                        {
                            fs << "method(\"" << getName() << "\", select_const<" << retType << "(" << params << ")>(&" << name << "))";
                        }
                        else
                        {
                            fs << "method(\"" << getName() << "\", select_overload<" << retType << "(" << params << ")>(&" << name << "))";
                        }

                        overload->generateMetaInfo(fs, 2);
                        
                        if (idx != mChildren.size() - 1)
                        {
                            fs << std::endl;
                        }
                    }
                    else
                    {
                        fs << "method(\"" << getName() << "\", &" << name << ")";
                        child.second->generateMetaInfo(fs, 2);
                    }
                }
                break;
            case Type::kConstructor:
                {
                    // 构造函数
                    ASTConstructor *constructor = static_cast<ASTConstructor*>(child.second);
                    String params;
                    size_t i = 0;
                    for (const auto &val : constructor->Params)
                    {
                        params += val.Type;
                        if (i != constructor->Params.size() - 1)
                        {
                            params += ", ";
                        }
                    }
                    fs << "\t\t.constructor<" << params << ">()";

                    constructor->generateMetaInfo(fs, 2);
                    
                    if (hasOverload && idx != mChildren.size() - 1)
                    {
                        fs << std::endl;
                    }
                }
                break;
            case Type::kDestructor:
                {
                    // 析构函数
                }
                break;
            }

            idx++;
        }

        if (isGlobal)
        {
            fs << ";" << std::endl;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ASTFunctionTemplate::generateSourceFileForProperty(FileDataStream &fs) const
    {
        TResult ret = T3D_OK;

        do
        {
            bool isGlobal = (getParent()->getType() == Type::kNamespace || getParent()->getType() == Type::kNull);

            ASTOverloadFunction *setter = nullptr, *getter = nullptr;
            
            if (mChildren.size() == 1)
            {
                // 只有 getter
                const auto itr = mChildren.begin();
                ASTOverloadFunction *overload = static_cast<ASTOverloadFunction*>(itr->second);
                if (overload->IsConst)
                {
                    // getter
                    getter = overload;
                }
                else
                {
                    // 错误了，只有一个属性函数的时候，只能是 getter 函数
                }
            }
            else if (mChildren.size() == 2)
            {
                // 有 getter 和 setter
                for (const auto &child : mChildren)
                {
                    ASTOverloadFunction *overload = static_cast<ASTOverloadFunction *>(child.second);
                    if (overload->IsConst)
                    {
                        getter = overload;
                    }
                    else
                    {
                        setter = overload;
                    }
                }
            }
            else
            {
                // 错误了
                ret = T3D_ERR_RP_INVALID_NUM_PROPERTY_FUNC;
                break;
            }

            if (isGlobal)
            {
                fs << std::endl << "\tregistration::";
            }
            else
            {
                fs << "\t\t.";
            }

            if (getter != nullptr && setter == nullptr)
            {
                fs << "property_readonly(\"" << getName() << "\", &" << getter->getPropertyFunctionName() << ")";
            }
            else
            {
                fs << "property(\"" << getName() << "\", &" << getter->getPropertyFunctionName() << ", &" << setter->getPropertyFunctionName() << ")"; 
            }

            getter->generateMetaInfo(fs, 2);
            
            if (isGlobal)
            {
                fs << ";" << std::endl;
            }
        } while (false);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    ASTNode *ASTFunctionTemplate::clone() const
    {
        ASTNode *node = new ASTFunctionTemplate(getName());
        cloneProperties(node);
        cloneChildren(node);
        return node;
    }

    //--------------------------------------------------------------------------

    void ASTFunctionTemplate::cloneProperties(ASTNode *newNode) const
    {
        ASTFunction::cloneProperties(newNode);
        ASTFunctionTemplate *function = static_cast<ASTFunctionTemplate*>(newNode);
        function->HasSpecialization = HasSpecialization;
    }
    
    //--------------------------------------------------------------------------

    // void ASTOverloadFunctionTemplate::dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const
    // {
    //     ASTOverloadFunction::dumpProperties(writer);
    //     dumpTemplateProperties(writer);
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // ASTNode *ASTOverloadFunctionTemplate::clone() const
    // {
    //     ASTNode *node = new ASTOverloadFunctionTemplate(getName());
    //     cloneProperties(node);
    //     cloneChildren(node);
    //     return node;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void ASTOverloadFunctionTemplate::cloneProperties(ASTNode *newNode) const
    // {
    //     ASTOverloadFunction::cloneProperties(newNode);
    //     ASTOverloadFunctionTemplate *function = static_cast<ASTOverloadFunctionTemplate *>(newNode);
    //     cloneTemplateProperties(function);
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void ASTOverloadFunctionTemplate::replaceTemplateParams(const StringArray &formals, const StringArray &actuals)
    // {
    //     instantiateTemplate(actuals);
    //     ASTOverloadFunction::replaceTemplateParams(formals, actuals);
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void ASTStaticFunctionTemplate::dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const
    // {
    //     ASTStaticFunction::dumpProperties(writer);
    //     dumpTemplateProperties(writer);
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // ASTNode *ASTStaticFunctionTemplate::clone() const
    // {
    //     ASTNode *node = new ASTStaticFunctionTemplate(getName());
    //     cloneProperties(node);
    //     cloneChildren(node);
    //     return node;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void ASTStaticFunctionTemplate::cloneProperties(ASTNode *newNode) const
    // {
    //     ASTStaticFunction::cloneProperties(newNode);
    //     ASTStaticFunctionTemplate *function = static_cast<ASTStaticFunctionTemplate *>(newNode);
    //     cloneTemplateProperties(function);
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void ASTInstanceFunctionTemplate::dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const
    // {
    //     ASTInstanceFunction::dumpProperties(writer);
    //     dumpTemplateProperties(writer);
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // ASTNode *ASTInstanceFunctionTemplate::clone() const
    // {
    //     ASTNode *node = new ASTInstanceFunctionTemplate(getName());
    //     cloneProperties(node);
    //     cloneChildren(node);
    //     return node;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void ASTInstanceFunctionTemplate::cloneProperties(ASTNode *newNode) const
    // {
    //     ASTInstanceFunction::cloneProperties(newNode);
    //     ASTInstanceFunctionTemplate *function = static_cast<ASTInstanceFunctionTemplate *>(newNode);
    //     cloneTemplateProperties(function);
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void ASTConstructorTemplate::dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const
    // {
    //     ASTConstructor::dumpProperties(writer);
    //     dumpTemplateProperties(writer);
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // ASTNode *ASTConstructorTemplate::clone() const
    // {
    //     ASTNode *node = new ASTConstructorTemplate(getName());
    //     cloneProperties(node);
    //     cloneChildren(node);
    //     return node;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void ASTConstructorTemplate::cloneProperties(ASTNode *newNode) const
    // {
    //     ASTConstructor::cloneProperties(newNode);
    //     ASTConstructorTemplate *function = static_cast<ASTConstructorTemplate *>(newNode);
    //     cloneTemplateProperties(function);
    // }

    //--------------------------------------------------------------------------
}
