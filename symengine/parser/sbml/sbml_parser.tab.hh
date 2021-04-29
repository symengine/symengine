/* A Bison parser, made by GNU Bison 3.7.5.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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

#ifndef YY_SBML_SBML_PARSER_TAB_HH_INCLUDED
# define YY_SBML_SBML_PARSER_TAB_HH_INCLUDED
/* Debug traces.  */
#ifndef SBMLDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define SBMLDEBUG 1
#  else
#   define SBMLDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define SBMLDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined SBMLDEBUG */
#if SBMLDEBUG
extern int sbmldebug;
#endif
/* "%code requires" blocks.  */
#line 16 "sbml_parser.yy"


#include "symengine/parser/sbml/sbml_parser.h"


#line 63 "sbml_parser.tab.hh"

/* Token kinds.  */
#ifndef SBMLTOKENTYPE
# define SBMLTOKENTYPE
  enum sbmltokentype
  {
    SBMLEMPTY = -2,
    END_OF_FILE = 0,               /* END_OF_FILE  */
    SBMLerror = 256,               /* error  */
    SBMLUNDEF = 257,               /* "invalid token"  */
    IDENTIFIER = 258,              /* IDENTIFIER  */
    NUMERIC = 259,                 /* NUMERIC  */
    AND = 260,                     /* AND  */
    OR = 261,                      /* OR  */
    EQ = 262,                      /* EQ  */
    LE = 263,                      /* LE  */
    GE = 264,                      /* GE  */
    NE = 265,                      /* NE  */
    UMINUS = 266,                  /* UMINUS  */
    UPLUS = 267                    /* UPLUS  */
  };
  typedef enum sbmltokentype sbmltoken_kind_t;
#endif

/* Value type.  */
#if ! defined SBMLSTYPE && ! defined SBMLSTYPE_IS_DECLARED
typedef struct SymEngine::SBMLSTYPE SBMLSTYPE;
# define SBMLSTYPE_IS_TRIVIAL 1
# define SBMLSTYPE_IS_DECLARED 1
#endif



int sbmlparse (SymEngine::SbmlParser &p);

#endif /* !YY_SBML_SBML_PARSER_TAB_HH_INCLUDED  */
