// Automatically generated

// base/comment

// $insert class.ih
#include "parser.ih"

// The FIRST element of SR arrays shown below uses `d_type', defining the
// state's type, and `d_lastIdx' containing the last element's index. If
// d_lastIdx contains the REQ_TOKEN bitflag (see below) then the state needs
// a token: if in this state d_token is Reserved__::UNDETERMINED__, nextToken()
// will be
// called

// The LAST element of SR arrays uses `d_token' containing the last retrieved
// token to speed up the (linear) seach.  Except for the first element of SR
// arrays, the field `d_action' is used to determine what to do next. If
// positive, it represents the next state (used with SHIFT); if zero, it
// indicates `ACCEPT', if negative, -d_action represents the number of the
// rule to reduce to.

// `lookup()' tries to find d_token in the current SR array. If it fails, and
// there is no default reduction UNEXPECTED_TOKEN__ is thrown, which is then
// caught by the error-recovery function.

// The error-recovery function will pop elements off the stack until a state
// having bit flag ERR_ITEM is found. This state has a transition on errTok__
// which is applied. In this errTok__ state, while the current token is not a
// proper continuation, new tokens are obtained by nextToken(). If such a
// token is found, error recovery is successful and the token is
// handled according to the error state's SR table and parsing continues.
// During error recovery semantic actions are ignored.

// A state flagged with the DEF_RED flag will perform a default
// reduction if no other continuations are available for the current token.

// The ACCEPT STATE never shows a default reduction: when it is reached the
// parser returns ACCEPT(). During the grammar
// analysis phase a default reduction may have been defined, but it is
// removed during the state-definition phase.

// So:
//      s_x[] =
//      {
//                  [_field_1_]         [_field_2_]
//
// First element:   {state-type,        idx of last element},
// Other elements:  {required token,    action to perform},
//                                      ( < 0: reduce,
//                                          0: ACCEPT,
//                                        > 0: next state)
//      }

// base/declarations

namespace // anonymous
{

enum Reserved__ { UNDETERMINED__ = -2, EOF__ = -1, errTok__ = 256 };
enum StateType // modify statetype/data.cc when this enum changes
{ NORMAL,
  ERR_ITEM,
  REQ_TOKEN,
  ERR_REQ,    // ERR_ITEM | REQ_TOKEN
  DEF_RED,    // state having default reduction
  ERR_DEF,    // ERR_ITEM | DEF_RED
  REQ_DEF,    // REQ_TOKEN | DEF_RED
  ERR_REQ_DEF // ERR_ITEM | REQ_TOKEN | DEF_RED
};
enum StateTransition {
    ACCEPT__ = 0, // `ACCEPT' TRANSITION
};

struct PI__ // Production Info
{
    size_t d_nonTerm; // identification number of this production's
                      // non-terminal
    size_t d_size;    // number of elements in this production
};

struct SR__ // Shift Reduce info, see its description above
{
    union {
        int _field_1_; // initializer, allowing initializations
                       // of the SR s_[] arrays
        StateType d_type;
        int d_token;
    };
    union {
        int _field_2_;

        int d_lastIdx; // if negative, the state uses SHIFT
        int d_action;  // may be negative (reduce),
                       // postive (shift), or 0 (accept)
    };
};

// $insert staticdata

enum // size to expand the state-stack with when
{    // full
    STACK_EXPANSION__ = 10
};

// Productions Info Records:
PI__ const s_productionInfo[] = {
    {0, 0},   // not used: reduction values are negative
    {271, 1}, // 1: st_expr ->  expr
    {272, 3}, // 2: expr ('+') ->  expr '+' expr
    {272, 3}, // 3: expr ('-') ->  expr '-' expr
    {272, 3}, // 4: expr ('*') ->  expr '*' expr
    {272, 3}, // 5: expr ('/') ->  expr '/' expr
    {272, 3}, // 6: expr (IMPLICIT_MUL) ->  IMPLICIT_MUL POW expr
    {272, 3}, // 7: expr (POW) ->  expr POW expr
    {272, 3}, // 8: expr ('<') ->  expr '<' expr
    {272, 3}, // 9: expr ('>') ->  expr '>' expr
    {272, 3}, // 10: expr (LE) ->  expr LE expr
    {272, 3}, // 11: expr (GE) ->  expr GE expr
    {272, 3}, // 12: expr (EQ) ->  expr EQ expr
    {272, 3}, // 13: expr ('|') ->  expr '|' expr
    {272, 3}, // 14: expr ('&') ->  expr '&' expr
    {272, 3}, // 15: expr ('^') ->  expr '^' expr
    {272, 3}, // 16: expr ('(') ->  '(' expr ')'
    {272, 2}, // 17: expr (UMINUS) ->  '-' expr
    {272, 2}, // 18: expr (NOT) ->  '~' expr
    {272, 1}, // 19: expr ->  leaf
    {273, 1}, // 20: leaf (IDENTIFIER) ->  IDENTIFIER
    {273, 1}, // 21: leaf (IMPLICIT_MUL) ->  IMPLICIT_MUL
    {273, 1}, // 22: leaf (NUMERIC) ->  NUMERIC
    {273, 1}, // 23: leaf ->  func
    {274, 4}, // 24: func (IDENTIFIER) ->  IDENTIFIER '(' expr_list ')'
    {275, 3}, // 25: expr_list (',') ->  expr_list ',' expr
    {275, 1}, // 26: expr_list ->  expr
    {276, 1}, // 27: st_expr_$ ->  st_expr
};

// State info and SR__ transitions for each state.

SR__ s_0[] = {
    {{REQ_TOKEN}, {11}}, {{271}, {1}}, // st_expr
    {{272}, {2}},                      // expr
    {{259}, {3}},                      // IMPLICIT_MUL
    {{40}, {4}},                       // '('
    {{45}, {5}},                       // '-'
    {{126}, {6}},                      // '~'
    {{273}, {7}},                      // leaf
    {{257}, {8}},                      // IDENTIFIER
    {{258}, {9}},                      // NUMERIC
    {{274}, {10}},                     // func
    {{0}, {0}},
};

SR__ s_1[] = {
    {{REQ_TOKEN}, {2}}, {{EOF__}, {ACCEPT__}}, {{0}, {0}},
};

SR__ s_2[] = {
    {{REQ_DEF}, {14}}, {{43}, {11}}, // '+'
    {{45}, {12}},                    // '-'
    {{42}, {13}},                    // '*'
    {{47}, {14}},                    // '/'
    {{264}, {15}},                   // POW
    {{60}, {16}},                    // '<'
    {{62}, {17}},                    // '>'
    {{261}, {18}},                   // LE
    {{262}, {19}},                   // GE
    {{260}, {20}},                   // EQ
    {{124}, {21}},                   // '|'
    {{38}, {22}},                    // '&'
    {{94}, {23}},                    // '^'
    {{0}, {-1}},
};

SR__ s_3[] = {
    {{REQ_DEF}, {2}},
    {{264}, {24}}, // POW
    {{0}, {-21}},
};

SR__ s_4[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {25}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_5[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {26}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_6[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {27}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_7[] = {
    {{DEF_RED}, {1}}, {{0}, {-19}},
};

SR__ s_8[] = {
    {{REQ_DEF}, {2}},
    {{40}, {28}}, // '('
    {{0}, {-20}},
};

SR__ s_9[] = {
    {{DEF_RED}, {1}}, {{0}, {-22}},
};

SR__ s_10[] = {
    {{DEF_RED}, {1}}, {{0}, {-23}},
};

SR__ s_11[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {29}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_12[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {30}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_13[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {31}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_14[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {32}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_15[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {33}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_16[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {34}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_17[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {35}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_18[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {36}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_19[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {37}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_20[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {38}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_21[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {39}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_22[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {40}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_23[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {41}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_24[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {42}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_25[] = {
    {{REQ_TOKEN}, {15}}, {{41}, {43}}, // ')'
    {{43}, {11}},                      // '+'
    {{45}, {12}},                      // '-'
    {{42}, {13}},                      // '*'
    {{47}, {14}},                      // '/'
    {{264}, {15}},                     // POW
    {{60}, {16}},                      // '<'
    {{62}, {17}},                      // '>'
    {{261}, {18}},                     // LE
    {{262}, {19}},                     // GE
    {{260}, {20}},                     // EQ
    {{124}, {21}},                     // '|'
    {{38}, {22}},                      // '&'
    {{94}, {23}},                      // '^'
    {{0}, {0}},
};

SR__ s_26[] = {
    {{REQ_DEF}, {2}},
    {{264}, {15}}, // POW
    {{0}, {-17}},
};

SR__ s_27[] = {
    {{REQ_DEF}, {1}}, {{0}, {-18}},
};

SR__ s_28[] = {
    {{REQ_TOKEN}, {11}}, {{275}, {44}}, // expr_list
    {{272}, {45}},                      // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_29[] = {
    {{REQ_DEF}, {4}}, {{42}, {13}}, // '*'
    {{47}, {14}},                   // '/'
    {{264}, {15}},                  // POW
    {{0}, {-2}},
};

SR__ s_30[] = {
    {{REQ_DEF}, {4}}, {{42}, {13}}, // '*'
    {{47}, {14}},                   // '/'
    {{264}, {15}},                  // POW
    {{0}, {-3}},
};

SR__ s_31[] = {
    {{REQ_DEF}, {2}},
    {{264}, {15}}, // POW
    {{0}, {-4}},
};

SR__ s_32[] = {
    {{REQ_DEF}, {2}},
    {{264}, {15}}, // POW
    {{0}, {-5}},
};

SR__ s_33[] = {
    {{REQ_DEF}, {2}},
    {{264}, {15}}, // POW
    {{0}, {-7}},
};

SR__ s_34[] = {
    {{REQ_DEF}, {8}}, {{43}, {11}}, // '+'
    {{45}, {12}},                   // '-'
    {{42}, {13}},                   // '*'
    {{47}, {14}},                   // '/'
    {{264}, {15}},                  // POW
    {{261}, {18}},                  // LE
    {{262}, {19}},                  // GE
    {{0}, {-8}},
};

SR__ s_35[] = {
    {{REQ_DEF}, {9}}, {{43}, {11}}, // '+'
    {{45}, {12}},                   // '-'
    {{42}, {13}},                   // '*'
    {{47}, {14}},                   // '/'
    {{264}, {15}},                  // POW
    {{60}, {16}},                   // '<'
    {{261}, {18}},                  // LE
    {{262}, {19}},                  // GE
    {{0}, {-9}},
};

SR__ s_36[] = {
    {{REQ_DEF}, {7}}, {{43}, {11}}, // '+'
    {{45}, {12}},                   // '-'
    {{42}, {13}},                   // '*'
    {{47}, {14}},                   // '/'
    {{264}, {15}},                  // POW
    {{262}, {19}},                  // GE
    {{0}, {-10}},
};

SR__ s_37[] = {
    {{REQ_DEF}, {6}}, {{43}, {11}}, // '+'
    {{45}, {12}},                   // '-'
    {{42}, {13}},                   // '*'
    {{47}, {14}},                   // '/'
    {{264}, {15}},                  // POW
    {{0}, {-11}},
};

SR__ s_38[] = {
    {{REQ_DEF}, {10}}, {{43}, {11}}, // '+'
    {{45}, {12}},                    // '-'
    {{42}, {13}},                    // '*'
    {{47}, {14}},                    // '/'
    {{264}, {15}},                   // POW
    {{60}, {16}},                    // '<'
    {{62}, {17}},                    // '>'
    {{261}, {18}},                   // LE
    {{262}, {19}},                   // GE
    {{0}, {-12}},
};

SR__ s_39[] = {
    {{REQ_DEF}, {13}}, {{43}, {11}}, // '+'
    {{45}, {12}},                    // '-'
    {{42}, {13}},                    // '*'
    {{47}, {14}},                    // '/'
    {{264}, {15}},                   // POW
    {{60}, {16}},                    // '<'
    {{62}, {17}},                    // '>'
    {{261}, {18}},                   // LE
    {{262}, {19}},                   // GE
    {{260}, {20}},                   // EQ
    {{38}, {22}},                    // '&'
    {{94}, {23}},                    // '^'
    {{0}, {-13}},
};

SR__ s_40[] = {
    {{REQ_DEF}, {11}}, {{43}, {11}}, // '+'
    {{45}, {12}},                    // '-'
    {{42}, {13}},                    // '*'
    {{47}, {14}},                    // '/'
    {{264}, {15}},                   // POW
    {{60}, {16}},                    // '<'
    {{62}, {17}},                    // '>'
    {{261}, {18}},                   // LE
    {{262}, {19}},                   // GE
    {{260}, {20}},                   // EQ
    {{0}, {-14}},
};

SR__ s_41[] = {
    {{REQ_DEF}, {12}}, {{43}, {11}}, // '+'
    {{45}, {12}},                    // '-'
    {{42}, {13}},                    // '*'
    {{47}, {14}},                    // '/'
    {{264}, {15}},                   // POW
    {{60}, {16}},                    // '<'
    {{62}, {17}},                    // '>'
    {{261}, {18}},                   // LE
    {{262}, {19}},                   // GE
    {{260}, {20}},                   // EQ
    {{38}, {22}},                    // '&'
    {{0}, {-15}},
};

SR__ s_42[] = {
    {{REQ_DEF}, {14}}, {{43}, {11}}, // '+'
    {{45}, {12}},                    // '-'
    {{42}, {13}},                    // '*'
    {{47}, {14}},                    // '/'
    {{264}, {15}},                   // POW
    {{60}, {16}},                    // '<'
    {{62}, {17}},                    // '>'
    {{261}, {18}},                   // LE
    {{262}, {19}},                   // GE
    {{260}, {20}},                   // EQ
    {{124}, {21}},                   // '|'
    {{38}, {22}},                    // '&'
    {{94}, {23}},                    // '^'
    {{0}, {-6}},
};

SR__ s_43[] = {
    {{DEF_RED}, {1}}, {{0}, {-16}},
};

SR__ s_44[] = {
    {{REQ_TOKEN}, {3}},
    {{41}, {46}}, // ')'
    {{44}, {47}}, // ','
    {{0}, {0}},
};

SR__ s_45[] = {
    {{REQ_DEF}, {14}}, {{43}, {11}}, // '+'
    {{45}, {12}},                    // '-'
    {{42}, {13}},                    // '*'
    {{47}, {14}},                    // '/'
    {{264}, {15}},                   // POW
    {{60}, {16}},                    // '<'
    {{62}, {17}},                    // '>'
    {{261}, {18}},                   // LE
    {{262}, {19}},                   // GE
    {{260}, {20}},                   // EQ
    {{124}, {21}},                   // '|'
    {{38}, {22}},                    // '&'
    {{94}, {23}},                    // '^'
    {{0}, {-26}},
};

SR__ s_46[] = {
    {{DEF_RED}, {1}}, {{0}, {-24}},
};

SR__ s_47[] = {
    {{REQ_TOKEN}, {10}}, {{272}, {48}}, // expr
    {{259}, {3}},                       // IMPLICIT_MUL
    {{40}, {4}},                        // '('
    {{45}, {5}},                        // '-'
    {{126}, {6}},                       // '~'
    {{273}, {7}},                       // leaf
    {{257}, {8}},                       // IDENTIFIER
    {{258}, {9}},                       // NUMERIC
    {{274}, {10}},                      // func
    {{0}, {0}},
};

SR__ s_48[] = {
    {{REQ_DEF}, {14}}, {{43}, {11}}, // '+'
    {{45}, {12}},                    // '-'
    {{42}, {13}},                    // '*'
    {{47}, {14}},                    // '/'
    {{264}, {15}},                   // POW
    {{60}, {16}},                    // '<'
    {{62}, {17}},                    // '>'
    {{261}, {18}},                   // LE
    {{262}, {19}},                   // GE
    {{260}, {20}},                   // EQ
    {{124}, {21}},                   // '|'
    {{38}, {22}},                    // '&'
    {{94}, {23}},                    // '^'
    {{0}, {-25}},
};

// State array:
SR__ *s_state[] = {
    s_0,  s_1,  s_2,  s_3,  s_4,  s_5,  s_6,  s_7,  s_8,  s_9,
    s_10, s_11, s_12, s_13, s_14, s_15, s_16, s_17, s_18, s_19,
    s_20, s_21, s_22, s_23, s_24, s_25, s_26, s_27, s_28, s_29,
    s_30, s_31, s_32, s_33, s_34, s_35, s_36, s_37, s_38, s_39,
    s_40, s_41, s_42, s_43, s_44, s_45, s_46, s_47, s_48,
};

} // anonymous namespace ends

// $insert namespace-open
namespace SymEngine
{

// $insert polymorphicCode
namespace Meta__
{

size_t const *t_nErrors;
// $insert idoftag
char const *idOfTag__[] = {"basic", "string", "basic_vec", "<undefined>"};

size_t const *s_nErrors__;

Base::~Base()
{
}

} // namespace Meta__

// If the parsing function call (i.e., parse()' needs arguments, then provide
// an overloaded function.  The code below doesn't rely on parameters, so no
// arguments are required.  Furthermore, parse uses a function try block to
// allow us to do ACCEPT and ABORT from anywhere, even from within members
// called by actions, simply throwing the appropriate exceptions.

// base/base1
ParserBase::ParserBase()
    : d_token(Reserved__::UNDETERMINED__),
      // $insert baseclasscode
      d_requiredTokens__(0)
{
    Meta__::t_nErrors = &d_nErrors__;
}

// base/clearin
void ParserBase::clearin__()
{
    d_nErrors__ = 0;
    d_stackIdx = -1;
    d_stateStack.clear();
    d_token = Reserved__::UNDETERMINED__;
    d_next = TokenPair{Reserved__::UNDETERMINED__, STYPE__{}};
    d_recovery = false;
    d_acceptedTokens__ = d_requiredTokens__;
    d_val__ = STYPE__{};

    push__(0);
}

// base/debugfunctions

void ParserBase::setDebug(bool mode)
{
    d_actionCases__ = false;
    d_debug__ = mode;
}

void ParserBase::setDebug(DebugMode__ mode)
{
    d_actionCases__ = mode & ACTIONCASES;
    d_debug__ = mode & ON;
}

// base/lex
void ParserBase::lex__(int token)
{
    d_token = token;

    if (d_token <= 0)
        d_token = Reserved__::EOF__;

    d_terminalToken = true;
}

// base/lookup
int ParserBase::lookup__() const
{
    // if the final transition is negative, then we should reduce by the rule
    // given by its positive value.

    SR__ const *sr = s_state[d_state];
    SR__ const *last = sr + sr->d_lastIdx;

    for (; ++sr != last;) // visit all but the last SR entries
    {
        if (sr->d_token == d_token)
            return sr->d_action;
    }

    if (sr == last) // reached the last element
    {
        if (sr->d_action < 0) // default reduction
        {
            return sr->d_action;
        }

        // No default reduction, so token not found, so error.
        throw UNEXPECTED_TOKEN__;
    }

    // not at the last element: inspect the nature of the action
    // (< 0: reduce, 0: ACCEPT, > 0: shift)

    int action = sr->d_action;

    return action;
}

// base/pop
void ParserBase::pop__(size_t count)
{
    if (d_stackIdx < static_cast<int>(count)) {
        ABORT();
    }

    d_stackIdx -= count;
    d_state = d_stateStack[d_stackIdx].first;
    d_vsp = &d_stateStack[d_stackIdx];
}

// base/poptoken
void ParserBase::popToken__()
{
    d_token = d_next.first;
    d_val__ = std::move(d_next.second);

    d_next.first = Reserved__::UNDETERMINED__;
}

// base/push
void ParserBase::push__(size_t state)
{
    size_t currentSize = d_stateStack.size();
    if (stackSize__() == currentSize) {
        size_t newSize = currentSize + STACK_EXPANSION__;
        d_stateStack.resize(newSize);
    }

    ++d_stackIdx;
    d_stateStack[d_stackIdx] = StatePair{d_state = state, std::move(d_val__)};

    d_vsp = &d_stateStack[d_stackIdx];

    if (d_stackIdx == 0) {
    } else {
    }
}

// base/pushtoken
void ParserBase::pushToken__(int token)
{
    d_next = TokenPair{d_token, std::move(d_val__)};
    d_token = token;
}

// base/redotoken
void ParserBase::redoToken__()
{
    if (d_token != Reserved__::UNDETERMINED__)
        pushToken__(d_token);
}

// base/reduce
void ParserBase::reduce__(int rule)
{
    PI__ const &pi = s_productionInfo[rule];

    d_token = pi.d_nonTerm;
    pop__(pi.d_size);

    d_terminalToken = false;
}

// base/shift
void ParserBase::shift__(int action)
{
    push__(action);
    popToken__(); // token processed

    if (d_recovery and d_terminalToken) {
        d_recovery = false;
        d_acceptedTokens__ = 0;
    }
}

// base/startrecovery
void ParserBase::startRecovery__()
{
    int lastToken = d_token; // give the unexpected token a
                             // chance to be processed
                             // again.

    pushToken__(Reserved__::errTok__); // specify errTok__ as next token
    push__(lookup__());                // push the error state

    d_token = lastToken; // reactivate the unexpected
                         // token (we're now in an
                         // ERROR state).

    d_recovery = true;
}

// base/top
inline size_t ParserBase::top__() const
{
    return d_stateStack[d_stackIdx].first;
}

// derived/errorrecovery
void Parser::errorRecovery__()
{
    // When an error has occurred, pop elements off the stack until the top
    // state has an error-item. If none is found, the default recovery
    // mode (which is to abort) is activated.
    //
    // If EOF is encountered without being appropriate for the current state,
    // then the error recovery will fall back to the default recovery mode.
    // (i.e., parsing terminates)

    if (d_acceptedTokens__ >= d_requiredTokens__) // only generate an error-
    {                                             // message if enough tokens
        ++d_nErrors__;                            // were accepted. Otherwise
        error();                                  // simply skip input
    }

    // get the error state
    while (not(s_state[top__()][0].d_type & ERR_ITEM)) {
        pop__();
    }

    // In the error state, looking up a token allows us to proceed.
    // Continuation may be require multiple reductions, but eventually a
    // terminal-token shift is used. See nextCycle__ for details.

    startRecovery__();
}

// derived/executeaction
void Parser::executeAction__(int production) try {
    if (token__() != Reserved__::UNDETERMINED__)
        pushToken__(token__()); // save an already available token
    switch (production) {
        // $insert actioncases

        case 1:
#line 43 "parser.yy"
        {
            d_val__ = vs__(0).get<Tag__::basic>();
            res = d_val__.get<Tag__::basic>();
        } break;

        case 2:
#line 51 "parser.yy"
        {
            d_val__ = add(vs__(-2).get<Tag__::basic>(),
                          vs__(0).get<Tag__::basic>());
        } break;

        case 3:
#line 54 "parser.yy"
        {
            d_val__ = sub(vs__(-2).get<Tag__::basic>(),
                          vs__(0).get<Tag__::basic>());
        } break;

        case 4:
#line 57 "parser.yy"
        {
            d_val__ = mul(vs__(-2).get<Tag__::basic>(),
                          vs__(0).get<Tag__::basic>());
        } break;

        case 5:
#line 60 "parser.yy"
        {
            d_val__ = div(vs__(-2).get<Tag__::basic>(),
                          vs__(0).get<Tag__::basic>());
        } break;

        case 6:
#line 63 "parser.yy"
        {
            auto tup = parse_implicit_mul(vs__(-2).get<Tag__::string>());
            if (neq(*std::get<1>(tup), *one)) {
                d_val__
                    = mul(std::get<0>(tup),
                          pow(std::get<1>(tup), vs__(0).get<Tag__::basic>()));
            } else {
                d_val__ = pow(std::get<0>(tup), vs__(0).get<Tag__::basic>());
            }
        } break;

        case 7:
#line 73 "parser.yy"
        {
            d_val__ = pow(vs__(-2).get<Tag__::basic>(),
                          vs__(0).get<Tag__::basic>());
        } break;

        case 8:
#line 76 "parser.yy"
        {
            d_val__ = rcp_static_cast<const Basic>(
                Lt(vs__(-2).get<Tag__::basic>(), vs__(0).get<Tag__::basic>()));
        } break;

        case 9:
#line 79 "parser.yy"
        {
            d_val__ = rcp_static_cast<const Basic>(
                Gt(vs__(-2).get<Tag__::basic>(), vs__(0).get<Tag__::basic>()));
        } break;

        case 10:
#line 82 "parser.yy"
        {
            d_val__ = rcp_static_cast<const Basic>(
                Le(vs__(-2).get<Tag__::basic>(), vs__(0).get<Tag__::basic>()));
        } break;

        case 11:
#line 85 "parser.yy"
        {
            d_val__ = rcp_static_cast<const Basic>(
                Ge(vs__(-2).get<Tag__::basic>(), vs__(0).get<Tag__::basic>()));
        } break;

        case 12:
#line 88 "parser.yy"
        {
            d_val__ = rcp_static_cast<const Basic>(
                Eq(vs__(-2).get<Tag__::basic>(), vs__(0).get<Tag__::basic>()));
        } break;

        case 13:
#line 91 "parser.yy"
        {
            set_boolean s;
            s.insert(
                rcp_static_cast<const Boolean>(vs__(-2).get<Tag__::basic>()));
            s.insert(
                rcp_static_cast<const Boolean>(vs__(0).get<Tag__::basic>()));
            d_val__ = rcp_static_cast<const Basic>(logical_or(s));
        } break;

        case 14:
#line 99 "parser.yy"
        {
            set_boolean s;
            s.insert(
                rcp_static_cast<const Boolean>(vs__(-2).get<Tag__::basic>()));
            s.insert(
                rcp_static_cast<const Boolean>(vs__(0).get<Tag__::basic>()));
            d_val__ = rcp_static_cast<const Basic>(logical_and(s));
        } break;

        case 15:
#line 107 "parser.yy"
        {
            vec_boolean s;
            s.push_back(
                rcp_static_cast<const Boolean>(vs__(-2).get<Tag__::basic>()));
            s.push_back(
                rcp_static_cast<const Boolean>(vs__(0).get<Tag__::basic>()));
            d_val__ = rcp_static_cast<const Basic>(logical_xor(s));
        } break;

        case 16:
#line 115 "parser.yy"
        {
            d_val__ = vs__(-1).get<Tag__::basic>();
        } break;

        case 17:
#line 118 "parser.yy"
        {
            d_val__ = neg(vs__(0).get<Tag__::basic>());
        } break;

        case 18:
#line 121 "parser.yy"
        {
            d_val__ = rcp_static_cast<const Basic>(logical_not(
                rcp_static_cast<const Boolean>(vs__(0).get<Tag__::basic>())));
        } break;

        case 19:
#line 124 "parser.yy"
        {
            d_val__ = rcp_static_cast<const Basic>(vs__(0).get<Tag__::basic>());
        } break;

        case 20:
#line 129 "parser.yy"
        {
            d_val__ = parse_identifier(vs__(0).get<Tag__::string>());
        } break;

        case 21:
#line 134 "parser.yy"
        {
            auto tup = parse_implicit_mul(vs__(0).get<Tag__::string>());
            d_val__ = mul(std::get<0>(tup), std::get<1>(tup));
        } break;

        case 22:
#line 140 "parser.yy"
        {
            d_val__ = parse_numeric(vs__(0).get<Tag__::string>());
        } break;

        case 23:
#line 145 "parser.yy"
        {
            d_val__ = vs__(0).get<Tag__::basic>();
        } break;

        case 24:
#line 152 "parser.yy"
        {
            d_val__ = functionify(vs__(-3).get<Tag__::string>(),
                                  vs__(-1).get<Tag__::basic_vec>());
        } break;

        case 25:
#line 160 "parser.yy"
        {
            d_val__ = vs__(-2).get<Tag__::basic_vec>();
            d_val__.get<Tag__::basic_vec>().push_back(
                vs__(0).get<Tag__::basic>());
        } break;

        case 26:
#line 166 "parser.yy"
        {
            d_val__ = vec_basic(1, vs__(0).get<Tag__::basic>());
        } break;
    }
} catch (std::exception const &exc) {
    exceptionHandler(exc);
}

// derived/nextcycle
void Parser::nextCycle__() try {
    if (s_state[state__()]->d_type & REQ_TOKEN)
        nextToken__(); // obtain next token

    int action = lookup__(); // lookup d_token in d_state

    if (action > 0) // SHIFT: push a new state
    {
        shift__(action);
        return;
    }

    if (action < 0) // REDUCE: execute and pop.
    {

        if (recovery__())
            redoToken__();
        else
            executeAction__(-action);
        // next token is the rule's LHS
        reduce__(-action);
        return;
    }

    if (recovery__())
        ABORT();
    else
        ACCEPT();
} catch (ErrorRecovery__) {
    if (not recovery__())
        errorRecovery__();
    else {
        if (token__() == Reserved__::EOF__)
            ABORT();
        popToken__(); // skip the failing token
    }
}

// derived/nexttoken
void Parser::nextToken__()
{
    // If d_token is Reserved__::UNDETERMINED__ then if savedToken__() is
    // Reserved__::UNDETERMINED__ another token is obtained from lex(). Then
    // savedToken__() is assigned to d_token.

    // no need for a token: got one already
    if (token__() != Reserved__::UNDETERMINED__) {
        return;
    }

    if (savedToken__() != Reserved__::UNDETERMINED__) {
        popToken__(); // consume pending token
    } else {
        ++d_acceptedTokens__; // accept another token (see
                              // errorRecover())
        lex__(lex());
        print__();
    }
    print();
}

// derived/print
void Parser::print__()
{
    // $insert print
}

// derived/parse
int Parser::parse() try {
    // The parsing algorithm:
    // Initially, state 0 is pushed on the stack, and all relevant variables
    // are initialized by Base::clearin__.
    //
    // Then, in an eternal loop:
    //
    //  1. If a state is a REQ_TOKEN type, then the next token is obtained
    //     from nextToken().  This may very well be the currently available
    //     token. When retrieving a terminal token d_terminal is set to true.
    //
    //  2. lookup() is called, d_token is looked up in the current state's
    //     SR_ array.
    //
    //  4. Depending on the result of the lookup() function the next state is
    //     shifted on the parser's stack, a reduction by some rule is applied,
    //     or the parsing function returns ACCEPT(). When a reduction is
    //     called for, any action that may have been defined for that
    //     reduction is executed.
    //
    //  5. An error occurs if d_token is not found, and the state has no
    //     default reduction.

    clearin__(); // initialize, push(0)

    while (true) {
        // $insert prompt
        nextCycle__();
    }
} catch (Return__ retValue) {
    return retValue or d_nErrors__;
}

// derived/tail

// $insert namespace-close
}
