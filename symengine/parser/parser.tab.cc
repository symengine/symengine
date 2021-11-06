// A Bison parser, made by GNU Bison 3.7.5.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "parser.tab.hh"


// Unqualified %code blocks.
#line 22 "parser.yy"

#include "symengine/pow.h"
#include "symengine/logic.h"
#include "symengine/parser/parser.h"

using SymEngine::RCP;
using SymEngine::Basic;
using SymEngine::vec_basic;
using SymEngine::rcp_static_cast;
using SymEngine::mul;
using SymEngine::pow;
using SymEngine::add;
using SymEngine::sub;
using SymEngine::Lt;
using SymEngine::Gt;
using SymEngine::Le;
using SymEngine::Ge;
using SymEngine::Ne;
using SymEngine::Eq;
using SymEngine::set_boolean;
using SymEngine::Boolean;
using SymEngine::one;
using SymEngine::vec_boolean;

#include "symengine/parser/tokenizer.h"

namespace yy
{

int yylex(yy::parser::semantic_type* yylval, SymEngine::Parser &p)
{
    return p.m_tokenizer->lex(yylval);
}

void parser::error(const std::string &msg)
{
    throw SymEngine::ParseError(msg);
}

}


#line 89 "parser.tab.cc"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif



// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace yy {
#line 162 "parser.tab.cc"

  /// Build a parser object.
  parser::parser (SymEngine::Parser &p_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      p (p_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | symbol kinds.  |
  `---------------*/

  // basic_symbol.
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
  {
    switch (this->kind ())
    {
      case symbol_kind::S_st_expr: // st_expr
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_leaf: // leaf
      case symbol_kind::S_func: // func
        value.copy< SymEngine::RCP<const SymEngine::Basic> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.copy< SymEngine::vec_basic > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMERIC: // NUMERIC
      case symbol_kind::S_IMPLICIT_MUL: // IMPLICIT_MUL
        value.copy< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }



  template <typename Base>
  parser::symbol_kind_type
  parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }

  template <typename Base>
  bool
  parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
    {
      case symbol_kind::S_st_expr: // st_expr
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_leaf: // leaf
      case symbol_kind::S_func: // func
        value.move< SymEngine::RCP<const SymEngine::Basic> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.move< SymEngine::vec_basic > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMERIC: // NUMERIC
      case symbol_kind::S_IMPLICIT_MUL: // IMPLICIT_MUL
        value.move< std::string > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

  }

  // by_kind.
  parser::by_kind::by_kind ()
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  parser::by_kind::by_kind (by_kind&& that)
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  parser::by_kind::by_kind (const by_kind& that)
    : kind_ (that.kind_)
  {}

  parser::by_kind::by_kind (token_kind_type t)
    : kind_ (yytranslate_ (t))
  {}

  void
  parser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  parser::symbol_kind_type
  parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }

  parser::symbol_kind_type
  parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_kind_type
  parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_st_expr: // st_expr
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_leaf: // leaf
      case symbol_kind::S_func: // func
        value.YY_MOVE_OR_COPY< SymEngine::RCP<const SymEngine::Basic> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.YY_MOVE_OR_COPY< SymEngine::vec_basic > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMERIC: // NUMERIC
      case symbol_kind::S_IMPLICIT_MUL: // IMPLICIT_MUL
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s)
  {
    switch (that.kind ())
    {
      case symbol_kind::S_st_expr: // st_expr
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_leaf: // leaf
      case symbol_kind::S_func: // func
        value.move< SymEngine::RCP<const SymEngine::Basic> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.move< SymEngine::vec_basic > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMERIC: // NUMERIC
      case symbol_kind::S_IMPLICIT_MUL: // IMPLICIT_MUL
        value.move< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_st_expr: // st_expr
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_leaf: // leaf
      case symbol_kind::S_func: // func
        value.copy< SymEngine::RCP<const SymEngine::Basic> > (that.value);
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.copy< SymEngine::vec_basic > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMERIC: // NUMERIC
      case symbol_kind::S_IMPLICIT_MUL: // IMPLICIT_MUL
        value.copy< std::string > (that.value);
        break;

      default:
        break;
    }

    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_st_expr: // st_expr
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_leaf: // leaf
      case symbol_kind::S_func: // func
        value.move< SymEngine::RCP<const SymEngine::Basic> > (that.value);
        break;

      case symbol_kind::S_expr_list: // expr_list
        value.move< SymEngine::vec_basic > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMERIC: // NUMERIC
      case symbol_kind::S_IMPLICIT_MUL: // IMPLICIT_MUL
        value.move< std::string > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " (";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.kind_ = yytranslate_ (yylex (&yyla.value, p));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_st_expr: // st_expr
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_leaf: // leaf
      case symbol_kind::S_func: // func
        yylhs.value.emplace< SymEngine::RCP<const SymEngine::Basic> > ();
        break;

      case symbol_kind::S_expr_list: // expr_list
        yylhs.value.emplace< SymEngine::vec_basic > ();
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMERIC: // NUMERIC
      case symbol_kind::S_IMPLICIT_MUL: // IMPLICIT_MUL
        yylhs.value.emplace< std::string > ();
        break;

      default:
        break;
    }



      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // st_expr: expr
#line 98 "parser.yy"
    {
        yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ();
        p.res = yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > ();
    }
#line 753 "parser.tab.cc"
    break;

  case 3: // expr: expr '+' expr
#line 106 "parser.yy"
        { yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = add(yystack_[2].value.as < SymEngine::RCP<const SymEngine::Basic> > (), yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ()); }
#line 759 "parser.tab.cc"
    break;

  case 4: // expr: expr '-' expr
#line 109 "parser.yy"
        { yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = sub(yystack_[2].value.as < SymEngine::RCP<const SymEngine::Basic> > (), yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ()); }
#line 765 "parser.tab.cc"
    break;

  case 5: // expr: expr '*' expr
#line 112 "parser.yy"
        { yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = mul(yystack_[2].value.as < SymEngine::RCP<const SymEngine::Basic> > (), yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ()); }
#line 771 "parser.tab.cc"
    break;

  case 6: // expr: expr '/' expr
#line 115 "parser.yy"
        { yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = div(yystack_[2].value.as < SymEngine::RCP<const SymEngine::Basic> > (), yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ()); }
#line 777 "parser.tab.cc"
    break;

  case 7: // expr: IMPLICIT_MUL POW expr
#line 120 "parser.yy"
        {
          auto tup = p.parse_implicit_mul(yystack_[2].value.as < std::string > ());
          if (neq(*std::get<1>(tup), *one)) {
            yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = mul(std::get<0>(tup), pow(std::get<1>(tup), yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ()));
          } else {
            yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = pow(std::get<0>(tup), yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ());
          }
        }
#line 790 "parser.tab.cc"
    break;

  case 8: // expr: expr POW expr
#line 130 "parser.yy"
        { yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = pow(yystack_[2].value.as < SymEngine::RCP<const SymEngine::Basic> > (), yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ()); }
#line 796 "parser.tab.cc"
    break;

  case 9: // expr: expr '<' expr
#line 133 "parser.yy"
        { yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = rcp_static_cast<const Basic>(Lt(yystack_[2].value.as < SymEngine::RCP<const SymEngine::Basic> > (), yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ())); }
#line 802 "parser.tab.cc"
    break;

  case 10: // expr: expr '>' expr
#line 136 "parser.yy"
        { yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = rcp_static_cast<const Basic>(Gt(yystack_[2].value.as < SymEngine::RCP<const SymEngine::Basic> > (), yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ())); }
#line 808 "parser.tab.cc"
    break;

  case 11: // expr: expr NE expr
#line 139 "parser.yy"
        { yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = rcp_static_cast<const Basic>(Ne(yystack_[2].value.as < SymEngine::RCP<const SymEngine::Basic> > (), yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ())); }
#line 814 "parser.tab.cc"
    break;

  case 12: // expr: expr LE expr
#line 142 "parser.yy"
        { yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = rcp_static_cast<const Basic>(Le(yystack_[2].value.as < SymEngine::RCP<const SymEngine::Basic> > (), yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ())); }
#line 820 "parser.tab.cc"
    break;

  case 13: // expr: expr GE expr
#line 145 "parser.yy"
        { yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = rcp_static_cast<const Basic>(Ge(yystack_[2].value.as < SymEngine::RCP<const SymEngine::Basic> > (), yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ())); }
#line 826 "parser.tab.cc"
    break;

  case 14: // expr: expr EQ expr
#line 148 "parser.yy"
        { yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = rcp_static_cast<const Basic>(Eq(yystack_[2].value.as < SymEngine::RCP<const SymEngine::Basic> > (), yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ())); }
#line 832 "parser.tab.cc"
    break;

  case 15: // expr: expr '|' expr
#line 151 "parser.yy"
        {
            set_boolean s;
            s.insert(rcp_static_cast<const Boolean>(yystack_[2].value.as < SymEngine::RCP<const SymEngine::Basic> > ()));
            s.insert(rcp_static_cast<const Boolean>(yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ()));
            yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = rcp_static_cast<const Basic>(logical_or(s));
        }
#line 843 "parser.tab.cc"
    break;

  case 16: // expr: expr '&' expr
#line 159 "parser.yy"
        {
            set_boolean s;
            s.insert(rcp_static_cast<const Boolean>(yystack_[2].value.as < SymEngine::RCP<const SymEngine::Basic> > ()));
            s.insert(rcp_static_cast<const Boolean>(yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ()));
            yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = rcp_static_cast<const Basic>(logical_and(s));
        }
#line 854 "parser.tab.cc"
    break;

  case 17: // expr: expr '^' expr
#line 167 "parser.yy"
        {
            vec_boolean s;
            s.push_back(rcp_static_cast<const Boolean>(yystack_[2].value.as < SymEngine::RCP<const SymEngine::Basic> > ()));
            s.push_back(rcp_static_cast<const Boolean>(yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ()));
            yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = rcp_static_cast<const Basic>(logical_xor(s));
        }
#line 865 "parser.tab.cc"
    break;

  case 18: // expr: '(' expr ')'
#line 175 "parser.yy"
        { yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = yystack_[1].value.as < SymEngine::RCP<const SymEngine::Basic> > (); }
#line 871 "parser.tab.cc"
    break;

  case 19: // expr: '-' expr
#line 178 "parser.yy"
        { yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = neg(yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ()); }
#line 877 "parser.tab.cc"
    break;

  case 20: // expr: '+' expr
#line 181 "parser.yy"
        { yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > (); }
#line 883 "parser.tab.cc"
    break;

  case 21: // expr: '~' expr
#line 184 "parser.yy"
        { yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = rcp_static_cast<const Basic>(logical_not(rcp_static_cast<const Boolean>(yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ()))); }
#line 889 "parser.tab.cc"
    break;

  case 22: // expr: leaf
#line 187 "parser.yy"
        { yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = rcp_static_cast<const Basic>(yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ()); }
#line 895 "parser.tab.cc"
    break;

  case 23: // leaf: IDENTIFIER
#line 192 "parser.yy"
    {
        yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = p.parse_identifier(yystack_[0].value.as < std::string > ());
    }
#line 903 "parser.tab.cc"
    break;

  case 24: // leaf: IMPLICIT_MUL
#line 197 "parser.yy"
    {
        auto tup = p.parse_implicit_mul(yystack_[0].value.as < std::string > ());
        yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = mul(std::get<0>(tup), std::get<1>(tup));
    }
#line 912 "parser.tab.cc"
    break;

  case 25: // leaf: NUMERIC
#line 203 "parser.yy"
    {
        yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = p.parse_numeric(yystack_[0].value.as < std::string > ());
    }
#line 920 "parser.tab.cc"
    break;

  case 26: // leaf: func
#line 208 "parser.yy"
    {
        yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ();
    }
#line 928 "parser.tab.cc"
    break;

  case 27: // func: IDENTIFIER '(' expr_list ')'
#line 215 "parser.yy"
    {
        yylhs.value.as < SymEngine::RCP<const SymEngine::Basic> > () = p.functionify(yystack_[3].value.as < std::string > (), yystack_[1].value.as < SymEngine::vec_basic > ());
    }
#line 936 "parser.tab.cc"
    break;

  case 28: // expr_list: expr_list ',' expr
#line 223 "parser.yy"
    {
        yylhs.value.as < SymEngine::vec_basic > () = yystack_[2].value.as < SymEngine::vec_basic > (); // TODO : should make copy?
        yylhs.value.as < SymEngine::vec_basic > () .push_back(yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ());
    }
#line 945 "parser.tab.cc"
    break;

  case 29: // expr_list: expr
#line 229 "parser.yy"
    {
        yylhs.value.as < SymEngine::vec_basic > () = vec_basic(1, yystack_[0].value.as < SymEngine::RCP<const SymEngine::Basic> > ());
    }
#line 953 "parser.tab.cc"
    break;


#line 957 "parser.tab.cc"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        std::string msg = YY_("syntax error");
        error (YY_MOVE (msg));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;


      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
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


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.what ());
  }

#if YYDEBUG || 0
  const char *
  parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytname_[yysymbol];
  }
#endif // #if YYDEBUG || 0





  const signed char parser::yypact_ninf_ = -14;

  const signed char parser::yytable_ninf_ = -1;

  const short
  parser::yypact_[] =
  {
      26,    -9,   -14,    23,    26,    26,    26,    26,    12,    65,
     -14,   -14,    26,    26,    24,    24,    46,   -14,   -14,    26,
      26,    26,    26,    26,    26,    26,    26,    26,    26,    26,
      26,    26,    26,    65,   -13,    24,   -14,    80,    94,   107,
      22,   118,   128,   137,   145,   -11,    29,    29,    24,    24,
      24,   -14,    26,    65
  };

  const signed char
  parser::yydefact_[] =
  {
       0,    23,    25,    24,     0,     0,     0,     0,     0,     2,
      22,    26,     0,     0,    19,    20,     0,    21,     1,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,     0,     7,    18,    15,    17,    16,
      14,    10,     9,    11,    12,    13,     4,     3,     5,     6,
       8,    27,     0,    28
  };

  const signed char
  parser::yypgoto_[] =
  {
     -14,   -14,    -4,   -14,   -14,   -14
  };

  const signed char
  parser::yydefgoto_[] =
  {
       0,     8,     9,    10,    11,    34
  };

  const signed char
  parser::yytable_[] =
  {
      14,    15,    16,    17,    28,    29,    30,    31,    33,    35,
      32,    51,    18,    52,    12,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,     1,
       2,     3,    23,    24,    25,    26,    27,    28,    29,    30,
      31,     4,     5,    32,    13,    32,    30,    31,    53,     6,
      32,     7,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,     0,     0,    32,     0,     0,
      36,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,     0,     0,    32,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,     0,
       0,    32,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,     0,     0,    32,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,     0,     0,    32,    24,
      25,    26,    27,    28,    29,    30,    31,     0,     0,    32,
      25,    26,    27,    28,    29,    30,    31,     0,     0,    32,
      26,    27,    28,    29,    30,    31,     0,     0,    32,    27,
      28,    29,    30,    31,     0,     0,    32
  };

  const signed char
  parser::yycheck_[] =
  {
       4,     5,     6,     7,    15,    16,    17,    18,    12,    13,
      21,    24,     0,    26,    23,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,     3,
       4,     5,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    15,    16,    21,    21,    21,    17,    18,    52,    23,
      21,    25,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    -1,    -1,    21,    -1,    -1,
      24,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    -1,    -1,    21,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    -1,
      -1,    21,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    -1,    -1,    21,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    -1,    -1,    21,    11,
      12,    13,    14,    15,    16,    17,    18,    -1,    -1,    21,
      12,    13,    14,    15,    16,    17,    18,    -1,    -1,    21,
      13,    14,    15,    16,    17,    18,    -1,    -1,    21,    14,
      15,    16,    17,    18,    -1,    -1,    21
  };

  const signed char
  parser::yystos_[] =
  {
       0,     3,     4,     5,    15,    16,    23,    25,    28,    29,
      30,    31,    23,    21,    29,    29,    29,    29,     0,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    21,    29,    32,    29,    24,    29,    29,    29,
      29,    29,    29,    29,    29,    29,    29,    29,    29,    29,
      29,    24,    26,    29
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    27,    28,    29,    29,    29,    29,    29,    29,    29,
      29,    29,    29,    29,    29,    29,    29,    29,    29,    29,
      29,    29,    29,    30,    30,    30,    30,    31,    32,    32
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     2,     1,     1,     1,     1,     1,     4,     3,     1
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "END_OF_FILE", "error", "\"invalid token\"", "IDENTIFIER", "NUMERIC",
  "IMPLICIT_MUL", "'|'", "'^'", "'&'", "EQ", "'>'", "'<'", "NE", "LE",
  "GE", "'-'", "'+'", "'*'", "'/'", "UMINUS", "UPLUS", "POW", "NOT", "'('",
  "')'", "'~'", "','", "$accept", "st_expr", "expr", "leaf", "func",
  "expr_list", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const unsigned char
  parser::yyrline_[] =
  {
       0,    97,    97,   105,   108,   111,   114,   119,   129,   132,
     135,   138,   141,   144,   147,   150,   158,   166,   174,   177,
     180,   183,   186,   191,   196,   202,   207,   214,   222,   228
  };

  void
  parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  parser::symbol_kind_type
  parser::yytranslate_ (int t)
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     8,     2,
      23,    24,    17,    16,    26,    15,     2,    18,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      11,     2,    10,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     7,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     6,     2,    25,     2,     2,     2,
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
       5,     9,    12,    13,    14,    19,    20,    21,    22
    };
    // Last valid token kind.
    const int code_max = 268;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return YY_CAST (symbol_kind_type, translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

} // yy
#line 1352 "parser.tab.cc"

