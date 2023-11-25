grammar ShellExpr;

start
	: command (separator command)* EOF
    ;

command
	: simple_command														# simpleStmt
	| compound_command														# compoundStmt
	| pipeline																# pipelineStmt
	| and_or        														# andOrStmt
	;

compound_command
	: for_clause
	| if_clause
	| while_clause
	| brace_group
	| function_definition
	;

function_definition
	: DEF ID LPAREN function_args? RPAREN LBRACE compound_list RBRACE		# functionDef
	;

function_args
	: ID (',' ID)* 															# functionArgs
	;

simple_command
	: cmd_name args															# cmdArgs
	| cmd_name																# cmd
	| operation                         									# operationStmt
    ;

args
	: ID+																	# argsBody
	;

operation
	: ID '=' expr                         									# assing
    | 'export' ID '=' expr		        									# export
	| 'let' ID '=' expr	                									# declaration
	| 'show' ID 	                										# show
	| 'fn' ID LPAREN function_args? RPAREN									# callFunction
	;
	
expr
	: expr opt=('*'|'/') expr                     							# mulDivOpe
    | expr opt=('+'|'-') expr                   							# sumMinOpe
    | expr opt=('<'|'>'|'>='|'<='| '=='|'!=') expr   						# compOpe
    | NUMBER                   												# number
	| ID                                    								# idStmt
    | LIST                                  								# listStmt
    ;

compound_list
	: (command) (separator (command))* separator? 			# compoundListBody
	;

for_clause
	: FOR ID IN expr do_group          										# forBody
	;

brace_group
	: LBRACE compound_list RBRACE    										# braceBody
    ;

do_group
	: DO compound_list DONE             									# doBody
    ;            

and_or
	: pipeline (AND_IF pipeline | OR_IF pipeline)* 							# andOrBody
    ;

pipeline
	: simple_command (io_redirect simple_command)* 							# pipelineBody
    ;

if_clause
	: IF expr DO compound_list else_part DONE         						# ifElseBody
	| IF expr do_group                              						# ifBody
	;

else_part
	: ELSE IF expr DO command* else_part              						# elseIfBody
	| ELSE compound_list                            						# elseBody
	;           

while_clause
	: WHILE expr do_group                   								# whileBody
	;

cmd_name
	: ID
	;

separator
	: '&'
	| ';'
	;



io_redirect
	: '<'
	| '>'
	| DGREAT
	| '|'
	;

WHILE		: 'while';
FOR			: 'for';
IF			: 'if';
ELSE		: 'else';
AND_IF		: '&&';
OR_IF		: '||';
IN			: 'in';
DO			: 'do';
DONE		: 'done';
TRUE		: 'true';
FALSE		: 'false';
DEF			: 'def';
LBRACE		: '{';
RBRACE		: '}';
LPAREN		: '(';
RPAREN		: ')';

DGREAT		: '>>';
PIPE		: '|';

MUL			: '*';
DIV			: '/';
PLUS		: '+';
MINUS		: '-';
LESS		: '<';
GREAT		: '>';
GREAT_EQ	: '>=';
LESS_EQ		: '<=';
NOT_EQ		: '!=';
EQUALS		: '==';



NUMBER						: '-'? ( '.' DIGIT+ | DIGIT+ ( '.' DIGIT*)?);
ID							: (ALPHANUMERIC_CHAR | DIGIT)+;

fragment DIGIT				: [0-9];
fragment LETTER				: [a-zA-Z];
fragment ALPHANUMERIC_CHAR	: [a-zA-Z$_./"-];

LIST						: '[' (ID (',' ID)*)? ']';
WS							: [ \t\n\r]+ -> skip;