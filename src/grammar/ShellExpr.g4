grammar ShellExpr;

start
    : command EOF
    ;

complete_commands
    : complete_command (complete_command)*
    |                                
    ;

complete_command
    : list separator?
    ;

list
    : and_or (separator_op and_or)*
    ;

command
    : simple_command   # simpleStmt
	| compound_command # compoundStmt
    ;

compound_command
    : brace_group
    | for_clause
	| if_clause
	| while_clause
    ;

simple_command
    : cmd_prefix cmd_word cmd_suffix	# cmdPrefWordSuff
	| cmd_prefix cmd_word				# cmdPrefWord
	| cmd_prefix						# cmdPref
	| cmd_name cmd_suffix				# cmdSuff
	| cmd_name							# cmd
    ;
    
cmd_prefix       
    : (io_redirect | assignment_word) (cmd_prefix)?
    ;


cmd_suffix       
    : (io_redirect | WORD) (cmd_suffix)? # cmdSuffBody
    ;


io_redirect      
    :           io_file
    | IO_NUMBER io_file
    |           io_here
    | IO_NUMBER io_here
    ;

io_file
    : '<'       filename
	| LESSAND   filename
	| '>'       filename
	| GREATAND  filename
	| DGREAT    filename
	| LESSGREAT filename
	| CLOBBER   filename
    ;

filename 
    : WORD
    ;

io_here          
    : DLESS here_end
    | DLESSDASH here_end
    ;

here_end         
    : WORD
    ;
    
compound_list
    : term separator?
    ;

for_clause
    : FOR WORD do_group
	| FOR WORD IN wordlist do_group 
    ;

wordlist
    : WORD+
    ;

brace_group
    : LBRACE compound_list RBRACE
    ;

do_group
    : DO compound_list DONE
    ;

and_or
    : pipeline (AND_IF pipeline| OR_IF pipeline)*
    ;

pipeline
    : pipe_sequence
    /*| Bang pipe_sequence*/
    ;

pipe_sequence
    : command ('|' command)*
    ;


if_clause 
    : IF compound_list DO command* else_part DONE
	| IF compound_list DO command*           DONE
    ;

else_part
    : ELSE IF compound_list DO command* else_part
    | ELSE compound_list
    ;

while_clause     
    : WHILE compound_list do_group
    ;

term
    : and_or (separator and_or)*
    ;

cmd_name 
    : WORD;

cmd_word
    : WORD
    ;

separator_op
    : '&'
    | ';'
    ;    

separator 
    : separator_op
    ;

assignment_word
    : WORD '='
    ;

WHILE: 'while';
FOR: 'for';
IF: 'if';
ELSE: 'else';
AND_IF: '&&';
OR_IF: '||';
IN: 'in';
DO: 'do';
DONE: 'done';
TRUE: 'true';
FALSE: 'false';
LBRACE: '{';
RBRACE: '}';
DLESS: '<<';
DGREAT: '>>';
LESSAND: '<&';
GREATAND: '>&';
LESSGREAT: '<>';
DLESSDASH: '<<-';
CLOBBER: '>|';

WORD: (ALPHANUMERIC_CHAR | DIGIT)+ (ALPHANUMERIC_CHAR | DIGIT)*;
IO_NUMBER: DIGIT+;
ID: LETTER+ (LETTER | DIGIT)*;
NUMBER: '-'? ( '.' DIGIT+ | DIGIT+ ( '.' DIGIT* )? );

fragment DIGIT
   : [0-9]
   ;

fragment LETTER
   : [a-zA-Z.]
   ;

fragment ALPHANUMERIC_CHAR
    : [a-zA-Z$_=.]
    ;

STRING: '"' ( '\\"' | . )*? '"';
LIST: '[' (STRING (',' STRING)*)? ']';
WS: [ \t\n\r]+ -> skip;