/* A Bison parser, made by GNU Bison 3.7.4.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30704

/* Bison version string.  */
#define YYBISON_VERSION "3.7.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         shaderlabparse
#define yylex           shaderlablex
#define yyerror         shaderlaberror
#define yydebug         shaderlabdebug
#define yynerrs         shaderlabnerrs
#define yylval          shaderlablval
#define yychar          shaderlabchar

/* First part of user prologue.  */
#line 1 "SLParserYacc.y"


// bison -d -b y -o SLParserYacc.cpp -p "shaderlab_parser" SLParserYacc.y

#include <stdio.h>
#include <stack>
#include <string>
#include <algorithm>
#include <map>
#include <vector>

#include "T3DSCCPrerequisites.h"
#include "SLParserData.h"

using namespace shaderlab;

SLShader*					g_CurrentShader;
std::stack<SLShaderState*>	g_ShaderStateStack;
const char*                 g_CurrentProgramCode;
int32_t						g_CurrentProgramLine;
ProgramType					g_CurrentProgramType;

extern int32_t yylineno;
extern char* yytext;

int32_t shaderlaberror(const char *s);

int32_t shaderlablex();

static void PushShaderState();
static void PopShaderState();

#define YYDEBUG 0


#line 114 "SLParserYacc.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "SLParserYacc.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_TOKEN_SHADER = 3,               /* TOKEN_SHADER  */
  YYSYMBOL_TOKEN_PASS = 4,                 /* TOKEN_PASS  */
  YYSYMBOL_TOKEN_USEPASS = 5,              /* TOKEN_USEPASS  */
  YYSYMBOL_TOKEN_SUBSHADER = 6,            /* TOKEN_SUBSHADER  */
  YYSYMBOL_TOKEN_CATEGORY = 7,             /* TOKEN_CATEGORY  */
  YYSYMBOL_TOKEN_FALLBACK = 8,             /* TOKEN_FALLBACK  */
  YYSYMBOL_TOKEN_LOD = 9,                  /* TOKEN_LOD  */
  YYSYMBOL_TOKEN_COLOR = 10,               /* TOKEN_COLOR  */
  YYSYMBOL_TOKEN_ALPHA_TO_MASK = 11,       /* TOKEN_ALPHA_TO_MASK  */
  YYSYMBOL_TOKEN_TRUE = 12,                /* TOKEN_TRUE  */
  YYSYMBOL_TOKEN_FALSE = 13,               /* TOKEN_FALSE  */
  YYSYMBOL_TOKEN_FLOAT = 14,               /* TOKEN_FLOAT  */
  YYSYMBOL_TOKEN_RANGE = 15,               /* TOKEN_RANGE  */
  YYSYMBOL_TOKEN_VECTOR = 16,              /* TOKEN_VECTOR  */
  YYSYMBOL_TOKEN_INTEGER = 17,             /* TOKEN_INTEGER  */
  YYSYMBOL_TOKEN_PROPERTIES = 18,          /* TOKEN_PROPERTIES  */
  YYSYMBOL_TOKEN_TAGS = 19,                /* TOKEN_TAGS  */
  YYSYMBOL_TOKEN_NAME = 20,                /* TOKEN_NAME  */
  YYSYMBOL_TOKEN_ZWRITE = 21,              /* TOKEN_ZWRITE  */
  YYSYMBOL_TOKEN_ZTEST = 22,               /* TOKEN_ZTEST  */
  YYSYMBOL_TOKEN_CULL = 23,                /* TOKEN_CULL  */
  YYSYMBOL_TOKEN_ZCLIP = 24,               /* TOKEN_ZCLIP  */
  YYSYMBOL_TOKEN_BLEND = 25,               /* TOKEN_BLEND  */
  YYSYMBOL_TOKEN_BLEND_OP = 26,            /* TOKEN_BLEND_OP  */
  YYSYMBOL_TOKEN_COLORMASK = 27,           /* TOKEN_COLORMASK  */
  YYSYMBOL_TOKEN_OFFSET = 28,              /* TOKEN_OFFSET  */
  YYSYMBOL_TOKEN_STENCIL = 29,             /* TOKEN_STENCIL  */
  YYSYMBOL_TOKEN_REF = 30,                 /* TOKEN_REF  */
  YYSYMBOL_TOKEN_READ_MASK = 31,           /* TOKEN_READ_MASK  */
  YYSYMBOL_TOKEN_WRITE_MASK = 32,          /* TOKEN_WRITE_MASK  */
  YYSYMBOL_TOKEN_COMP = 33,                /* TOKEN_COMP  */
  YYSYMBOL_TOKEN_OP_PASS = 34,             /* TOKEN_OP_PASS  */
  YYSYMBOL_TOKEN_OP_FAIL = 35,             /* TOKEN_OP_FAIL  */
  YYSYMBOL_TOKEN_OP_ZFAIL = 36,            /* TOKEN_OP_ZFAIL  */
  YYSYMBOL_TOKEN_COMP_BACK = 37,           /* TOKEN_COMP_BACK  */
  YYSYMBOL_TOKEN_OP_PASS_BACK = 38,        /* TOKEN_OP_PASS_BACK  */
  YYSYMBOL_TOKEN_OP_FAIL_BACK = 39,        /* TOKEN_OP_FAIL_BACK  */
  YYSYMBOL_TOKEN_OP_ZFAIL_BACK = 40,       /* TOKEN_OP_ZFAIL_BACK  */
  YYSYMBOL_TOKEN_COMP_FRONT = 41,          /* TOKEN_COMP_FRONT  */
  YYSYMBOL_TOKEN_OP_PASS_FRONT = 42,       /* TOKEN_OP_PASS_FRONT  */
  YYSYMBOL_TOKEN_OP_FAIL_FRONT = 43,       /* TOKEN_OP_FAIL_FRONT  */
  YYSYMBOL_TOKEN_OP_ZFAIL_FRONT = 44,      /* TOKEN_OP_ZFAIL_FRONT  */
  YYSYMBOL_TOKEN_GRABPASS = 45,            /* TOKEN_GRABPASS  */
  YYSYMBOL_TOKEN_CONSERVATIVE = 46,        /* TOKEN_CONSERVATIVE  */
  YYSYMBOL_TOKEN_PACKAGE_REQUIREMENTS = 47, /* TOKEN_PACKAGE_REQUIREMENTS  */
  YYSYMBOL_VAL_NUMBER = 48,                /* VAL_NUMBER  */
  YYSYMBOL_VAL_ID = 49,                    /* VAL_ID  */
  YYSYMBOL_VAL_STRING = 50,                /* VAL_STRING  */
  YYSYMBOL_VAL_BRACKET_ID = 51,            /* VAL_BRACKET_ID  */
  YYSYMBOL_VAL_BRACKET_ATTR = 52,          /* VAL_BRACKET_ATTR  */
  YYSYMBOL_VAL_PROGRAM_SOURCE = 53,        /* VAL_PROGRAM_SOURCE  */
  YYSYMBOL_VAL_TRIANGLE_FACE = 54,         /* VAL_TRIANGLE_FACE  */
  YYSYMBOL_VAL_COMPARE_MODE = 55,          /* VAL_COMPARE_MODE  */
  YYSYMBOL_VAL_RGBA_MASK = 56,             /* VAL_RGBA_MASK  */
  YYSYMBOL_VAL_TEX_DIM = 57,               /* VAL_TEX_DIM  */
  YYSYMBOL_VAL_BLEND_FACTOR = 58,          /* VAL_BLEND_FACTOR  */
  YYSYMBOL_VAL_BLEND_OP_MODE = 59,         /* VAL_BLEND_OP_MODE  */
  YYSYMBOL_VAL_STENCIL_ACTION = 60,        /* VAL_STENCIL_ACTION  */
  YYSYMBOL_VAL_HLSLPROGRAM = 61,           /* VAL_HLSLPROGRAM  */
  YYSYMBOL_VAL_GLSLPROGRAM = 62,           /* VAL_GLSLPROGRAM  */
  YYSYMBOL_VAL_CGPROGRAM = 63,             /* VAL_CGPROGRAM  */
  YYSYMBOL_VAL_HLSLINCLUDE = 64,           /* VAL_HLSLINCLUDE  */
  YYSYMBOL_VAL_CGINCLUDE = 65,             /* VAL_CGINCLUDE  */
  YYSYMBOL_66_ = 66,                       /* '{'  */
  YYSYMBOL_67_ = 67,                       /* '}'  */
  YYSYMBOL_68_ = 68,                       /* '('  */
  YYSYMBOL_69_ = 69,                       /* ','  */
  YYSYMBOL_70_ = 70,                       /* ')'  */
  YYSYMBOL_71_ = 71,                       /* '='  */
  YYSYMBOL_YYACCEPT = 72,                  /* $accept  */
  YYSYMBOL_shader = 73,                    /* shader  */
  YYSYMBOL_74_1 = 74,                      /* $@1  */
  YYSYMBOL_75_2 = 75,                      /* $@2  */
  YYSYMBOL_properties = 76,                /* properties  */
  YYSYMBOL_propitems = 77,                 /* propitems  */
  YYSYMBOL_propflags = 78,                 /* propflags  */
  YYSYMBOL_vector4 = 79,                   /* vector4  */
  YYSYMBOL_shaderBlocks = 80,              /* shaderBlocks  */
  YYSYMBOL_packageitems = 81,              /* packageitems  */
  YYSYMBOL_category = 82,                  /* category  */
  YYSYMBOL_83_3 = 83,                      /* $@3  */
  YYSYMBOL_subshader = 84,                 /* subshader  */
  YYSYMBOL_85_4 = 85,                      /* $@4  */
  YYSYMBOL_fallback = 86,                  /* fallback  */
  YYSYMBOL_passes = 87,                    /* passes  */
  YYSYMBOL_grabpass = 88,                  /* grabpass  */
  YYSYMBOL_pass = 89,                      /* pass  */
  YYSYMBOL_90_5 = 90,                      /* $@5  */
  YYSYMBOL_states = 91,                    /* states  */
  YYSYMBOL_cullval = 92,                   /* cullval  */
  YYSYMBOL_boolean = 93,                   /* boolean  */
  YYSYMBOL_tags = 94,                      /* tags  */
  YYSYMBOL_stencilblock = 95,              /* stencilblock  */
  YYSYMBOL_stencilaction = 96,             /* stencilaction  */
  YYSYMBOL_numval = 97,                    /* numval  */
  YYSYMBOL_blendval = 98,                  /* blendval  */
  YYSYMBOL_blendopval = 99,                /* blendopval  */
  YYSYMBOL_maskval = 100,                  /* maskval  */
  YYSYMBOL_funcval = 101                   /* funcval  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   239

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  72
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  30
/* YYNRULES -- Number of rules.  */
#define YYNRULES  109
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  222

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   320


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      68,    70,     2,     2,    69,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    71,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    66,     2,    67,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    85,    85,    89,    84,   103,   106,   113,   116,   125,
     134,   143,   153,   163,   177,   194,   197,   204,   243,   248,
     256,   259,   263,   270,   274,   281,   288,   294,   296,   301,
     311,   310,   321,   320,   339,   344,   349,   355,   360,   365,
     370,   377,   381,   389,   388,   413,   421,   424,   429,   436,
     443,   450,   457,   462,   467,   474,   481,   488,   493,   507,
     519,   527,   537,   546,   559,   569,   576,   583,   600,   615,
     626,   632,   638,   646,   653,   660,   667,   672,   680,   683,
     694,   708,   712,   719,   726,   733,   740,   747,   754,   761,
     768,   775,   782,   789,   796,   803,   810,   819,   826,   833,
     838,   847,   854,   861,   868,   875,   882,   887,   894,   901
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "TOKEN_SHADER",
  "TOKEN_PASS", "TOKEN_USEPASS", "TOKEN_SUBSHADER", "TOKEN_CATEGORY",
  "TOKEN_FALLBACK", "TOKEN_LOD", "TOKEN_COLOR", "TOKEN_ALPHA_TO_MASK",
  "TOKEN_TRUE", "TOKEN_FALSE", "TOKEN_FLOAT", "TOKEN_RANGE",
  "TOKEN_VECTOR", "TOKEN_INTEGER", "TOKEN_PROPERTIES", "TOKEN_TAGS",
  "TOKEN_NAME", "TOKEN_ZWRITE", "TOKEN_ZTEST", "TOKEN_CULL", "TOKEN_ZCLIP",
  "TOKEN_BLEND", "TOKEN_BLEND_OP", "TOKEN_COLORMASK", "TOKEN_OFFSET",
  "TOKEN_STENCIL", "TOKEN_REF", "TOKEN_READ_MASK", "TOKEN_WRITE_MASK",
  "TOKEN_COMP", "TOKEN_OP_PASS", "TOKEN_OP_FAIL", "TOKEN_OP_ZFAIL",
  "TOKEN_COMP_BACK", "TOKEN_OP_PASS_BACK", "TOKEN_OP_FAIL_BACK",
  "TOKEN_OP_ZFAIL_BACK", "TOKEN_COMP_FRONT", "TOKEN_OP_PASS_FRONT",
  "TOKEN_OP_FAIL_FRONT", "TOKEN_OP_ZFAIL_FRONT", "TOKEN_GRABPASS",
  "TOKEN_CONSERVATIVE", "TOKEN_PACKAGE_REQUIREMENTS", "VAL_NUMBER",
  "VAL_ID", "VAL_STRING", "VAL_BRACKET_ID", "VAL_BRACKET_ATTR",
  "VAL_PROGRAM_SOURCE", "VAL_TRIANGLE_FACE", "VAL_COMPARE_MODE",
  "VAL_RGBA_MASK", "VAL_TEX_DIM", "VAL_BLEND_FACTOR", "VAL_BLEND_OP_MODE",
  "VAL_STENCIL_ACTION", "VAL_HLSLPROGRAM", "VAL_GLSLPROGRAM",
  "VAL_CGPROGRAM", "VAL_HLSLINCLUDE", "VAL_CGINCLUDE", "'{'", "'}'", "'('",
  "','", "')'", "'='", "$accept", "shader", "$@1", "$@2", "properties",
  "propitems", "propflags", "vector4", "shaderBlocks", "packageitems",
  "category", "$@3", "subshader", "$@4", "fallback", "passes", "grabpass",
  "pass", "$@5", "states", "cullval", "boolean", "tags", "stencilblock",
  "stencilaction", "numval", "blendval", "blendopval", "maskval",
  "funcval", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   123,   125,    40,    44,
      41,    61
};
#endif

#define YYPACT_NINF (-104)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      41,   -35,    48,  -104,  -104,   -16,    70,   -12,  -104,  -104,
    -104,    10,     0,  -104,    77,    32,    35,    -4,    50,    46,
      64,  -104,  -104,  -104,  -104,    52,  -104,  -104,  -104,  -104,
    -104,    61,  -104,  -104,  -104,    71,  -104,  -104,    95,   -48,
      75,    78,    12,   113,  -104,    79,  -104,    45,  -104,    12,
      83,   101,   107,     1,    97,   114,     1,    15,    -9,     1,
       5,    38,    30,    36,    99,   100,     1,   115,   116,   117,
    -104,  -104,     4,   121,   102,   103,   124,   123,   125,   126,
    -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,
    -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,
    -104,  -104,  -104,    27,  -104,  -104,    27,  -104,  -104,    98,
    -104,  -104,  -104,   146,  -104,  -104,   128,  -104,    26,  -104,
    -104,  -104,  -104,  -104,  -104,   127,   129,   151,   130,   131,
     132,  -104,    33,    27,   135,    38,  -104,    36,   147,   138,
    -104,   139,   158,   140,   139,   160,   161,    85,   141,   142,
    -104,   148,    27,  -104,  -104,    36,    36,    36,   -29,   -31,
     -31,   -31,   -29,   -31,   -31,   -31,   -29,   -31,   -31,   -31,
    -104,  -104,   162,  -104,  -104,   167,  -104,  -104,   150,  -104,
     169,   170,    27,    27,  -104,  -104,  -104,  -104,  -104,  -104,
    -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,
    -104,   152,   149,   155,  -104,  -104,    27,  -104,   175,   154,
    -104,  -104,   156,   157,   178,   179,   -45,  -104,   181,  -104,
     163,  -104
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     2,     1,     0,     5,     0,     3,     7,
      20,    15,     0,     6,     0,     0,     0,     0,     0,     0,
       0,     4,    21,    22,    23,     0,    16,    17,    32,    30,
      36,    34,    27,    24,    25,     0,    46,    46,     0,     0,
       0,    46,     0,    20,    35,    28,    26,     0,    33,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      39,    37,     0,     0,     0,     0,     0,     0,     0,     0,
      40,    38,    43,    45,    47,    76,    77,    75,    50,    78,
      48,    54,    52,    53,   108,   109,    51,    57,    73,    74,
      56,    65,    60,     0,   101,   102,     0,   103,   104,    62,
     107,   105,   106,    55,    99,   100,     0,    81,     0,    66,
      70,    71,    72,    31,    29,     0,     0,     0,     0,     0,
       0,    46,     0,     0,    59,     0,    69,     0,     0,     0,
      41,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      49,    68,     0,    61,    64,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      63,    42,     0,    12,     9,     0,    11,     8,    13,    44,
       0,     0,     0,     0,    82,    83,    84,    85,    97,    98,
      86,    87,    88,    93,    94,    95,    96,    89,    90,    91,
      92,     0,     0,     0,    79,    80,     0,    58,     0,     0,
      14,    67,     0,     0,     0,     0,     0,    10,     0,    19,
       0,    18
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -104,  -104,  -104,  -104,  -104,  -104,  -104,    86,   188,  -104,
    -104,  -104,  -104,  -104,  -104,  -104,   183,   185,  -104,   -36,
    -104,    59,  -104,  -104,    -7,   -65,  -103,   104,  -104,   -39
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     5,    10,     8,    11,    14,   173,    12,    39,
      22,    37,    23,    36,    24,    41,    70,    71,   131,    42,
     100,    88,   132,   138,   190,   116,   106,   109,   113,    96
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
     133,    43,    45,   134,    97,    49,    15,    16,    17,    30,
      15,    16,    17,    85,    86,     3,    50,    51,   102,    46,
     188,    52,    94,    53,   218,   219,    95,    92,    93,   189,
     151,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    98,   110,     1,    99,    31,    18,     4,   183,
       6,    18,    87,   103,     9,    74,   104,    65,    66,    75,
      76,    77,    78,   105,    19,    20,    94,    21,    19,    20,
      95,   123,   154,    67,    68,    69,   139,    13,   104,   206,
     207,   111,   148,   149,   114,   105,   112,   115,     7,   107,
     184,   185,   186,   140,    52,   147,    53,   108,    28,    33,
     150,    29,    79,   211,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    91,    32,    34,   101,   187,
      35,    40,    52,   193,    53,   119,    25,   197,    26,    27,
      38,    66,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    44,    47,    48,    67,    68,    69,    82,
      73,    83,   179,   191,   192,    84,   194,   195,   196,    66,
     198,   199,   200,    89,    90,   117,   118,   135,   120,   121,
     122,   124,   125,   126,    67,    68,    69,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   127,   128,   136,   129,   130,   137,   141,   143,
     142,   144,   145,   146,   152,   171,   174,   172,   177,   175,
     201,   178,   180,   181,   170,   202,   203,   182,   204,   209,
     205,   208,   210,   212,   213,   214,   216,   217,   215,   220,
     176,    72,    80,   221,    81,     0,     0,     0,     0,   153
};

static const yytype_int16 yycheck[] =
{
     103,    37,    50,   106,    13,    41,     6,     7,     8,    13,
       6,     7,     8,    12,    13,    50,     4,     5,    13,    67,
      51,     9,    51,    11,    69,    70,    55,    12,    13,    60,
     133,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    51,    13,     3,    54,    50,    47,     0,   152,
      66,    47,    51,    48,    66,    10,    51,    45,    46,    14,
      15,    16,    17,    58,    64,    65,    51,    67,    64,    65,
      55,    67,   137,    61,    62,    63,    50,    67,    51,   182,
     183,    51,    49,    50,    48,    58,    56,    51,    18,    51,
     155,   156,   157,    67,     9,   131,    11,    59,    66,    53,
      67,    66,    57,   206,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    56,    66,    53,    59,   158,
      68,    50,     9,   162,    11,    66,    49,   166,    51,    52,
      69,    46,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    48,    69,    67,    61,    62,    63,    66,
      71,    50,    67,   160,   161,    48,   163,   164,   165,    46,
     167,   168,   169,    66,    50,    66,    66,    69,    53,    53,
      53,    50,    70,    70,    61,    62,    63,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    68,    70,    48,    70,    70,    69,    71,    48,
      71,    71,    71,    71,    69,    67,    48,    68,    48,    69,
      48,    50,    71,    71,    67,    48,    66,    69,    49,    70,
      50,    69,    67,    48,    70,    69,    48,    48,    71,    48,
     144,    43,    49,    70,    49,    -1,    -1,    -1,    -1,   135
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,    73,    50,     0,    74,    66,    18,    76,    66,
      75,    77,    80,    67,    78,     6,     7,     8,    47,    64,
      65,    67,    82,    84,    86,    49,    51,    52,    66,    66,
      13,    50,    66,    53,    53,    68,    85,    83,    69,    81,
      50,    87,    91,    91,    48,    50,    67,    69,    67,    91,
       4,     5,     9,    11,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    45,    46,    61,    62,    63,
      88,    89,    80,    71,    10,    14,    15,    16,    17,    57,
      88,    89,    66,    50,    48,    12,    13,    51,    93,    66,
      50,    93,    12,    13,    51,    55,   101,    13,    51,    54,
      92,    93,    13,    48,    51,    58,    98,    51,    59,    99,
      13,    51,    56,   100,    48,    51,    97,    66,    66,    93,
      53,    53,    53,    67,    50,    70,    70,    68,    70,    70,
      70,    90,    94,    98,    98,    69,    48,    69,    95,    50,
      67,    71,    71,    48,    71,    71,    71,    91,    49,    50,
      67,    98,    69,    99,    97,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      67,    67,    68,    79,    48,    69,    79,    48,    50,    67,
      71,    71,    69,    98,    97,    97,    97,   101,    51,    60,
      96,    96,    96,   101,    96,    96,    96,   101,    96,    96,
      96,    48,    48,    66,    49,    50,    98,    98,    69,    70,
      67,    98,    48,    70,    69,    71,    48,    48,    69,    70,
      48,    70
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    72,    74,    75,    73,    76,    76,    77,    77,    77,
      77,    77,    77,    77,    77,    78,    78,    78,    79,    79,
      80,    80,    80,    80,    80,    80,    80,    81,    81,    81,
      83,    82,    85,    84,    86,    86,    86,    87,    87,    87,
      87,    88,    88,    90,    89,    89,    91,    91,    91,    91,
      91,    91,    91,    91,    91,    91,    91,    91,    91,    91,
      91,    91,    91,    91,    91,    91,    91,    91,    91,    91,
      91,    91,    91,    92,    92,    93,    93,    93,    94,    94,
      94,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    96,    96,    97,
      97,    98,    98,    99,    99,   100,   100,   100,   101,   101
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     0,     8,     0,     4,     0,    10,    10,
      15,    10,    10,    10,    12,     0,     2,     2,     9,     7,
       0,     2,     2,     2,     3,     3,     5,     0,     2,     4,
       0,     6,     0,     5,     2,     4,     2,     2,     3,     2,
       3,     3,     4,     0,     5,     2,     0,     3,     3,     5,
       3,     3,     3,     3,     3,     3,     3,     3,     7,     4,
       3,     5,     3,     5,     5,     3,     3,     8,     5,     4,
       3,     3,     3,     1,     1,     1,     1,     1,     0,     4,
       4,     0,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* $@1: %empty  */
#line 85 "SLParserYacc.y"
                        {
				g_CurrentShader->shaderName = (yyvsp[0].strval);
			}
#line 1367 "SLParserYacc.cpp"
    break;

  case 3: /* $@2: %empty  */
#line 89 "SLParserYacc.y"
                        {
				if ((yyvsp[0].properties)) 
				{
					g_CurrentShader->properties = *(yyvsp[0].properties);
					delete (yyvsp[0].properties);
				}
			}
#line 1379 "SLParserYacc.cpp"
    break;

  case 4: /* shader: TOKEN_SHADER VAL_STRING $@1 '{' properties $@2 shaderBlocks '}'  */
#line 97 "SLParserYacc.y"
                        {

			}
#line 1387 "SLParserYacc.cpp"
    break;

  case 5: /* properties: %empty  */
#line 103 "SLParserYacc.y"
                        {
				(yyval.properties) = nullptr;
			}
#line 1395 "SLParserYacc.cpp"
    break;

  case 6: /* properties: TOKEN_PROPERTIES '{' propitems '}'  */
#line 107 "SLParserYacc.y"
                        {
				(yyval.properties) = (yyvsp[-1].properties); 
			}
#line 1403 "SLParserYacc.cpp"
    break;

  case 7: /* propitems: %empty  */
#line 113 "SLParserYacc.y"
                        {
				(yyval.properties) = new SLProperties();
			}
#line 1411 "SLParserYacc.cpp"
    break;

  case 8: /* propitems: propitems propflags VAL_ID '(' VAL_STRING ',' TOKEN_INTEGER ')' '=' VAL_NUMBER  */
#line 117 "SLParserYacc.y"
                        {
				(yyval.properties) = (yyvsp[-9].properties);
				(yyval.properties)->AddIntegerProperty((yyvsp[-7].strval), (yyvsp[-5].strval), *(yyvsp[-8].attributes), (int32_t)(yyvsp[0].number));

				delete   (yyvsp[-8].attributes);
				delete[] (yyvsp[-7].strval);
				delete[] (yyvsp[-5].strval);
			}
#line 1424 "SLParserYacc.cpp"
    break;

  case 9: /* propitems: propitems propflags VAL_ID '(' VAL_STRING ',' TOKEN_FLOAT ')' '=' VAL_NUMBER  */
#line 126 "SLParserYacc.y"
                        {
				(yyval.properties) = (yyvsp[-9].properties);
				(yyval.properties)->AddFloatProperty((yyvsp[-7].strval), (yyvsp[-5].strval), *(yyvsp[-8].attributes), (yyvsp[0].number));

				delete   (yyvsp[-8].attributes);
				delete[] (yyvsp[-7].strval);
				delete[] (yyvsp[-5].strval);
			}
#line 1437 "SLParserYacc.cpp"
    break;

  case 10: /* propitems: propitems propflags VAL_ID '(' VAL_STRING ',' TOKEN_RANGE '(' VAL_NUMBER ',' VAL_NUMBER ')' ')' '=' VAL_NUMBER  */
#line 135 "SLParserYacc.y"
                        {
				(yyval.properties) = (yyvsp[-14].properties);
				(yyval.properties)->AddRangeProperty((yyvsp[-12].strval), (yyvsp[-10].strval), *(yyvsp[-13].attributes), (yyvsp[0].number), (yyvsp[-6].number), (yyvsp[-4].number)); 

				delete   (yyvsp[-13].attributes);
				delete[] (yyvsp[-12].strval);
				delete[] (yyvsp[-10].strval);
			}
#line 1450 "SLParserYacc.cpp"
    break;

  case 11: /* propitems: propitems propflags VAL_ID '(' VAL_STRING ',' TOKEN_VECTOR ')' '=' vector4  */
#line 144 "SLParserYacc.y"
                        {
				(yyval.properties) = (yyvsp[-9].properties);
				(yyval.properties)->AddVectorProperty((yyvsp[-7].strval), (yyvsp[-5].strval), *(yyvsp[-8].attributes), *(yyvsp[0].vector4));

				delete[] (yyvsp[-7].strval);
				delete[] (yyvsp[-5].strval);
				delete   (yyvsp[-8].attributes);
				delete   (yyvsp[0].vector4); 
			}
#line 1464 "SLParserYacc.cpp"
    break;

  case 12: /* propitems: propitems propflags VAL_ID '(' VAL_STRING ',' TOKEN_COLOR ')' '=' vector4  */
#line 154 "SLParserYacc.y"
                        {
				(yyval.properties) = (yyvsp[-9].properties);
				(yyval.properties)->AddColorProperty((yyvsp[-7].strval), (yyvsp[-5].strval), *(yyvsp[-8].attributes), *(yyvsp[0].vector4));

				delete[] (yyvsp[-7].strval);
				delete[] (yyvsp[-5].strval);
				delete   (yyvsp[-8].attributes);
				delete   (yyvsp[0].vector4);
			}
#line 1478 "SLParserYacc.cpp"
    break;

  case 13: /* propitems: propitems propflags VAL_ID '(' VAL_STRING ',' VAL_TEX_DIM ')' '=' VAL_STRING  */
#line 164 "SLParserYacc.y"
                        {
				SLPropTexture texture;
				texture.name      = (yyvsp[0].strval);
				texture.dimension = (TextureDimension)(yyvsp[-3].enumval);

				(yyval.properties) = (yyvsp[-9].properties);
				(yyval.properties)->AddTextureProperty((yyvsp[-7].strval), (yyvsp[-5].strval), *(yyvsp[-8].attributes), texture);

				delete   (yyvsp[-8].attributes);
				delete[] (yyvsp[-7].strval);
				delete[] (yyvsp[-5].strval);
				delete[] (yyvsp[0].strval);
			}
#line 1496 "SLParserYacc.cpp"
    break;

  case 14: /* propitems: propitems propflags VAL_ID '(' VAL_STRING ',' VAL_TEX_DIM ')' '=' VAL_STRING '{' '}'  */
#line 178 "SLParserYacc.y"
                        {
				SLPropTexture texture;
				texture.name      = (yyvsp[-2].strval);
				texture.dimension = (TextureDimension)(yyvsp[-5].enumval);

				(yyval.properties) = (yyvsp[-11].properties);
				(yyval.properties)->AddTextureProperty((yyvsp[-9].strval), (yyvsp[-7].strval), *(yyvsp[-10].attributes), texture);

				delete   (yyvsp[-10].attributes);
				delete[] (yyvsp[-9].strval);
				delete[] (yyvsp[-7].strval);
				delete[] (yyvsp[-2].strval);
			}
#line 1514 "SLParserYacc.cpp"
    break;

  case 15: /* propflags: %empty  */
#line 194 "SLParserYacc.y"
                        {
				(yyval.attributes) = new std::vector<SLAttribute>();
			}
#line 1522 "SLParserYacc.cpp"
    break;

  case 16: /* propflags: propflags VAL_BRACKET_ID  */
#line 198 "SLParserYacc.y"
                        {
				(yyval.attributes) = (yyvsp[-1].attributes);
				(yyval.attributes)->emplace_back(std::string((yyvsp[0].strval)));

				delete[] (yyvsp[0].strval);
			}
#line 1533 "SLParserYacc.cpp"
    break;

  case 17: /* propflags: propflags VAL_BRACKET_ATTR  */
#line 205 "SLParserYacc.y"
                        {
				(yyval.attributes) = (yyvsp[-1].attributes);
				// Parse "name(arg1,arg2,...)" format
				std::string full((yyvsp[0].strval));
				size_t parenStart = full.find('(');
				std::string attrName = full.substr(0, parenStart);
				std::string argsStr = full.substr(parenStart + 1, full.size() - parenStart - 2);
				std::vector<std::string> args;
				size_t start = 0;
				size_t pos = 0;
				while ((pos = argsStr.find(',', start)) != std::string::npos)
				{
					std::string arg = argsStr.substr(start, pos - start);
					// trim whitespace
					size_t b = arg.find_first_not_of(" \t");
					size_t e = arg.find_last_not_of(" \t");
					if (b != std::string::npos)
						args.push_back(arg.substr(b, e - b + 1));
					else
						args.push_back("");
					start = pos + 1;
				}
				if (start < argsStr.size())
				{
					std::string arg = argsStr.substr(start);
					size_t b = arg.find_first_not_of(" \t");
					size_t e = arg.find_last_not_of(" \t");
					if (b != std::string::npos)
						args.push_back(arg.substr(b, e - b + 1));
					else
						args.push_back("");
				}
				(yyval.attributes)->emplace_back(attrName, args);

				delete[] (yyvsp[0].strval);
			}
#line 1574 "SLParserYacc.cpp"
    break;

  case 18: /* vector4: '(' VAL_NUMBER ',' VAL_NUMBER ',' VAL_NUMBER ',' VAL_NUMBER ')'  */
#line 244 "SLParserYacc.y"
                        {
				(yyval.vector4) = new SLVector4();
				(yyval.vector4)->SetValue((yyvsp[-7].number), (yyvsp[-5].number), (yyvsp[-3].number), (yyvsp[-1].number));
			}
#line 1583 "SLParserYacc.cpp"
    break;

  case 19: /* vector4: '(' VAL_NUMBER ',' VAL_NUMBER ',' VAL_NUMBER ')'  */
#line 249 "SLParserYacc.y"
                        {
				(yyval.vector4) = new SLVector4();
				(yyval.vector4)->SetValue((yyvsp[-5].number), (yyvsp[-3].number), (yyvsp[-1].number), 1.0f);
			}
#line 1592 "SLParserYacc.cpp"
    break;

  case 20: /* shaderBlocks: %empty  */
#line 256 "SLParserYacc.y"
                        {

			}
#line 1600 "SLParserYacc.cpp"
    break;

  case 21: /* shaderBlocks: shaderBlocks category  */
#line 260 "SLParserYacc.y"
                        {

			}
#line 1608 "SLParserYacc.cpp"
    break;

  case 22: /* shaderBlocks: shaderBlocks subshader  */
#line 264 "SLParserYacc.y"
                        {
				if ((yyvsp[0].subshader)) 
				{
					g_CurrentShader->subShaders.push_back((yyvsp[0].subshader)); 
				}
			}
#line 1619 "SLParserYacc.cpp"
    break;

  case 23: /* shaderBlocks: shaderBlocks fallback  */
#line 271 "SLParserYacc.y"
                        {
				
			}
#line 1627 "SLParserYacc.cpp"
    break;

  case 24: /* shaderBlocks: shaderBlocks VAL_HLSLINCLUDE VAL_PROGRAM_SOURCE  */
#line 275 "SLParserYacc.y"
                        {
				if ((yyvsp[0].strval))
				{
					g_CurrentShader->includeCode = (yyvsp[0].strval);
				}
			}
#line 1638 "SLParserYacc.cpp"
    break;

  case 25: /* shaderBlocks: shaderBlocks VAL_CGINCLUDE VAL_PROGRAM_SOURCE  */
#line 282 "SLParserYacc.y"
                        {
				if ((yyvsp[0].strval))
				{
					g_CurrentShader->includeCode = (yyvsp[0].strval);
				}
			}
#line 1649 "SLParserYacc.cpp"
    break;

  case 26: /* shaderBlocks: shaderBlocks TOKEN_PACKAGE_REQUIREMENTS '{' packageitems '}'  */
#line 289 "SLParserYacc.y"
                        {
			}
#line 1656 "SLParserYacc.cpp"
    break;

  case 27: /* packageitems: %empty  */
#line 294 "SLParserYacc.y"
                        {
			}
#line 1663 "SLParserYacc.cpp"
    break;

  case 28: /* packageitems: packageitems VAL_STRING  */
#line 297 "SLParserYacc.y"
                        {
				g_CurrentShader->packageRequirements.emplace_back((yyvsp[0].strval));
				delete[] (yyvsp[0].strval);
			}
#line 1672 "SLParserYacc.cpp"
    break;

  case 29: /* packageitems: packageitems VAL_STRING '=' VAL_STRING  */
#line 302 "SLParserYacc.y"
                        {
				std::string req = std::string((yyvsp[-2].strval)) + "=" + std::string((yyvsp[0].strval));
				g_CurrentShader->packageRequirements.emplace_back(req);
				delete[] (yyvsp[-2].strval);
				delete[] (yyvsp[0].strval);
			}
#line 1683 "SLParserYacc.cpp"
    break;

  case 30: /* $@3: %empty  */
#line 311 "SLParserYacc.y"
                        {
				PushShaderState();
			}
#line 1691 "SLParserYacc.cpp"
    break;

  case 31: /* category: TOKEN_CATEGORY '{' $@3 states shaderBlocks '}'  */
#line 315 "SLParserYacc.y"
                        {
				PopShaderState();
			}
#line 1699 "SLParserYacc.cpp"
    break;

  case 32: /* $@4: %empty  */
#line 321 "SLParserYacc.y"
                        {
				PushShaderState();
			}
#line 1707 "SLParserYacc.cpp"
    break;

  case 33: /* subshader: TOKEN_SUBSHADER '{' $@4 passes '}'  */
#line 325 "SLParserYacc.y"
                        {
				const SLShaderState* state = g_ShaderStateStack.top();

				(yyval.subshader) = new SLSubShader();
				(yyval.subshader)->tags = state->tags;
				(yyval.subshader)->lod  = state->lod;
				(yyval.subshader)->SetPasses((yyvsp[-1].passes));

				delete (yyvsp[-1].passes);

				PopShaderState();
			}
#line 1724 "SLParserYacc.cpp"
    break;

  case 34: /* fallback: TOKEN_FALLBACK VAL_STRING  */
#line 340 "SLParserYacc.y"
                        {
				g_CurrentShader->fallbackName = (yyvsp[0].strval);
				delete[] (yyvsp[0].strval);
			}
#line 1733 "SLParserYacc.cpp"
    break;

  case 35: /* fallback: TOKEN_FALLBACK VAL_STRING ',' VAL_NUMBER  */
#line 345 "SLParserYacc.y"
                        {
				g_CurrentShader->fallbackName = (yyvsp[-2].strval);
				delete[] (yyvsp[-2].strval);
			}
#line 1742 "SLParserYacc.cpp"
    break;

  case 36: /* fallback: TOKEN_FALLBACK TOKEN_FALSE  */
#line 350 "SLParserYacc.y"
                        {
				g_CurrentShader->fallbackName = "";
			}
#line 1750 "SLParserYacc.cpp"
    break;

  case 37: /* passes: states pass  */
#line 356 "SLParserYacc.y"
                        {
				(yyval.passes) = new std::vector<SLPassBase*>(); 
				(yyval.passes)->push_back((yyvsp[0].pass)); 
			}
#line 1759 "SLParserYacc.cpp"
    break;

  case 38: /* passes: passes states pass  */
#line 361 "SLParserYacc.y"
                        {
				(yyval.passes) = (yyvsp[-2].passes);  
				(yyval.passes)->push_back((yyvsp[0].pass));
			}
#line 1768 "SLParserYacc.cpp"
    break;

  case 39: /* passes: states grabpass  */
#line 366 "SLParserYacc.y"
                        {
				(yyval.passes) = new std::vector<SLPassBase*>();
				(yyval.passes)->push_back((yyvsp[0].pass));
			}
#line 1777 "SLParserYacc.cpp"
    break;

  case 40: /* passes: passes states grabpass  */
#line 371 "SLParserYacc.y"
                        {
				(yyval.passes) = (yyvsp[-2].passes);
				(yyval.passes)->push_back((yyvsp[0].pass));
			}
#line 1786 "SLParserYacc.cpp"
    break;

  case 41: /* grabpass: TOKEN_GRABPASS '{' '}'  */
#line 378 "SLParserYacc.y"
                        {
				(yyval.pass) = new SLGrabPass();
			}
#line 1794 "SLParserYacc.cpp"
    break;

  case 42: /* grabpass: TOKEN_GRABPASS '{' VAL_STRING '}'  */
#line 382 "SLParserYacc.y"
                        {
				(yyval.pass) = new SLGrabPass((yyvsp[-1].strval));
				delete[] (yyvsp[-1].strval);
			}
#line 1803 "SLParserYacc.cpp"
    break;

  case 43: /* $@5: %empty  */
#line 389 "SLParserYacc.y"
                        {
				PushShaderState();
			}
#line 1811 "SLParserYacc.cpp"
    break;

  case 44: /* pass: TOKEN_PASS '{' $@5 states '}'  */
#line 393 "SLParserYacc.y"
                        {
				SLNormalPass* pp = new SLNormalPass(*g_ShaderStateStack.top());
				// Collect all programs accumulated during this pass
				if (g_CurrentProgramCode != nullptr)
				{
					SLProgram prog;
					prog.type   = g_CurrentProgramType;
					prog.source = g_CurrentProgramCode;
					prog.lineNo = g_CurrentProgramLine;
					pp->programs.push_back(prog);
				}

				(yyval.pass) = pp;
				
				g_CurrentProgramType = kCG;
				g_CurrentProgramCode = nullptr;
				g_CurrentProgramLine = 0;
				
				PopShaderState();
			}
#line 1836 "SLParserYacc.cpp"
    break;

  case 45: /* pass: TOKEN_USEPASS VAL_STRING  */
#line 414 "SLParserYacc.y"
                        {
				(yyval.pass) = new SLUsePass((yyvsp[0].strval)); 
				delete[] (yyvsp[0].strval);
			}
#line 1845 "SLParserYacc.cpp"
    break;

  case 46: /* states: %empty  */
#line 421 "SLParserYacc.y"
                        {
				(yyval.states) = g_ShaderStateStack.top();
			}
#line 1853 "SLParserYacc.cpp"
    break;

  case 47: /* states: states TOKEN_LOD VAL_NUMBER  */
#line 425 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states); 
				(yyval.states)->lod = int32_t((yyvsp[0].number));
			}
#line 1862 "SLParserYacc.cpp"
    break;

  case 48: /* states: states TOKEN_NAME VAL_STRING  */
#line 430 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states); 
				(yyval.states)->name = (yyvsp[0].strval);

				delete[] (yyvsp[0].strval);
			}
#line 1873 "SLParserYacc.cpp"
    break;

  case 49: /* states: states TOKEN_TAGS '{' tags '}'  */
#line 437 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-4].states); 
				(yyval.states)->AddTags(*(yyvsp[-1].tags));

				delete (yyvsp[-1].tags);
			}
#line 1884 "SLParserYacc.cpp"
    break;

  case 50: /* states: states TOKEN_ALPHA_TO_MASK boolean  */
#line 444 "SLParserYacc.y"
                        {	
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->alphaToMask = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 1895 "SLParserYacc.cpp"
    break;

  case 51: /* states: states TOKEN_ZTEST funcval  */
#line 451 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->zTest = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 1906 "SLParserYacc.cpp"
    break;

  case 52: /* states: states TOKEN_ZTEST TOKEN_TRUE  */
#line 458 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states); 
				(yyval.states)->zTest.SetValue(kFuncLEqual);
			}
#line 1915 "SLParserYacc.cpp"
    break;

  case 53: /* states: states TOKEN_ZTEST TOKEN_FALSE  */
#line 463 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states); 
				(yyval.states)->zTest.SetValue(kFuncDisabled);
			}
#line 1924 "SLParserYacc.cpp"
    break;

  case 54: /* states: states TOKEN_ZWRITE boolean  */
#line 468 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->zWrite = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 1935 "SLParserYacc.cpp"
    break;

  case 55: /* states: states TOKEN_COLORMASK maskval  */
#line 475 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->colMask = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 1946 "SLParserYacc.cpp"
    break;

  case 56: /* states: states TOKEN_CULL cullval  */
#line 482 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->culling = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 1957 "SLParserYacc.cpp"
    break;

  case 57: /* states: states TOKEN_CULL TOKEN_FALSE  */
#line 489 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states); 
				(yyval.states)->culling.SetValue(kCullOff);
			}
#line 1966 "SLParserYacc.cpp"
    break;

  case 58: /* states: states TOKEN_BLEND blendval blendval ',' blendval blendval  */
#line 494 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-6].states);

				(yyval.states)->srcBlend       = *(yyvsp[-4].floatval);
				(yyval.states)->destBlend      = *(yyvsp[-3].floatval);
				(yyval.states)->srcBlendAlpha  = *(yyvsp[-1].floatval);
				(yyval.states)->destBlendAlpha = *(yyvsp[0].floatval);

				delete (yyvsp[-4].floatval);
				delete (yyvsp[-3].floatval);
				delete (yyvsp[-1].floatval);
				delete (yyvsp[0].floatval);
			}
#line 1984 "SLParserYacc.cpp"
    break;

  case 59: /* states: states TOKEN_BLEND blendval blendval  */
#line 508 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-3].states); 

				(yyval.states)->srcBlend       = *(yyvsp[-1].floatval);
				(yyval.states)->destBlend      = *(yyvsp[0].floatval);
				(yyval.states)->srcBlendAlpha  = *(yyvsp[-1].floatval);
				(yyval.states)->destBlendAlpha = *(yyvsp[0].floatval);

				delete (yyvsp[-1].floatval);
				delete (yyvsp[0].floatval);
			}
#line 2000 "SLParserYacc.cpp"
    break;

  case 60: /* states: states TOKEN_BLEND TOKEN_FALSE  */
#line 520 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states); 
				(yyval.states)->srcBlend.SetValue(kBlendOne); 
				(yyval.states)->srcBlendAlpha.SetValue(kBlendOne); 
				(yyval.states)->destBlend.SetValue(kBlendZero); 
				(yyval.states)->destBlendAlpha.SetValue(kBlendZero);
			}
#line 2012 "SLParserYacc.cpp"
    break;

  case 61: /* states: states TOKEN_BLEND_OP blendopval ',' blendopval  */
#line 528 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-4].states);

				(yyval.states)->blendOp      = *(yyvsp[-2].floatval);
				(yyval.states)->blendOpAlpha = *(yyvsp[0].floatval);

				delete (yyvsp[-2].floatval);
				delete (yyvsp[0].floatval);
			}
#line 2026 "SLParserYacc.cpp"
    break;

  case 62: /* states: states TOKEN_BLEND_OP blendopval  */
#line 538 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				
				(yyval.states)->blendOp      = *(yyvsp[0].floatval);
				(yyval.states)->blendOpAlpha = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2039 "SLParserYacc.cpp"
    break;

  case 63: /* states: states TOKEN_STENCIL '{' stencilblock '}'  */
#line 547 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-4].states);

				(yyval.states)->stencilRef       = (yyvsp[-1].states)->stencilRef;
				(yyval.states)->stencilReadMask  = (yyvsp[-1].states)->stencilReadMask;
				(yyval.states)->stencilWriteMask = (yyvsp[-1].states)->stencilWriteMask;
				(yyval.states)->stencilOp 		 = (yyvsp[-1].states)->stencilOp;
				(yyval.states)->stencilOpFront   = (yyvsp[-1].states)->stencilOpFront;
				(yyval.states)->stencilOpBack    = (yyvsp[-1].states)->stencilOpBack;

				PopShaderState();
			}
#line 2056 "SLParserYacc.cpp"
    break;

  case 64: /* states: states TOKEN_OFFSET numval ',' numval  */
#line 560 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-4].states);

				(yyval.states)->offsetFactor = *(yyvsp[-2].floatval);
				(yyval.states)->offsetUnits  = *(yyvsp[0].floatval);

				delete (yyvsp[-2].floatval);
				delete (yyvsp[0].floatval);
			}
#line 2070 "SLParserYacc.cpp"
    break;

  case 65: /* states: states TOKEN_ZCLIP boolean  */
#line 570 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->zClip = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2081 "SLParserYacc.cpp"
    break;

  case 66: /* states: states TOKEN_CONSERVATIVE boolean  */
#line 577 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->conservative = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2092 "SLParserYacc.cpp"
    break;

  case 67: /* states: states TOKEN_BLEND VAL_NUMBER blendval blendval ',' blendval blendval  */
#line 584 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-7].states);
				int32_t idx = int32_t((yyvsp[-5].number));
				if (idx >= 0 && idx < kMaxRTs)
				{
					(yyval.states)->mrtSrcBlend[idx]       = *(yyvsp[-4].floatval);
					(yyval.states)->mrtDestBlend[idx]      = *(yyvsp[-3].floatval);
					(yyval.states)->mrtSrcBlendAlpha[idx]  = *(yyvsp[-1].floatval);
					(yyval.states)->mrtDestBlendAlpha[idx] = *(yyvsp[0].floatval);
				}

				delete (yyvsp[-4].floatval);
				delete (yyvsp[-3].floatval);
				delete (yyvsp[-1].floatval);
				delete (yyvsp[0].floatval);
			}
#line 2113 "SLParserYacc.cpp"
    break;

  case 68: /* states: states TOKEN_BLEND VAL_NUMBER blendval blendval  */
#line 601 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-4].states);
				int32_t idx = int32_t((yyvsp[-2].number));
				if (idx >= 0 && idx < kMaxRTs)
				{
					(yyval.states)->mrtSrcBlend[idx]       = *(yyvsp[-1].floatval);
					(yyval.states)->mrtDestBlend[idx]      = *(yyvsp[0].floatval);
					(yyval.states)->mrtSrcBlendAlpha[idx]  = *(yyvsp[-1].floatval);
					(yyval.states)->mrtDestBlendAlpha[idx] = *(yyvsp[0].floatval);
				}

				delete (yyvsp[-1].floatval);
				delete (yyvsp[0].floatval);
			}
#line 2132 "SLParserYacc.cpp"
    break;

  case 69: /* states: states TOKEN_COLORMASK maskval VAL_NUMBER  */
#line 616 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-3].states);
				int32_t idx = int32_t((yyvsp[0].number));
				if (idx >= 0 && idx < kMaxRTs)
				{
					(yyval.states)->mrtColMask[idx] = *(yyvsp[-1].floatval);
				}

				delete (yyvsp[-1].floatval);
			}
#line 2147 "SLParserYacc.cpp"
    break;

  case 70: /* states: states VAL_HLSLPROGRAM VAL_PROGRAM_SOURCE  */
#line 627 "SLParserYacc.y"
                        {
				g_CurrentProgramCode = (yyvsp[0].strval);
				g_CurrentProgramLine = (yyvsp[-1].number);
				g_CurrentProgramType = kHLSL;
			}
#line 2157 "SLParserYacc.cpp"
    break;

  case 71: /* states: states VAL_GLSLPROGRAM VAL_PROGRAM_SOURCE  */
#line 633 "SLParserYacc.y"
                        {
				g_CurrentProgramCode = (yyvsp[0].strval);
				g_CurrentProgramLine = (yyvsp[-1].number);
				g_CurrentProgramType = kGLSL;
			}
#line 2167 "SLParserYacc.cpp"
    break;

  case 72: /* states: states VAL_CGPROGRAM VAL_PROGRAM_SOURCE  */
#line 639 "SLParserYacc.y"
                        {
				g_CurrentProgramCode = (yyvsp[0].strval);
				g_CurrentProgramLine = (yyvsp[-1].number);
				g_CurrentProgramType = kCG;
			}
#line 2177 "SLParserYacc.cpp"
    break;

  case 73: /* cullval: VAL_BRACKET_ID  */
#line 647 "SLParserYacc.y"
                        {
				(yyval.floatval) = new SLFloat(); 
				(yyval.floatval)->SetRef((yyvsp[0].strval));

				delete[] (yyvsp[0].strval);
			}
#line 2188 "SLParserYacc.cpp"
    break;

  case 74: /* cullval: VAL_TRIANGLE_FACE  */
#line 654 "SLParserYacc.y"
                        {
				(yyval.floatval) = new SLFloat();
				(yyval.floatval)->SetValue((yyvsp[0].enumval));
			}
#line 2197 "SLParserYacc.cpp"
    break;

  case 75: /* boolean: VAL_BRACKET_ID  */
#line 661 "SLParserYacc.y"
                        {
				(yyval.floatval) = new SLFloat(); 
				(yyval.floatval)->SetRef((yyvsp[0].strval));

				delete[] (yyvsp[0].strval);
			}
#line 2208 "SLParserYacc.cpp"
    break;

  case 76: /* boolean: TOKEN_TRUE  */
#line 668 "SLParserYacc.y"
                        {
				(yyval.floatval) = new SLFloat(); 
				(yyval.floatval)->SetValue(1);
			}
#line 2217 "SLParserYacc.cpp"
    break;

  case 77: /* boolean: TOKEN_FALSE  */
#line 673 "SLParserYacc.y"
                        {
				(yyval.floatval) = new SLFloat(); 
				(yyval.floatval)->SetValue(0);
			}
#line 2226 "SLParserYacc.cpp"
    break;

  case 78: /* tags: %empty  */
#line 680 "SLParserYacc.y"
                        {
				(yyval.tags) = new std::map<std::string, std::string>();
			}
#line 2234 "SLParserYacc.cpp"
    break;

  case 79: /* tags: tags VAL_ID '=' VAL_ID  */
#line 684 "SLParserYacc.y"
                        {
				std::string key((yyvsp[-2].strval));
				std::string value((yyvsp[0].strval));

				(yyval.tags) = (yyvsp[-3].tags);
				(*(yyval.tags))[key] = value;

				delete[] (yyvsp[-2].strval);
				delete[] (yyvsp[0].strval);
			}
#line 2249 "SLParserYacc.cpp"
    break;

  case 80: /* tags: tags VAL_STRING '=' VAL_STRING  */
#line 695 "SLParserYacc.y"
                        {
				std::string key((yyvsp[-2].strval));
				std::string value((yyvsp[0].strval));

				(yyval.tags) = (yyvsp[-3].tags);
				(*(yyval.tags))[key] = value;

				delete[] (yyvsp[-2].strval);
				delete[] (yyvsp[0].strval);
			}
#line 2264 "SLParserYacc.cpp"
    break;

  case 81: /* stencilblock: %empty  */
#line 708 "SLParserYacc.y"
                        {
				PushShaderState(); 
				(yyval.states) = g_ShaderStateStack.top();
			}
#line 2273 "SLParserYacc.cpp"
    break;

  case 82: /* stencilblock: stencilblock TOKEN_REF numval  */
#line 713 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->stencilRef = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2284 "SLParserYacc.cpp"
    break;

  case 83: /* stencilblock: stencilblock TOKEN_READ_MASK numval  */
#line 720 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states); 
				(yyval.states)->stencilReadMask = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2295 "SLParserYacc.cpp"
    break;

  case 84: /* stencilblock: stencilblock TOKEN_WRITE_MASK numval  */
#line 727 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->stencilWriteMask = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2306 "SLParserYacc.cpp"
    break;

  case 85: /* stencilblock: stencilblock TOKEN_COMP funcval  */
#line 734 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->stencilOp.comp = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2317 "SLParserYacc.cpp"
    break;

  case 86: /* stencilblock: stencilblock TOKEN_OP_PASS stencilaction  */
#line 741 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->stencilOp.pass = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2328 "SLParserYacc.cpp"
    break;

  case 87: /* stencilblock: stencilblock TOKEN_OP_FAIL stencilaction  */
#line 748 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->stencilOp.fail = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2339 "SLParserYacc.cpp"
    break;

  case 88: /* stencilblock: stencilblock TOKEN_OP_ZFAIL stencilaction  */
#line 755 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->stencilOp.zFail = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2350 "SLParserYacc.cpp"
    break;

  case 89: /* stencilblock: stencilblock TOKEN_COMP_FRONT funcval  */
#line 762 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->stencilOpFront.comp = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2361 "SLParserYacc.cpp"
    break;

  case 90: /* stencilblock: stencilblock TOKEN_OP_PASS_FRONT stencilaction  */
#line 769 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->stencilOpFront.pass = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2372 "SLParserYacc.cpp"
    break;

  case 91: /* stencilblock: stencilblock TOKEN_OP_FAIL_FRONT stencilaction  */
#line 776 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->stencilOpFront.fail = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2383 "SLParserYacc.cpp"
    break;

  case 92: /* stencilblock: stencilblock TOKEN_OP_ZFAIL_FRONT stencilaction  */
#line 783 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->stencilOpFront.zFail = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2394 "SLParserYacc.cpp"
    break;

  case 93: /* stencilblock: stencilblock TOKEN_COMP_BACK funcval  */
#line 790 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->stencilOpBack.comp = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2405 "SLParserYacc.cpp"
    break;

  case 94: /* stencilblock: stencilblock TOKEN_OP_PASS_BACK stencilaction  */
#line 797 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->stencilOpBack.pass = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2416 "SLParserYacc.cpp"
    break;

  case 95: /* stencilblock: stencilblock TOKEN_OP_FAIL_BACK stencilaction  */
#line 804 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->stencilOpBack.fail = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2427 "SLParserYacc.cpp"
    break;

  case 96: /* stencilblock: stencilblock TOKEN_OP_ZFAIL_BACK stencilaction  */
#line 811 "SLParserYacc.y"
                        {
				(yyval.states) = (yyvsp[-2].states);
				(yyval.states)->stencilOpBack.zFail = *(yyvsp[0].floatval);

				delete (yyvsp[0].floatval);
			}
#line 2438 "SLParserYacc.cpp"
    break;

  case 97: /* stencilaction: VAL_BRACKET_ID  */
#line 820 "SLParserYacc.y"
                        {
				(yyval.floatval) = new SLFloat(); 
				(yyval.floatval)->SetRef((yyvsp[0].strval));

				delete[] (yyvsp[0].strval);
			}
#line 2449 "SLParserYacc.cpp"
    break;

  case 98: /* stencilaction: VAL_STENCIL_ACTION  */
#line 827 "SLParserYacc.y"
                        {
				(yyval.floatval) = new SLFloat();
				(yyval.floatval)->SetValue ((yyvsp[0].enumval));
			}
#line 2458 "SLParserYacc.cpp"
    break;

  case 99: /* numval: VAL_NUMBER  */
#line 834 "SLParserYacc.y"
                        {
				(yyval.floatval) = new SLFloat(); 
				(yyval.floatval)->SetValue((yyvsp[0].number));
			}
#line 2467 "SLParserYacc.cpp"
    break;

  case 100: /* numval: VAL_BRACKET_ID  */
#line 839 "SLParserYacc.y"
                        {
				(yyval.floatval) = new SLFloat();
				(yyval.floatval)->SetRef((yyvsp[0].strval));

				delete[] (yyvsp[0].strval);
			}
#line 2478 "SLParserYacc.cpp"
    break;

  case 101: /* blendval: VAL_BRACKET_ID  */
#line 848 "SLParserYacc.y"
                        {
				(yyval.floatval) = new SLFloat(); 
				(yyval.floatval)->SetRef((yyvsp[0].strval));

				delete[] (yyvsp[0].strval);
			}
#line 2489 "SLParserYacc.cpp"
    break;

  case 102: /* blendval: VAL_BLEND_FACTOR  */
#line 855 "SLParserYacc.y"
                        { 
				(yyval.floatval) = new SLFloat(); 
				(yyval.floatval)->SetValue((yyvsp[0].enumval));
			}
#line 2498 "SLParserYacc.cpp"
    break;

  case 103: /* blendopval: VAL_BRACKET_ID  */
#line 862 "SLParserYacc.y"
                        {
				(yyval.floatval) = new SLFloat();
				(yyval.floatval)->SetRef((yyvsp[0].strval));

				delete[] (yyvsp[0].strval);
			}
#line 2509 "SLParserYacc.cpp"
    break;

  case 104: /* blendopval: VAL_BLEND_OP_MODE  */
#line 869 "SLParserYacc.y"
                        {
				(yyval.floatval) = new SLFloat();
				(yyval.floatval)->SetValue((yyvsp[0].enumval));
			}
#line 2518 "SLParserYacc.cpp"
    break;

  case 105: /* maskval: VAL_BRACKET_ID  */
#line 876 "SLParserYacc.y"
                        {
				(yyval.floatval) = new SLFloat();
				(yyval.floatval)->SetRef((yyvsp[0].strval));

				delete[] (yyvsp[0].strval);
			}
#line 2529 "SLParserYacc.cpp"
    break;

  case 106: /* maskval: VAL_RGBA_MASK  */
#line 883 "SLParserYacc.y"
                        {
				(yyval.floatval) = new SLFloat();
				(yyval.floatval)->SetValue((yyvsp[0].enumval));
			}
#line 2538 "SLParserYacc.cpp"
    break;

  case 107: /* maskval: TOKEN_FALSE  */
#line 888 "SLParserYacc.y"
                        {
				(yyval.floatval) = new SLFloat();
				(yyval.floatval)->SetValue(0);
			}
#line 2547 "SLParserYacc.cpp"
    break;

  case 108: /* funcval: VAL_BRACKET_ID  */
#line 895 "SLParserYacc.y"
                        {
				(yyval.floatval) = new SLFloat();
				(yyval.floatval)->SetRef((yyvsp[0].strval));

				delete[] (yyvsp[0].strval);
			}
#line 2558 "SLParserYacc.cpp"
    break;

  case 109: /* funcval: VAL_COMPARE_MODE  */
#line 902 "SLParserYacc.y"
                        {
				(yyval.floatval) = new SLFloat();
				(yyval.floatval)->SetValue((yyvsp[0].enumval));
			}
#line 2567 "SLParserYacc.cpp"
    break;


#line 2571 "SLParserYacc.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 908 "SLParserYacc.y"


int32_t shaderlaberror(const char *s)
{
    printf("shaderlaberror line %d %s\n", yylineno, s);
	return 0;
}

static void PushShaderState()
{
	SLShaderState* s = new SLShaderState(*g_ShaderStateStack.top());
	g_ShaderStateStack.push(s);
}

static void PopShaderState()
{
	SLShaderState* s = g_ShaderStateStack.top();
	g_ShaderStateStack.pop();
	delete s;
}
