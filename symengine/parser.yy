%baseclass-preinclude symengine/visitor.h
%scanner scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cpp
%namespace SymEngine


%polymorphic basic: RCP<const Basic>;
			 basic_vec :  vec_basic;
			 string : std::string;
			 symbol : RCP<const Symbol>;
			 integer : RCP<const Integer>;

%token <integer> INTEGER
%token <string> IDENTIFIER
%token <string> CONSTANT

%left '+' '-'
%left '*' '/'
%right POW
%right UMINUS
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
        {
        	$$ = add($1, $3);
        	std::cout<<"here"<<std::endl;
        }
|
        expr '-' expr
        {
        	$$ = sub($1, $3);
        }
|
        expr '*' expr
        {
        	$$ = mul($1, $3);
        }
|
        expr '/' expr
        {
        	$$ = div($1, $3);
        }
|
        expr POW expr
        {
        	$$ = pow($1, $3);
        }
|
        '(' expr ')'
        {
        	$$ = $2;
        }
|
       	'-' expr %prec UMINUS
       	{
       		$$ = neg($2);
       	}
|
		leaf
		{
			$$ = $1;
		}
;

leaf:
	IDENTIFIER
	{
		$$ = SymEngine::symbol($1);
	}
|
	INTEGER
	{
		$$ = $1;
	}
|
	CONSTANT
	{
		$$ = constants[$1];
	}
|
	func
	{
		$$ = $1;
	}
;

func:
	IDENTIFIER '(' expr ')'
	{
		if (single_arg_functions.find($1) != single_arg_functions.end()) {
		    $$ = single_arg_functions[$1]($3);
		} else if (multi_arg_functions.find($1) != multi_arg_functions.end()) {
			vec_basic v = {$3};
		    $$ = multi_arg_functions[$1](v);
		} else {
			$$ = function_symbol($1, vec_basic({$3}));
		}
	}
|
	IDENTIFIER '(' expr ',' expr ')'
	{
		if (double_arg_functions.find($1) != double_arg_functions.end()) {
		    $$ = double_arg_functions[$1]($3, $5);
		} else if (multi_arg_functions.find($1) != multi_arg_functions.end()) {
			vec_basic v = {$3, $5};
		    $$ = multi_arg_functions[$1](v);
		} else {
			$$ = function_symbol($1, vec_basic({$3, $5}));
		}
	} 
|
	IDENTIFIER '(' expr_list ')'
	{
		if (multi_arg_functions.find($1) != multi_arg_functions.end()) {
		    $$ = multi_arg_functions[$1]($3);
		} else {
			$$ = function_symbol($1, $3);
		}
	}
;

expr_list:

	expr_list ',' expr
	{
		$$ = $1; // TODO : should make copy?
		$$ .push_back($3);
	}
|
	expr ',' expr ',' expr
	{
		$$ = {$1, $3, $5};
	}
;