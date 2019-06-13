/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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

#ifndef __T3D_SCRIPT_TYPE_H__
#define __T3D_SCRIPT_TYPE_H__


#include "T3DScriptPrerequisites.h"


namespace Tiny3D
{
    /**
         * @brief 符号ID
         */
    enum
    {
        TID_LBRACE = 0,     /// {
        TID_RBRACE,         /// }
        TID_COLON,          /// :
        TID_VARIABLE,       /// $...
        TID_WORD,           /// *
        TID_QUOTE,          /// "*"
        TID_NEWLINE,        /// \n
        TID_UNKNOWN,        ///
        TID_END
    };

    /**
     * @brief 符号结构
     */
    struct Token
    {
        String      lexeme; /// 符号的词素
        String      file;   /// 当前文件名
        uint32_t    type;   /// 符号对应的ID
        uint32_t    line;   /// 当前行号
    };

    typedef std::shared_ptr<Token>      TokenPtr;
    typedef std::vector<TokenPtr>       TokenList;
    typedef std::shared_ptr<TokenList>  TokenListPtr;

    /**
         * @brief 结点类型
         */
    enum ConcreteNodeType
    {
        CNT_VARIABLE,           /// 变量 $xxx
        CNT_VARIABLE_ASSIGN,    /// 变量赋值，set $xxx
        CNT_WORD,               /// *
        CNT_IMPORT,             /// import * from *
        CNT_QUOTE,              /// "*"
        CNT_LBRACE,             /// {
        CNT_RBRACE,             /// }
        CNT_COLON               /// :
    };

    struct ConcreteNode;

    typedef std::shared_ptr<ConcreteNode>       ConcreteNodePtr;
    typedef std::list<ConcreteNodePtr>          ConcreteNodeList;
    typedef std::shared_ptr<ConcreteNodeList>   ConcreteNodeListPtr;

    struct ConcreteNode
    {
        String              token;      /// 符号名称
        String              file;       /// 脚本文件名
        uint32_t            line;       /// 当前符号行号
        ConcreteNodeType    type;       /// 结点类型
        ConcreteNodeList    children;   /// 子结点
        ConcreteNode        *parent;    /// 父结点
    };
}


#endif  /*__T3D_SCRIPT_TYPE_H__*/
