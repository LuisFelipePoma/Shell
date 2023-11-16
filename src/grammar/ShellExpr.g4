grammar ShellExpr;

start
    : command EOF
    ;

complete_command
    : list separator?
    ;

list
    : and_or (separator_op and_or)*
    ;

command
    : simple_command
	| compound_command 
    ;

simple_command 
    : cmd_word
    | cmd_name
    ;

compound_command
    : for_clause
	| if_clause
	| while_clause
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

do_group
    : DO compound_list DONE
    ;

and_or
    : pipeline (AND_IF | OR_IF) pipeline*
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
	| IF compound_list DO command* DONE;

else_part
    : ELSE IF compound_list DO else_part
    | ELSE compound_list;

while_clause     
    : WHILE compound_list do_group
    ;

term
    : and_or (separator and_or)*
    ;

/*expr
    : expr op = ('==' | '!=' | '<' | '>' | '<=' | '>=') expr
    | expr AND expr					
    | expr OR expr					
    | TRUE							
    | FALSE							
    | ID				
    | NUMBER			
    | '(' expr ')'					
    ;*/

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


//NAME: LETTER ( LETTER | DIGIT)*;
NUMBER: '-'? ( '.' DIGIT+ | DIGIT+ ( '.' DIGIT* )? );
WORD: LETTER ( LETTER | DIGIT)*;

fragment DIGIT
   : [0-9]
   ;

STRING: '"' ( '\\"' | . )*? '"';

fragment LETTER
   : [a-zA-Z\u0080-\u00FF_]
   ;
   
COMMENT
   : '#' ~[\r\n]* -> skip
   ;
LIST: '[' (STRING (',' STRING)*)? ']';

WS
   : [ \t\n\r]+ -> skip
   ;