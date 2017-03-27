/*
 * Copyright (C) 2005 National Association of REALTORS(R)
 *
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished
 * to do so, provided that the above copyright notice(s) and this
 * permission notice appear in all copies of the Software and that
 * both the above copyright notice(s) and this permission notice
 * appear in supporting documentation.
 */
header "post_include_hpp"
{
#include "librets/RetsAST.h"
}

options
{
    language="Cpp";
    namespace = "librets";
    namespaceStd = "std";
    namespaceAntlr = "antlr";
}

class RetsSqlParser extends Parser;

options
{
    exportVocab = RetsSql;
    k = 3;
    ASTLabelType = "RefRetsAST";
    buildAST = true;
    defaultErrorHandler = false;
}

tokens
{
    SELECT = "select"; FROM = "from"; WHERE = "where";
    OR = "or"; AND = "and"; NOT = "not"; ORDER = "order"; BY = "by"; AS = "as";
    IN_ = "in"; LIMIT = "limit"; OFFSET = "offset"; COUNT = "count";
    COLUMNS; COLUMN; QUERY_ELEMENT; TABLE; NEQ;
}

{
  public: antlr::RefToken getTableName() { return mTableName; };
  private: antlr::RefToken mTableName;
}

sql_statements
    : (sql_statement)* EOF
    ;

sql_statement
    : sql_command (SEMI!)?
    ;

sql_command
    : select_statement
    ;

select_statement
    :! SELECT^ c:column_names
        FROM! t:table_name
        (WHERE! w:where_condition)?
        (order_by)?
        (LIMIT l:limit)?
        (OFFSET o:offset)?
        { #select_statement = #([SELECT], #t, #c, #w, #l, #o); }
    ;

column_names
    : STAR { #column_names = #([COLUMNS, "COLS"]); }
    | column_name (COMMA! column_name)*
        { #column_names = #([COLUMNS, "COLS"], #column_names); }
    | count_clause
    ;

column_name!
    : id:ID         { #column_name = #([COLUMN, "COL"], #[ID, ""], #id);  }
    | table:ID DOT col:ID
        { #column_name = #([COLUMN, "COL"], #table, #col); }
    ;

table_name!
{ RefRetsAST tableAst; }
    : id:ID 
        { mTableName = id;
          // Must clone #id, rather than use #(... #id, #id) which
          // creates an infinite loop.  I'm sure there is a better way
          // to clone #id, but I can't figure it out.
          #table_name = #([TABLE, "TABLE"], #id, #[ID, #id->getText()]); }
    | table:ID (AS)? alias:ID
        { mTableName = table;
          #table_name = #([TABLE, "TABLE"], #table, #alias); }
    ;

order_by!
    : ORDER BY column_name (COMMA column_name)*
    ;

count_clause
    : COUNT LPAREN! STAR! RPAREN!
    ;

limit
    : l:INT
      { #limit = #([LIMIT, "LIMIT"], #l); }
    ;

offset!
    : o:INT
      { #offset = #([OFFSET], #o); }
    ;

where_condition
    : and_clause (OR^ and_clause)*
    ;

and_clause
    : not_clause (AND^ not_clause)*
    ;

not_clause
    : NOT^ query_element
    | query_element
    ;

query_element
    : LPAREN! where_condition RPAREN!
    |! c:column_name cc:column_condition[#c] {#query_element = #cc; }
    ;

column_condition [RefRetsAST c]
    :! o:numeric_operator v:field_value
        { #column_condition = #([QUERY_ELEMENT, "QE"], #c, (#o, #v)); }
    |! IN_ fvl:field_value_list
        { #column_condition = #([IN_], #c, (#fvl)); }
    |! NOT IN_ nfvl:field_value_list
        { #column_condition = #([NOT], #([IN_], #c, (#nfvl))); }
    ;

field_value_list
    : LPAREN! field_value (COMMA! field_value)* RPAREN!
    ;

numeric_operator
    : (EQ | LESS | LTE | GREATER | GTE
        | NEQ1 { #numeric_operator = #([NEQ]); }
        | NEQ2 { #numeric_operator = #([NEQ]); })
    ;

field_value
    : ID
    | INT
    | STRING
    | QMARK
    ;


class RetsSqlLexer extends Lexer;

options
{
    k = 2;
    testLiterals = false;
    caseSensitiveLiterals = false;
    exportVocab = RetsSql;
}

WS_ :	(' '
	|	'\t' { tab(); }
	|	'\n' { newline(); }
	|	'\r')
		{ $setType(antlr::Token::SKIP); }
	;

LPAREN  : '(' ;
RPAREN  : ')' ;
STAR    : '*' ;
PLUS    : '+' ;
EQ      : '='  ;
NEQ1    : "!=" ;
NEQ2    : "<>";
GREATER : ">" ;
GTE     : ">=" ;
LESS    : "<"  ;
QMARK  : '?' ;
LTE  options { paraphrase = "<="; }             : "<=" ;
SEMI options { paraphrase = "a semicolon"; }    : ';' ;
COMMA options { paraphrase = ","; }             : ',' ;
DOT options { paraphrase = "."; }               : '.' ;

COMMENT
    : "--" (~('\n'|'\r'))* ('\n'|'\r'('\n')?)?
        {
            $setType(antlr::Token::SKIP);
            newline();
        }
    ;
  
protected
DIGIT
	:	'0'..'9'
	;

protected
ALPHA
    : ('a'..'z' | 'A'..'Z')
    ;

INT
    : (DIGIT)+
	;

ID
options
{
    paraphrase = "an identifier";
}
    : (ALPHA | '_') (ALPHA | DIGIT | '_' | ':')*
        {
            $setType(testLiteralsTable(ID));
        }
    | '\"'! (~'\"')* '\"'!
	;

STRING
    : '\''! (~'\'')* '\''!
    ;
