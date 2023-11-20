grammar ShellExpr;

start: command EOF;

list: and_or (separator and_or)*;

command:
	simple_command		# simpleStmt
	| compound_command	# compoundStmt;

compound_command:
	for_clause
	| if_clause
	| while_clause
	| brace_group;

simple_command:
	cmd_name args? io_redirect filename	# cmdIO
	| cmd_name args						# cmdArgs
	| cmd_name							# cmd
	| 'export' ID '=' (ID | WORD)		# export
	| 'let' ID '=' (ID | WORD | LIST)	# assing;

args: (ID | WORD)+;
filename: WORD+;

io_redirect:
	'<'
	| LESSAND
	| '>'
	| GREATAND
	| DGREAT
	| LESSGREAT
	| CLOBBER;

compound_list: and_or (separator and_or)* separator?;

for_clause:
	FOR (ID | WORD) IN (ID | WORD)+ do_group
	| FOR WORD do_group;

brace_group: LBRACE compound_list RBRACE;

do_group: DO compound_list DONE;

and_or: pipeline (AND_IF pipeline | OR_IF pipeline)*;

pipeline: command ('|' command)*;

if_clause:
	IF compound_list DO compound_list else_part DONE
	| IF compound_list do_group;

else_part:
	ELSE IF compound_list DO command* else_part
	| ELSE compound_list;

while_clause: WHILE compound_list do_group;

cmd_name: (WORD | ID);

separator: '&' | ';';

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

ID: LETTER ( LETTER | DIGIT)*;
WORD: (ALPHANUMERIC_CHAR | DIGIT)+;
NUMBER: '-'? ( '.' DIGIT+ | DIGIT+ ( '.' DIGIT*)?);

fragment DIGIT: [0-9];

fragment LETTER: [a-zA-Z];

fragment ALPHANUMERIC_CHAR: [a-zA-Z$_./"];

STRING: '"' ( '\\"' | .)*? '"';
LIST: '[' (WORD (',' WORD)*)? ']';
WS: [ \t\n\r]+ -> skip;