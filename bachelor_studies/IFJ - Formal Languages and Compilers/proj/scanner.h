/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2014/2015      **|
|**Subor : scanner.h                                     **|
|**Popis : Hlavickovy subor k modulu scanner_new.c       **|
|**Autori:                                               **|
|**                                                      **|
|**       Tomas Poremba     xporem00                     **|
|**       Ladislav Sulak    xsulak04                     **|
|**********************************************************/

#ifndef SCANNER_H
#define SCANNER_H

extern int lineNumber;

#define RET_TOKEN_NEQAL(tok, err)\
    if(token != (tok))\
        return (err)

#define GET_TOKEN\
    if((token=getToken(&attr)) == T_ERR_LEX) \
        return ERR_LEX

typedef enum {    
    //Data types
    T_IDENTIFIER = 10,
    
    T_INTEGER,
    T_REAL,
    T_STRING,
    T_BOOLEAN,
    
    T_INTEGER_NUM, //16
    T_DOUBLE_NUM,
    T_STR_LIT,
    T_STR_LIT_EMPTY,
    
    //Keywords
    T_BEGIN, //20
    T_END,
    T_VAR,
    
    T_WHILE,
    T_DO,
    
    T_IF, //25
    T_THEN,
    T_ELSE,
    
    T_TRUE,
    T_FALSE,
    
    T_WRITE, //30
    T_READLN,
    T_SORT,
    T_FIND,
    T_COPY,
    T_LENGTH,
    T_FORWARD,
    T_FUNCTION,
    
    //Operators
    T_PLUS, //36
    T_MINUS,
    T_MULTIPLICATION,
    T_DIVISION,
    
    T_MORE,         //40 '>'
    T_LESS,         //41 '<'
    T_MORE_EQUAL,   //42 '>='
    T_LESS_EQUAL,    //43 '<='
    T_EQUAL,        //44 '='  NOT ASSIGN! Use with comparison
    T_NOT_EQUAL,    //45 '<>'
    
    T_ASSIGN,         //46 := 
    
    //Characters
    T_POINT,        //47 '.' 
    T_COLON,        //48 ':' 
    T_COMMA,        //49 ',' 
    T_SEMICOLON,    //50 ';' 

    T_L_VINCULUM,    //51 '{' 
    T_R_VINCULUM,    //52 '}' 
    T_L_BRACKET,    //53 '('
    T_R_BRACKET,    //54 ')' 
    T_L_SQ_BRACKET,    //55 '[' 
    T_R_SQ_BRACKET,    //56 ']'

    T_BACKSLASH,    //57 '\'

    T_EOF_S,            

    T_ERR_LEX,

} token_t;


//Enum for naming states in FSM
typedef enum {    
    START,
    COMMENT,
    ID_KEYWORD,
    INT_NUMBER,
    POSS_ASSIGN,
    POSS_L_EQ,
    POSS_M_EQ,
    STRING_ST,
    POSS_ESCAPE,
    ASCII_ESCAPE,
    ASCII_NUM,
    REAL_NUMBER_POINT,
    REAL_NUMBER,
    EXPONENT_SIGN,
    EXPONENT_NUMBER,
    EXPONENT
} fsm_state_t;


//Functions
int openFile(char *fileParam);
void closeFile();
token_t getToken(string *attr);

#endif
