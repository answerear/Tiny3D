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

#ifndef __T3D_ABSTRACT_SYNTAX_TREE_H__
#define __T3D_ABSTRACT_SYNTAX_TREE_H__


#include "T3DRPPrerequisites.h"


namespace Tiny3D
{
    const String kMacroClass = "TCLASS";
    const String kMacroFunction = "TFUNCTION";
    const String kMacroProperty = "TPROPERTY";
    const String kMacroStruct = "TSTRUCT";
    const String kMacroEnum = "TENUM";
    const String kRTTIEnable = "TRTTI_ENABLE";
    const String kRTTIFriend = "TRTTI_FRIEND";

    /// 分类名
    const String kSpecName = "RTTRFuncName";
    const String kSpecPropertyType = "RTTRFuncType";
    const String kSpecPropertyGetter = "\"getter\"";
    const String kSpecPropertySetter = "\"setter\"";
    const String kSpecAsConstructor = "RTTRAsConstructor";
    const String kSpecConstructAsPointer = "RTTRConstructAsPointer";
    const String kSpecConstructAsObject = "RTTRConstructAsObject";

    struct Specifier
    {
        String  name;
        String  value;
    };

    typedef TMap<uint32_t, TList<Specifier>> Specifiers;
    typedef Specifiers::iterator SpecifiersItr;
    typedef Specifiers::const_iterator SpecifiersConstItr;
    typedef Specifiers::value_type SpecifiersValue;

    typedef TMap<String, class ASTNode*> ASTChildren;
    typedef ASTChildren::value_type ASTChildrenValue;

    struct ASTNodeInfo
    {
        String name;
        CXCursorKind cxKind;
    };

    /**
     * @brief AST 语法树结点
     */
    class ASTNode : public Noncopyable
    {
    public:
        /** 结点类型 */
        enum class Type : uint32_t
        {
            kNone = 0,
            kNull,                      /// 空结点，表示什么都不是
            kNamespace,                 /// 命名空间
            kClass,                     /// 类
            kStruct,                    /// 结构体
            kClassTemplate,             /// 类模板
            kFunction,                  /// 函数
            kOverloadFunction,          /// 可重载函数
            kStaticFunction,            /// 静态函数
            kInstanceFunction,          /// 成员函数
            kConstructor,               /// 构造函数
            kDestructor,                /// 析构函数
            kFunctionTemplate,          /// 函数模板
            // kOverloadFunctionTemplate,  /// 可重载函数模板
            // kStaticFunctionTemplate,    /// 静态函数模板
            // kInstanceFunctionTemplate,  /// 成员函数模板
            // kConstructorTemplate,       /// 构造函数模板
            kProperty,                  /// 属性
            kEnum,                      /// 枚举
            kEnumConstant,              /// 枚举值
        };

        /** Constructor */
        ASTNode(const String &name);

        /** Destructor */
        virtual ~ASTNode() override;
        
        /** 获取结点类型 */
        virtual Type getType() const
        {
            return Type::kNull;
        }

        /** 获取结点类型字符串描述 */
        virtual String getTypeString() const
        {
            return "Default";
        }

        /** 获取结点名称 */
        const String &getName() const
        {
            return mName;
        }

        /**
         * @brief 添加子结点
         * @param [in] name : 结点名称
         * @param [in] child : 子结点对象
         */
        void addChild(const String &name, ASTNode *child)
        {
            child->mParent = this;
            mChildren[name] = child;
        }

        /**
         * @brief 移除子结点
         * @param [in] name : 结点名称
         */
        void removeChild(const String &name)
        {
            const auto itr = mChildren.find(name);
            if (itr != mChildren.end())
            {
                ASTNode *child = itr->second;
                delete child;
                mChildren.erase(itr);
            }
        }

        /**
         * @brief 移除所有子结点
         */
        void removeAllChildren();

        /**
         * @brief 返回当前结点父结点
         * @return 父结点对象
         */
        ASTNode *getParent() const
        {
            return mParent;
        }

        /**
         * @brief 返回所有子结点
         */
        const ASTChildren &getChildren() const
        {
            return mChildren;
        }

        /**
         * @brief 根据名称获取子结点。
         * @param [in] name : 子结点名称。 可以用 :: 隔开，递归查找子树，如：Namespace::Class::Member 作为查找名称
         * @param [in] recursive : 是否递归找子树
         * @return 返回指定名称的子结点，没有对应子结点则返回 nullptr
         */
        ASTNode *getChild(const String &name, bool recursive = false) const
        {
            if (!recursive)
                return getChildDirectly(name);
            return getChildRecursively(name);
        }

        /**
         * @brief 根据名称列表获取叶子结点
         * @param [in] names : 各层级子结点名称列表
         * @return 返回指定名称列表下的叶子结点，没有对应的叶子结点返回 nullptr
         */
        ASTNode *getChild(const StringList &names) const
        {
            return getChildRecursively(names);
        }

        ASTNode *findChild(const String &name) const;

        /**
         * @brief 获取层级结构名称
         * @return 返回以 :: 分割的当前结点各级父结点组成的名称
         */
        String getHierarchyName() const;

        /**
         * @brief 输出 AST 到 json 文件
         * @param [in] writer : json 写对象
         */
        void dump(rapidjson::PrettyWriter<JsonStream> &writer) const;

        /**
         * @brief 生成源码文件
         */
        virtual TResult generateSourceFile(FileDataStream &fs) const;

        /**
         * @brief 生成元信息
         */
        virtual TResult generateMetaInfo(FileDataStream &fs, int32_t indents) const;

        /**
         * @brief 克隆一个新对象，包括复制所有子树
         */
        virtual ASTNode *clone() const;

        /**
         * @brief 复制属性
         */
        virtual void cloneProperties(ASTNode *newNode) const;
        
        /**
         * @brief 克隆所有子树
         */
        virtual void cloneChildren(ASTNode *newNode) const;

        /**
         * @brief 替换模板中的形参为实参
         * @param [in] formals : 形参
         * @param [in] actuals : 实参
         */
        virtual void replaceTemplateParams(const StringArray &formals, const StringArray &actuals);

        TList<Specifier>    *Specifiers;        /// 反射属性说明符

    protected:
        ASTNode *getChildDirectly(const String &name) const
        {
            if (mChildren.find(name) == mChildren.end())
                return nullptr;
            return mChildren.at(name);
        }

        ASTNode *getChildRecursively(const StringList &names) const;

        ASTNode *getChildRecursively(const String &name) const
        {
            StringList names = StringUtil::split2(name, "::");
            return getChildRecursively(names);
        }

        /**
         * @brief 用实参替换子结点的形参
         */
        virtual void replaceChildrenTemplateParams(const StringArray &formals, const StringArray &actuals);

        virtual void dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const;

        String          mName;          /// 结点名称
        ASTNode         *mParent;       /// 父结点
        ASTChildren     mChildren;      /// 子结点
    };

    /**
     * @brief 文件信息
     */
    class ASTFileInfo
    {
    public:
        ASTFileInfo()
            : StartLine(-1)
            , EndLine(-1)
        {}

        String      Path;       /// 文件路径
        uint32_t    StartLine;  /// 所在文件的起始行号
        uint32_t    EndLine;    /// 所在文件的结束行号
    };

    typedef std::shared_ptr<ASTFileInfo> ASTFileInfoPtr;

    /**
     * @brief 命名空间结点
     */
    class ASTNamespace : public ASTNode
    {
    public:
        ASTNamespace(const String &name)
            : ASTNode(name)
        {}

        virtual Type getType() const override
        {
            return Type::kNamespace;
        }

        virtual String getTypeString() const override
        {
            return "Namespace";
        }

        virtual ASTNode *clone() const override;
    };

    class ASTStruct;
    typedef TMap<String, ASTStruct*> ASTBaseClasses;
    typedef ASTBaseClasses::value_type ASTBaseClassesValue;
    
    /**
     * @brief 结构体结点
     */
    class ASTStruct : public ASTNode
    {
    public:
        ASTStruct(const String &name)
            : ASTNode(name)
            , RTTIEnabled(false)
            , ConstructAsPointer(false)
            , HasDefaultConstructor(false)
            , HasConstructor(false)
            , RTTIBaseClasses(nullptr)
        {}

        virtual Type getType() const override
        {
            return Type::kStruct;
        }

        virtual String getTypeString() const override
        {
            return "Struct";
        }

        virtual TResult generateSourceFile(FileDataStream& fs) const override;

        virtual ASTNode *clone() const override;

        virtual void cloneProperties(ASTNode *newNode) const override;

        /**
         * @brief 是否指定类名的派生类
         */
        bool isDerivedOf(const String &name) const;
        
    protected:
        virtual void dumpProperties(rapidjson::PrettyWriter<JsonStream>& writer) const override;

    public:
        ASTBaseClasses      BaseClasses;            /// 基类列表
        ASTFileInfo         FileInfo;               /// 结构体所在文件信息
        bool                RTTIEnabled;            /// 是否开启了反射功能
        bool                ConstructAsPointer;     /// 构造返回指针或者普通对象
        bool                HasDefaultConstructor;  /// 是否有默认构造函数
        bool                HasConstructor;         /// 是否有构造函数
        TList<String>       *RTTIBaseClasses;       /// 反射的基类列表
    };

    /**
     * @brief 类结点
     */
    class ASTClass : public ASTStruct
    {
    public:
        ASTClass(const String &name)
            : ASTStruct(name)
        {}
        
        virtual Type getType() const override
        {
            return Type::kClass;
        }

        virtual String getTypeString() const override
        {
            return "Class";
        }

        virtual ASTNode *clone() const override;
    };
    
    /**
     * @brief 函数结点
     */
    class ASTFunction : public ASTNode
    {
    public:
        ASTFunction(const String &name)
            : ASTNode(name)
            , IsProperty(false)
            , IsGetter(false)
        {}

        virtual Type getType() const override
        {
            return Type::kFunction;
        }

        virtual String getTypeString() const override
        {
            return "Function";
        }

        virtual TResult generateSourceFile(FileDataStream &fs) const override;

        virtual ASTNode *clone() const override;

        virtual void cloneProperties(ASTNode *newNode) const override;
        
    protected:
        virtual TResult generateSourceFileForProperty(FileDataStream &fs) const;

        virtual TResult generateSourceFileForFunction(FileDataStream &fs) const;

    public:
        bool                IsProperty;     /// 是否属性函数
        bool                IsGetter;       /// 是否 Getter 属性函数，当 IsProperty 为 false 时，该字段无效
        ASTFileInfo         FileInfo;       /// 函数所在文件信息
    };
    
    /**
     * @brief 函数参数
     */
    struct ASTFunctionParam
    {
        String Type;        /// 参数类型
        String Name;        /// 参数名称
        String Default;     /// 默认参数
    };

    #define AST_NO_IMPLEMENTATION(x) { T3D_ASSERT(0, "No implementation !"); return x; }
    #define AST_NODE_NOT_INSTANTIATE() \
        virtual Type getType() const override   \
        {   \
            AST_NO_IMPLEMENTATION(Type::kNone); \
        }   \
        virtual String getTypeString() const override   \
        {   \
            AST_NO_IMPLEMENTATION("None");  \
        }
    
    /**
     * @brief 可带说明符的函数
     */
    class ASTSpecifierFunction : public ASTFunction
    {
    public:
        ASTSpecifierFunction(const String &name)
            : ASTFunction(name)
        {}

        AST_NODE_NOT_INSTANTIATE();
        
    protected:
        virtual void dumpProperties(rapidjson::PrettyWriter<JsonStream>& writer) const override;
    };

    /**
     * @brief 有参数的函数：普通函数、构造函数都是这类
     */
    class ASTParameterFunction : public ASTSpecifierFunction
    {
    public:
        ASTParameterFunction(const String &name)
            : ASTSpecifierFunction(name)
        {}

        AST_NODE_NOT_INSTANTIATE();

        void cloneProperties(ASTNode *newNode) const override;
        
    protected:
        virtual void dumpProperties(rapidjson::PrettyWriter<JsonStream>& writer) const override;
        
    public:
        TList<ASTFunctionParam> Params;         /// 函数参数列表
    };

    /**
     * @brief 可重载的函数结点
     */
    class ASTOverloadFunction : public ASTParameterFunction
    {
    public:
        ASTOverloadFunction(const String &name)
            : ASTParameterFunction(name)
            , IsConst(false)
        {}

        virtual Type getType() const override
        {
            return Type::kOverloadFunction;
        }

        virtual String getTypeString() const override
        {
            return "Overload Function";
        }

        String getPropertyFunctionName() const;

        virtual ASTNode *clone() const override;

        virtual void cloneProperties(ASTNode *newNode) const override;

        virtual void replaceTemplateParams(const StringArray &formals, const StringArray &actuals) override;
        
    protected:
        virtual void dumpProperties(rapidjson::PrettyWriter<JsonStream>& writer) const override;

    public:
        String  RetType;        /// 函数返回值类型
        bool    IsConst;        /// 是否常量函数
    };

    /**
     * @brief 静态函数结点
     */
    class ASTStaticFunction : public ASTOverloadFunction
    {
    public:
        ASTStaticFunction(const String &name)
            : ASTOverloadFunction(name)
        {}

        virtual Type getType() const override
        {
            return Type::kStaticFunction;
        }

        virtual String getTypeString() const override
        {
            return "Static Function";
        }

        virtual ASTNode *clone() const override;
    };
    
    /**
     * @brief 非静态函数结点。 可以是全局函数，也可以是类实例成员函数
     */
    class ASTInstanceFunction : public ASTOverloadFunction
    {
    public:
        ASTInstanceFunction(const String &name)
            : ASTOverloadFunction(name)
        {}

        virtual Type getType() const override
        {
            return Type::kInstanceFunction;
        }

        virtual String getTypeString() const override
        {
            return "Instance Function";
        }

        virtual ASTNode *clone() const override;
    };

    /**
     * @brief 构造函数结点
     */
    class ASTConstructor : public ASTOverloadFunction
    {
    public:
        ASTConstructor(const String &name)
            : ASTOverloadFunction(name)
            , IsNormal(true)
        {}

        virtual Type getType() const override
        {
            return Type::kConstructor;
        }

        virtual String getTypeString() const override
        {
            return "Constructor";
        }

        virtual ASTNode *clone() const override;

        bool IsNormal;  /// 非标准构造函数（即一般函数）作为其构造函数

    protected:
        virtual void dumpProperties(rapidjson::PrettyWriter<JsonStream>& writer) const override;
    };

    /**
     * @brief 析构函数结点
     */
    class ASTDestructor : public ASTOverloadFunction
    {
    public:
        ASTDestructor(const String &name)
            : ASTOverloadFunction(name)
        {}

        virtual Type getType() const override
        {
            return Type::kDestructor;
        }

        virtual String getTypeString() const override
        {
            return "Destructor";
        }

        virtual ASTNode *clone() const override;
    };

    /**
     * @brief 属性结点
     */
    class ASTProperty : public ASTNode
    {
    public:
        ASTProperty(const String &name)
            : ASTNode(name)
        {}

        virtual Type getType() const override
        {
            return Type::kProperty;
        }

        virtual String getTypeString() const override
        {
            return "Property";
        }

        virtual TResult generateSourceFile(FileDataStream &fs) const override;

        virtual ASTNode *clone() const override;

        virtual void cloneProperties(ASTNode *newNode) const override;

        virtual void replaceTemplateParams(const StringArray &formals, const StringArray &actuals) override;
        
    protected:
        virtual void dumpProperties(rapidjson::PrettyWriter<JsonStream>& writer) const override;
        
    public:
        String              DataType;       /// 数据类型
        ASTFileInfo         FileInfo;       /// 属性所在文件信息
    };

    /**
     * @brief 枚举结点
     */
    class ASTEnum : public ASTNode
    {
    public:
        ASTEnum(const String &name)
            : ASTNode(name)
        {}

        virtual Type getType() const override
        {
            return Type::kEnum;
        }

        virtual String getTypeString() const override
        {
            return "Enumeration";
        }

        virtual TResult generateSourceFile(FileDataStream& fs) const override;

        virtual ASTNode *clone() const override;

        virtual void cloneProperties(ASTNode *newNode) const override;
        
    public:
        ASTFileInfo         FileInfo;       /// 枚举所在文件信息
    };

    /**
     * @brief 枚举常量
     */
    class ASTEnumConstant : public ASTNode
    {
    public:
        ASTEnumConstant(const String &name)
            : ASTNode(name)
            , Value(0)
        {}

        virtual Type getType() const override
        {
            return Type::kEnumConstant;
        }

        virtual String getTypeString() const override
        {
            return "Enumeration Constant";
        }

        virtual ASTNode *clone() const override;

        virtual void cloneProperties(ASTNode *newNode) const override;
        
    protected:
        virtual void dumpProperties(rapidjson::PrettyWriter<JsonStream>& writer) const override;
        
    public:
        uint64_t    Value;
    };

    /**
     * @brief 模板参数
     */
    struct ASTTemplateParam
    {
        /** 模板参数类型 */
        enum class Kind : uint32_t
        {
            kNone = 0,
            kTemplateType,      /// 模板类型参数，T
            kNonType,           /// 非模板类型参数，int
            kTemplateTemplate,  /// 模板的模板类型参数，template<typename T> class U
        };

        ASTTemplateParam()
            : kind(Kind::kNone)
        {}

        String  type;   /// 数据类型
        String  name;   /// 模板参数名称
        Kind    kind;   /// 模板参数类型
    };

    /**
     * @brief 模板（类模板和函数模板）
     */
    class ASTTemplate
    {
    public:
        ASTTemplate()
            : IsSpecialization(false)
        {}

        void instantiateTemplate(const StringArray &actuals);
        
        void cloneTemplateProperties(ASTTemplate *newTemplate) const;
        
    protected:
        void dumpTemplateProperties(rapidjson::PrettyWriter<JsonStream> &writer) const;

    public:
        TList<ASTTemplateParam> TemplateParams;     /// 模板参数列表
        bool                    IsSpecialization;   /// 是否特化或实例化
    };
    
    /**
     * @brief 类模板结点
     */
    class ASTClassTemplate : public ASTStruct, public ASTTemplate
    {
    public:
        ASTClassTemplate(const String &name)
            : ASTStruct(name)
        {}

        virtual Type getType() const override
        {
            return Type::kClassTemplate;
        }

        virtual String getTypeString() const override
        {
            return "Class Template";
        }

        void setName(const String &name)
        {
            mName = name;
        }

        virtual ASTNode *clone() const override;

        virtual void cloneProperties(ASTNode *newNode) const override;

        virtual void replaceTemplateParams(const StringArray &formals, const StringArray &actuals) override;
        
    protected:
        virtual void dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const override;
    };

    /**
     * @brief 函数模板结点
     */
    class ASTFunctionTemplate : public ASTFunction, public ASTTemplate
    {
    public:
        ASTFunctionTemplate(const String &name)
            : ASTFunction(name)
            , HasSpecialization(false)
        {}

        virtual Type getType() const override
        {
            return Type::kFunctionTemplate;
        }

        virtual String getTypeString() const override
        {
            return "Function Template";
        }

        void setName(const String &name)
        {
            mName = name;
        }
        
        virtual TResult generateSourceFile(FileDataStream &fs) const override;

        virtual ASTNode *clone() const override;

        virtual void cloneProperties(ASTNode *newNode) const override;
        
    protected:
        virtual TResult generateSourceFileForProperty(FileDataStream &fs) const override;

        virtual TResult generateSourceFileForFunction(FileDataStream &fs) const override;

        virtual void dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const override;

    public:
        bool HasSpecialization;  /// 是否有实例化模板
    };
    
    // /**
    //  * @brief 可重载函数模板结点
    //  */
    // class ASTOverloadFunctionTemplate : public ASTOverloadFunction, public ASTTemplate
    // {
    // public:
    //     ASTOverloadFunctionTemplate(const String &name)
    //         : ASTOverloadFunction(name)
    //     {}
    //
    //     virtual Type getType() const override
    //     {
    //         return Type::kOverloadFunctionTemplate;
    //     }
    //
    //     virtual String getTypeString() const override
    //     {
    //         return "Overload Function Template";
    //     }
    //
    //     void setName(const String &name)
    //     {
    //         mName = name;
    //     }
    //
    //     virtual ASTNode *clone() const override;
    //
    //     virtual void cloneProperties(ASTNode *newNode) const override;
    //
    //     virtual void replaceTemplateParams(const StringArray &formals, const StringArray &actuals) override;
    //
    // protected:
    //     virtual void dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const override;
    // };
    //
    // /**
    //  * @brief 静态函数模板结点
    //  */
    // class ASTStaticFunctionTemplate : public ASTStaticFunction, public ASTTemplate
    // {
    // public:
    //     ASTStaticFunctionTemplate(const String &name)
    //         : ASTStaticFunction(name)
    //     {}
    //
    //     virtual Type getType() const override
    //     {
    //         return Type::kStaticFunctionTemplate;
    //     }
    //
    //     virtual String getTypeString() const override
    //     {
    //         return "Static Function Template";
    //     }
    //
    //     virtual ASTNode *clone() const override;
    //
    //     virtual void cloneProperties(ASTNode *newNode) const override;
    //     
    // protected:
    //     virtual void dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const override;
    // };
    //
    // /**
    //  * @brief 成员函数模板结点
    //  */
    // class ASTInstanceFunctionTemplate : public ASTInstanceFunction, public ASTTemplate
    // {
    // public:
    //     ASTInstanceFunctionTemplate(const String &name)
    //         : ASTInstanceFunction(name)
    //     {}
    //
    //     virtual Type getType() const override
    //     {
    //         return Type::kInstanceFunctionTemplate;
    //     }
    //
    //     virtual String getTypeString() const override
    //     {
    //         return "Instance Function Template";
    //     }
    //
    //     virtual ASTNode *clone() const override;
    //
    //     virtual void cloneProperties(ASTNode *newNode) const override;
    //
    // protected:
    //     virtual void dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const override;
    // };
    //
    // /**
    //  * @brief 构造函数模板结点
    //  */
    // class ASTConstructorTemplate : public ASTConstructor, public ASTTemplate
    // {
    // public:
    //     ASTConstructorTemplate(const String &name)
    //         : ASTConstructor(name)
    //     {}
    //
    //     virtual Type getType() const override
    //     {
    //         return Type::kConstructorTemplate;
    //     }
    //
    //     virtual String getTypeString() const override
    //     {
    //         return "Constructor Template";
    //     }
    //
    //     virtual ASTNode *clone() const override;
    //
    //     virtual void cloneProperties(ASTNode *newNode) const override;
    //     
    // protected:
    //     virtual void dumpProperties(rapidjson::PrettyWriter<JsonStream> &writer) const override;
    // };
}

#endif  /*__T3D_ABSTRACT_SYNTAX_TREE_H__*/
