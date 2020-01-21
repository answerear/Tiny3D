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


#ifndef __T3D_METHOD_H__
#define __T3D_METHOD_H__


#include "Reflection/T3DMember.h"
#include "Reflection/T3DReflectionMacro.h"


namespace Tiny3D
{
    /**
     * @class   MethodBase
     * @brief   A 3D platform api.
     */
    class T3D_PLATFORM_API MethodBase : public MemberBase
    {
        T3D_DISABLE_COPY(MethodBase);

        friend class Class;

    public:
        /**
         * @fn  virtual MethodBase::~MethodBase();
         * @brief   Destructor
         */
        virtual ~MethodBase();

        /**
         * @fn  const char MethodBase::*getID() const
         * @brief   Gets the identifier
         * @return  Null if it fails, else the identifier.
         */
        const char *getID() const { return mID.c_str(); }

        /**
         * @fn  const char MethodBase::*getSignature() const
         * @brief   Gets the signature
         * @return  Null if it fails, else the signature.
         */
        const char *getSignature() const { return mSignature.c_str(); }

        /**
         * @fn  const char MethodBase::*getArgs() const
         * @brief   Gets the arguments
         * @return  Null if it fails, else the arguments.
         */
        const char *getArgs() const { return mArgs.c_str(); }

        /**
         * @fn  int MethodBase::getArgsCount() const;
         * @brief   Gets arguments count
         * @return  The arguments count.
         */
        int getArgsCount() const;

        /**
         * @fn  const Class MethodBase::*getRuturnClass() const;
         * @brief   Gets ruturn class
         * @return  Null if it fails, else the ruturn class.
         */
        const Class *getRuturnClass() const;

    protected:
        /**
         * @fn  MethodBase::MethodBase(const Class *owner, AccessType access, 
         *      const char *type, const char *name, const char *args);
         * @brief   Constructor
         * @param   owner   The owner.
         * @param   access  The access.
         * @param   type    The type.
         * @param   name    The name.
         * @param   args    The arguments.
         */
        MethodBase(const Class *owner, AccessType access, const char *type, 
            const char *name, const char *args);

        /**
         * @fn  virtual std::string 
         *      MethodBase::getPrefix(const Class *retType) const;
         * @brief   Gets a prefix
         * @param   retType Type of the ret.
         * @return  The prefix.
         */
        virtual std::string getPrefix(const Class *retType) const;

    private:
        /**
         * @fn  void MethodBase::set_callable(__callable__ *cb);
         * @brief   Sets a callable
         * @param [in,out]  cb  If non-null, the cb.
         */
        void set_callable(__callable__ *cb);

    protected:
        __callable__ *mCallable;    /**< The callable */

    private:
        std::string mID;        /**< The identifier */
        std::string mArgs;      /**< The arguments */
        std::string mSignature; /**< The signature */
        bool        mOwnCB;     /**< True to own cb */
    };


    /**
     * @class   Method
     * @brief   A 3D platform api.
     */
    class T3D_PLATFORM_API Method : public MethodBase
    {
        friend class Class;
        friend struct __register_method__;

    public:
        /**
         * @fn  virtual Method::~Method();
         * @brief   Destructor
         */
        virtual ~Method();

        /**
         * @fn  bool Method::isVirtual() const;
         * @brief   Query if this object is virtual
         * @return  True if virtual, false if not.
         */
        bool isVirtual() const { return mIsVirtual; }

        __INVOKE__(0)
        __INVOKE__(1)
        __INVOKE__(2)
        __INVOKE__(3)
        __INVOKE__(4)
        __INVOKE__(5)
        __INVOKE__(6)
        __INVOKE__(7)
        __INVOKE__(8)
        __INVOKE__(9)
        __INVOKE__(10)
        __INVOKE__(11)
        __INVOKE__(12)
        __INVOKE__(13)
        __INVOKE__(14)
        __INVOKE__(15)
        __INVOKE__(16)
        __INVOKE__(17)
        __INVOKE__(18)
        __INVOKE__(19)
        __INVOKE__(20)

        __INVOKE_VOID__(0)
        __INVOKE_VOID__(1)
        __INVOKE_VOID__(2)
        __INVOKE_VOID__(3)
        __INVOKE_VOID__(4)
        __INVOKE_VOID__(5)
        __INVOKE_VOID__(6)
        __INVOKE_VOID__(7)
        __INVOKE_VOID__(8)
        __INVOKE_VOID__(9)
        __INVOKE_VOID__(10)
        __INVOKE_VOID__(11)
        __INVOKE_VOID__(12)
        __INVOKE_VOID__(13)
        __INVOKE_VOID__(14)
        __INVOKE_VOID__(15)
        __INVOKE_VOID__(16)
        __INVOKE_VOID__(17)
        __INVOKE_VOID__(18)
        __INVOKE_VOID__(19)
        __INVOKE_VOID__(20)
    private:
        /**
         * @fn  Method::Method(const Class *cls, AccessType access, 
         *      const char *type, const char *name, const char *args, bool virt);
         * @brief   Constructor
         * @param   cls     The cls.
         * @param   access  The access.
         * @param   type    The type.
         * @param   name    The name.
         * @param   args    The arguments.
         * @param   virt    True to virt.
         */
        Method(const Class *cls, AccessType access, const char *type, 
            const char *name, const char *args, bool virt);

        bool mIsVirtual;    /**< True if is virtual, false if not */
    };

    /**
     * @class   StaticMethod
     * @brief   A 3D platform api.
     */
    class T3D_PLATFORM_API StaticMethod : public MethodBase
    {
        friend class Class;
        friend struct __register_static_method__;

    public:
        /**   
         * @brief invokes the non-void method
         * @param result optional variable to store the result (if the method is non-void)
         * @param t1...tn parameters of the invocation
         */
        __STATIC_INVOKE__(0, invoke, mCallable)
        __STATIC_INVOKE__(1, invoke, mCallable)
        __STATIC_INVOKE__(2, invoke, mCallable)
        __STATIC_INVOKE__(3, invoke, mCallable)
        __STATIC_INVOKE__(4, invoke, mCallable)
        __STATIC_INVOKE__(5, invoke, mCallable)
        __STATIC_INVOKE__(6, invoke, mCallable)
        __STATIC_INVOKE__(7, invoke, mCallable)
        __STATIC_INVOKE__(8, invoke, mCallable)
        __STATIC_INVOKE__(9, invoke, mCallable)
        __STATIC_INVOKE__(10, invoke, mCallable)
        __STATIC_INVOKE__(11, invoke, mCallable)
        __STATIC_INVOKE__(12, invoke, mCallable)
        __STATIC_INVOKE__(13, invoke, mCallable)
        __STATIC_INVOKE__(14, invoke, mCallable)
        __STATIC_INVOKE__(15, invoke, mCallable)
        __STATIC_INVOKE__(16, invoke, mCallable)
        __STATIC_INVOKE__(17, invoke, mCallable)
        __STATIC_INVOKE__(18, invoke, mCallable)
        __STATIC_INVOKE__(19, invoke, mCallable)
        __STATIC_INVOKE__(20, invoke, mCallable)

        /**   
            * @brief invokes the void method
            * @param t1...tn parameters of the invocation
            */
        __STATIC_INVOKE_VOID__(0)
        __STATIC_INVOKE_VOID__(1)
        __STATIC_INVOKE_VOID__(2)
        __STATIC_INVOKE_VOID__(3)
        __STATIC_INVOKE_VOID__(4)
        __STATIC_INVOKE_VOID__(5)
        __STATIC_INVOKE_VOID__(6)
        __STATIC_INVOKE_VOID__(7)
        __STATIC_INVOKE_VOID__(8)
        __STATIC_INVOKE_VOID__(9)
        __STATIC_INVOKE_VOID__(10)
        __STATIC_INVOKE_VOID__(11)
        __STATIC_INVOKE_VOID__(12)
        __STATIC_INVOKE_VOID__(13)
        __STATIC_INVOKE_VOID__(14)
        __STATIC_INVOKE_VOID__(15)
        __STATIC_INVOKE_VOID__(16)
        __STATIC_INVOKE_VOID__(17)
        __STATIC_INVOKE_VOID__(18)
        __STATIC_INVOKE_VOID__(19)
        __STATIC_INVOKE_VOID__(20)

    protected:
        /**
         * @fn  StaticMethod::StaticMethod(const Class *owner, 
         *      AccessType access, const char *type, const char *name, 
         *      const char *args);
         * @brief   Constructor
         * @param   owner   The owner.
         * @param   access  The access.
         * @param   type    The type.
         * @param   name    The name.
         * @param   args    The arguments.
         */
        StaticMethod(const Class *owner, AccessType access, const char *type, 
            const char *name, const char *args);

        /**
         * @fn  std::string StaticMethod::getPrefix(const Class *retType) const;
         * @brief   Gets a prefix
         * @param   retType Type of the ret.
         * @return  The prefix.
         */
        std::string getPrefix(const Class *retType) const;
    };

    class T3D_PLATFORM_API ConstructorMethod : public StaticMethod
    {
        friend struct __register_constructor_method__;

    public:
        __CONSTRUCTOR_PLACEMENT_INVOKE__(0)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(1)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(2)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(3)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(4)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(5)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(6)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(7)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(8)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(9)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(10)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(11)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(12)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(13)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(14)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(15)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(16)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(17)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(18)
        __CONSTRUCTOR_PLACEMENT_INVOKE__(19)

    protected:
        std::string getPrefix(const Class *retType) const;

    private:
        __STATIC_INVOKE__(0, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(1, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(2, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(3, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(4, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(5, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(6, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(7, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(8, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(9, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(10, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(11, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(12, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(13, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(14, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(15, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(16, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(17, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(18, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(19, _invoke_placement, m_placement_callable)
        __STATIC_INVOKE__(20, _invoke_placement, m_placement_callable)

        ConstructorMethod(const Class *owner, AccessType access, 
            const char *type, const char *name, const char *args, 
            __callable__ *callable);

        __callable__ *mPlacementCallable;
    };
}


#endif  /*__T3D_METHOD_H__*/
