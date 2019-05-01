%baseclass-preinclude symengine/visitor.h
%scanner scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cpp
%namespace SymEngine


%polymorphic basic: RCP<const Basic>;
             basic_vec : vec_basic;
             string : std::string;

%token <string> IDENTIFIER
%token <string> NUMERIC
%token <string> IMPLICIT_MUL

%left '|'
%left '^'
%left '&'
%left EQ
%left '>'
%left '<'
%left LE
%left GE
%left '-' '+'
%left '*' '/'
%right UMINUS
%right POW
%right NOT
%nonassoc '('

%type <basic> st_expr
%type <basic> expr
%type <basic_vec> expr_list
%type <basic> leaf
%type <basic> func

%start st_expr

%%
st_expr :
    expr
    {
        $$ = $1;
        res = $$;
    }
;

expr:
        expr '+' expr
        { $$ = add($1, $3); }
|
        expr '-' expr
        { $$ = sub($1, $3); }
|
        expr '*' expr
        { $$ = mul($1, $3); }
|
        expr '/' expr
        { $$ = div($1, $3); }
|
        IMPLICIT_MUL POW expr
        { 
          auto tup = parse_implicit_mul($1);
          if (neq(*std::get<1>(tup), *one)) {
            $$ = mul(std::get<0>(tup), pow(std::get<1>(tup), $3));
          } else {
            $$ = pow(std::get<0>(tup), $3);
          }
        }
|
        expr POW expr
        { $$ = pow($1, $3); }
|
        expr '<' expr
        { $$ = rcp_static_cast<const Basic>(Lt($1, $3)); }
|
        expr '>' expr
        { $$ = rcp_static_cast<const Basic>(Gt($1, $3)); }
|
        expr LE expr
        { $$ = rcp_static_cast<const Basic>(Le($1, $3)); }
|
        expr GE expr
        { $$ = rcp_static_cast<const Basic>(Ge($1, $3)); }
|
        expr EQ expr
        { $$ = rcp_static_cast<const Basic>(Eq($1, $3)); }
|
        expr '|' expr
        { 
            set_boolean s;
            s.insert(rcp_static_cast<const Boolean>($1));
            s.insert(rcp_static_cast<const Boolean>($3));
            $$ = rcp_static_cast<const Basic>(logical_or(s));
        }
|
        expr '&' expr
        { 
            set_boolean s;
            s.insert(rcp_static_cast<const Boolean>($1));
            s.insert(rcp_static_cast<const Boolean>($3));
            $$ = rcp_static_cast<const Basic>(logical_and(s));
        }
|
        expr '^' expr
        {
            vec_boolean s;
            s.push_back(rcp_static_cast<const Boolean>($1));
            s.push_back(rcp_static_cast<const Boolean>($3));
            $$ = rcp_static_cast<const Basic>(logical_xor(s));
        }
|
        '(' expr ')'
        { $$ = $2; }
|
        '-' expr %prec UMINUS
        { $$ = neg($2); }
|
        '~' expr %prec NOT
        { $$ = rcp_static_cast<const Basic>(logical_not(rcp_static_cast<const Boolean>($2))); }
|
        leaf
        { $$ = rcp_static_cast<const Basic>($1); }
;

leaf:
    IDENTIFIER
    {
        $$ = parse_identifier($1);
    }
|
    IMPLICIT_MUL
    {
        auto tup = parse_implicit_mul($1);
        $$ = mul(std::get<0>(tup), std::get<1>(tup));
    }
|
    NUMERIC
    {
        $$ = parse_numeric($1);
    }
|
    func
    {
        $$ = $1;
    }
;

func:
    IDENTIFIER '(' expr_list ')'
    {
        $$ = functionify($1, $3);
    }
;

expr_list:

    expr_list ',' expr
    {
        $$ = $1; // TODO : should make copy?
        $$ .push_back($3);
    }
|
    expr
    {
        $$ = vec_basic(1, $1);
    }
;
