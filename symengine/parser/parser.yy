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
%right POW
%right UMINUS
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
        expr POW expr
        { $$ = pow($1, $3); }
|
        expr '<' expr
        { $$ = Lt($1, $3); }
|
        expr '>' expr
        { $$ = Gt($1, $3); }
|
        expr LE expr
        { $$ = Le($1, $3); }
|
        expr GE expr
        { $$ = Ge($1, $3); }
|
        expr EQ expr
        { $$ = Eq($1, $3); }
|
        expr '|' expr
        { 
            set_boolean s;
            s.insert(rcp_static_cast<const Boolean>($1));
            s.insert(rcp_static_cast<const Boolean>($3));
            $$ = logical_or(s);
        }
|
        expr '&' expr
        { 
            set_boolean s;
            s.insert(rcp_static_cast<const Boolean>($1));
            s.insert(rcp_static_cast<const Boolean>($3));
            $$ = logical_and(s);
        }
|
        expr '^' expr
        {
            vec_boolean s;
            s.push_back(rcp_static_cast<const Boolean>($1));
            s.push_back(rcp_static_cast<const Boolean>($3));
            $$ = logical_xor(s);
        }
|
        '(' expr ')'
        { $$ = $2; }
|
        '-' expr %prec UMINUS
        { $$ = neg($2); }
|
        '~' expr %prec NOT
        { $$ = logical_not(rcp_static_cast<const Boolean>($2)); }
|
        leaf
        { $$ = $1; }
;

leaf:
    IDENTIFIER
    {
        $$ = parse_identifier($1);
    }
|
    IMPLICIT_MUL
    {
        $$ = parse_implicit_mul($1);
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
        $$ = {$1};
    }
;
