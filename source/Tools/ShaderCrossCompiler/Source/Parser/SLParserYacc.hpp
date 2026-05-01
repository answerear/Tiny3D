/* A Bison parser, made by GNU Bison 3.7.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_SHADERLAB_SLPARSERYACC_HPP_INCLUDED
# define YY_SHADERLAB_SLPARSERYACC_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int shaderlabdebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    TOKEN_SHADER = 258,            /* TOKEN_SHADER  */
    TOKEN_PASS = 259,              /* TOKEN_PASS  */
    TOKEN_USEPASS = 260,           /* TOKEN_USEPASS  */
    TOKEN_SUBSHADER = 261,         /* TOKEN_SUBSHADER  */
    TOKEN_CATEGORY = 262,          /* TOKEN_CATEGORY  */
    TOKEN_FALLBACK = 263,          /* TOKEN_FALLBACK  */
    TOKEN_LOD = 264,               /* TOKEN_LOD  */
    TOKEN_COLOR = 265,             /* TOKEN_COLOR  */
    TOKEN_ALPHA_TO_MASK = 266,     /* TOKEN_ALPHA_TO_MASK  */
    TOKEN_TRUE = 267,              /* TOKEN_TRUE  */
    TOKEN_FALSE = 268,             /* TOKEN_FALSE  */
    TOKEN_FLOAT = 269,             /* TOKEN_FLOAT  */
    TOKEN_RANGE = 270,             /* TOKEN_RANGE  */
    TOKEN_VECTOR = 271,            /* TOKEN_VECTOR  */
    TOKEN_INTEGER = 272,           /* TOKEN_INTEGER  */
    TOKEN_PROPERTIES = 273,        /* TOKEN_PROPERTIES  */
    TOKEN_TAGS = 274,              /* TOKEN_TAGS  */
    TOKEN_NAME = 275,              /* TOKEN_NAME  */
    TOKEN_ZWRITE = 276,            /* TOKEN_ZWRITE  */
    TOKEN_ZTEST = 277,             /* TOKEN_ZTEST  */
    TOKEN_CULL = 278,              /* TOKEN_CULL  */
    TOKEN_ZCLIP = 279,             /* TOKEN_ZCLIP  */
    TOKEN_BLEND = 280,             /* TOKEN_BLEND  */
    TOKEN_BLEND_OP = 281,          /* TOKEN_BLEND_OP  */
    TOKEN_COLORMASK = 282,         /* TOKEN_COLORMASK  */
    TOKEN_OFFSET = 283,            /* TOKEN_OFFSET  */
    TOKEN_STENCIL = 284,           /* TOKEN_STENCIL  */
    TOKEN_REF = 285,               /* TOKEN_REF  */
    TOKEN_READ_MASK = 286,         /* TOKEN_READ_MASK  */
    TOKEN_WRITE_MASK = 287,        /* TOKEN_WRITE_MASK  */
    TOKEN_COMP = 288,              /* TOKEN_COMP  */
    TOKEN_OP_PASS = 289,           /* TOKEN_OP_PASS  */
    TOKEN_OP_FAIL = 290,           /* TOKEN_OP_FAIL  */
    TOKEN_OP_ZFAIL = 291,          /* TOKEN_OP_ZFAIL  */
    TOKEN_COMP_BACK = 292,         /* TOKEN_COMP_BACK  */
    TOKEN_OP_PASS_BACK = 293,      /* TOKEN_OP_PASS_BACK  */
    TOKEN_OP_FAIL_BACK = 294,      /* TOKEN_OP_FAIL_BACK  */
    TOKEN_OP_ZFAIL_BACK = 295,     /* TOKEN_OP_ZFAIL_BACK  */
    TOKEN_COMP_FRONT = 296,        /* TOKEN_COMP_FRONT  */
    TOKEN_OP_PASS_FRONT = 297,     /* TOKEN_OP_PASS_FRONT  */
    TOKEN_OP_FAIL_FRONT = 298,     /* TOKEN_OP_FAIL_FRONT  */
    TOKEN_OP_ZFAIL_FRONT = 299,    /* TOKEN_OP_ZFAIL_FRONT  */
    TOKEN_GRABPASS = 300,          /* TOKEN_GRABPASS  */
    TOKEN_CONSERVATIVE = 301,      /* TOKEN_CONSERVATIVE  */
    TOKEN_PACKAGE_REQUIREMENTS = 302, /* TOKEN_PACKAGE_REQUIREMENTS  */
    VAL_NUMBER = 303,              /* VAL_NUMBER  */
    VAL_ID = 304,                  /* VAL_ID  */
    VAL_STRING = 305,              /* VAL_STRING  */
    VAL_BRACKET_ID = 306,          /* VAL_BRACKET_ID  */
    VAL_BRACKET_ATTR = 307,        /* VAL_BRACKET_ATTR  */
    VAL_PROGRAM_SOURCE = 308,      /* VAL_PROGRAM_SOURCE  */
    VAL_TRIANGLE_FACE = 309,       /* VAL_TRIANGLE_FACE  */
    VAL_COMPARE_MODE = 310,        /* VAL_COMPARE_MODE  */
    VAL_RGBA_MASK = 311,           /* VAL_RGBA_MASK  */
    VAL_TEX_DIM = 312,             /* VAL_TEX_DIM  */
    VAL_BLEND_FACTOR = 313,        /* VAL_BLEND_FACTOR  */
    VAL_BLEND_OP_MODE = 314,       /* VAL_BLEND_OP_MODE  */
    VAL_STENCIL_ACTION = 315,      /* VAL_STENCIL_ACTION  */
    VAL_HLSLPROGRAM = 316,         /* VAL_HLSLPROGRAM  */
    VAL_GLSLPROGRAM = 317,         /* VAL_GLSLPROGRAM  */
    VAL_CGPROGRAM = 318,           /* VAL_CGPROGRAM  */
    VAL_HLSLINCLUDE = 319,         /* VAL_HLSLINCLUDE  */
    VAL_CGINCLUDE = 320            /* VAL_CGINCLUDE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 38 "SLParserYacc.y"

	SLVector4*							vector4;
	SLFloat*							floatval;
	SLShader*							shader;
	SLProperties*						properties;
	SLShaderState*						states;
	SLPassBase*							pass;
	SLSubShader*						subshader;
	const char*							strval;
	float								number;
	int32_t								enumval;
	std::vector<SLAttribute>*			attributes;
	std::map<std::string, std::string>* tags;
	std::vector<SLPassBase*>*			passes;

#line 145 "SLParserYacc.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE shaderlablval;

int shaderlabparse (void);

#endif /* !YY_SHADERLAB_SLPARSERYACC_HPP_INCLUDED  */
