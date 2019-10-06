/**********************************************************|
|**        Projekt do predmetov IFJ a IAL 2014/2015      **|
|**Subor : scanner_new.c                                 **|
|**Popis : Obsahuje implementaciu konecneho automatu     **|
|**        ktory obstarava lexikalnu analyzu             **|
|**Autori:                                               **|
|**                                                      **|
|**       Tomas Poremba     xporem00                     **|
|**********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "interpret.h"
#include "scanner.h"
#include "str.h"

FILE *f = NULL;
int lineNumber = 1;

#define UNGETC(a,b) {ungetc((a),(b)); if((a) == '\n') lineNumber--;}
int openFile(char *fileParam)
{
    if ((f = fopen(fileParam, "r")) == NULL)
        return ERR_INTERN;

    return SUCCESS;
}

void closeFile()
{
    fclose(f);    
}

//Function returns integer - token number, or -1 on error
token_t getToken(string *attr)
{
    int c;
    fsm_state_t state = START;
    int ascii = 0;

    strClear(attr);
    while(1)
    {
        c = getc(f);

        if(c == '\n')
            lineNumber++;

        switch(state)
        {
            case START:
                {
                    if (isspace(c))
                        ;

                    else if (isalpha(c) || c == '_') //zaciatok T_IDENTIFIER
                    {
                        if (isalpha(c))
                            c = toupper(c); //case insensitive
                        
                        if(strAddChar(attr,c) != STR_SUCCESS)
                            exit(ERR_INTERN);
                        state = ID_KEYWORD;
                    }
                    else if (isdigit(c))
                    {
                        if(strAddChar(attr,c) != STR_SUCCESS)
                            exit(ERR_INTERN);
                        state = INT_NUMBER;
                    }
                    else if (c == '{')
                        state = COMMENT;

                    else if (c == ':')
                        state = POSS_ASSIGN;

                    else if (c == '<')
                        state = POSS_L_EQ;

                    else if (c == '>')
                        state = POSS_M_EQ;

                    else if (c == '\'')
                        state = STRING_ST;

                    else if (c == '.')
                        return T_POINT;

                    else if (c == ';')
                        return T_SEMICOLON;

                    else if (c == '(')
                        return T_L_BRACKET;

                    else if (c == ')')
                        return T_R_BRACKET;

                    else if (c == '}')
                        return T_R_VINCULUM;

                    else if (c == '+')
                        return T_PLUS;

                    else if (c == '-')
                        return T_MINUS;

                    else if (c == '*')
                        return T_MULTIPLICATION;

                    else if (c == '/')
                        return T_DIVISION;

                    else if (c == '=')
                        return T_EQUAL;

                    else if (c == ',')
                        return T_COMMA;

                    else if (c == EOF)
                        return T_EOF_S;
                    
                    else
                        return T_ERR_LEX;

                }
                break;

            case COMMENT:
                {
                    if (c == '}')
                        state = START;

                    else if (c == EOF)
                        return T_ERR_LEX;

                    else
                    {
                        ;
                    }
                }
                break;

            case POSS_ASSIGN:
                {
                    if (c == '=')
                        return T_ASSIGN;

                    else
                    {
                        UNGETC(c, f);
                        return T_COLON;
                    }
                }
                break;

            case POSS_L_EQ:
                {
                    if (c == '=')
                        return T_LESS_EQUAL;

                    else if (c == '>')
                        return T_NOT_EQUAL;
                    else
                    {
                        UNGETC(c, f);
                        return T_LESS;
                    }
                }
                break;

            case POSS_M_EQ:
                {
                    if (c == '=')
                        return T_MORE_EQUAL;
                    else
                    {
                        UNGETC(c, f);
                        return T_MORE;
                    }
                }
                break;

            case ID_KEYWORD:
                {
                    if (isalnum(c) || c == '_')
                    {
                        if (isalpha(c))
                            c = toupper(c);

                        if(strAddChar(attr, c) != STR_SUCCESS)
                            exit(ERR_INTERN);
                    }
                    else
                    {
                        UNGETC(c, f);

                        if(strCmpConstStr(attr, "BEGIN") == 0)
                            return T_BEGIN;

                        else if(strCmpConstStr(attr, "BOOLEAN") == 0)
                            return T_BOOLEAN;

                        else if(strCmpConstStr(attr, "COPY") == 0)
                            return T_COPY;

                        else if(strCmpConstStr(attr, "DO") == 0)
                            return T_DO;

                        else if(strCmpConstStr(attr, "ELSE") == 0)
                            return T_ELSE;

                        else if(strCmpConstStr(attr, "END") == 0)
                            return T_END;

                        else if(strCmpConstStr(attr, "FALSE") == 0)
                            return T_FALSE;

                        else if(strCmpConstStr(attr, "FIND") == 0)
                            return T_FIND;

                        else if(strCmpConstStr(attr, "FORWARD") == 0)
                            return T_FORWARD;

                        else if(strCmpConstStr(attr, "FUNCTION") == 0)
                            return T_FUNCTION;

                        else if(strCmpConstStr(attr, "IF") == 0)
                            return T_IF;

                        else if(strCmpConstStr(attr, "INTEGER") == 0)
                            return T_INTEGER;

                        else if(strCmpConstStr(attr, "LENGTH") == 0)
                            return T_LENGTH;

                        else if(strCmpConstStr(attr, "READLN") == 0)
                            return T_READLN;

                        else if(strCmpConstStr(attr, "REAL") == 0)
                            return T_REAL;

                        else if(strCmpConstStr(attr, "SORT") == 0)
                            return T_SORT;

                        else if(strCmpConstStr(attr, "STRING") == 0)
                            return T_STRING;

                        else if(strCmpConstStr(attr, "THEN") == 0)
                            return T_THEN;

                        else if(strCmpConstStr(attr, "TRUE") == 0)
                            return T_TRUE;

                        else if(strCmpConstStr(attr, "VAR") == 0)
                            return T_VAR;

                        else if(strCmpConstStr(attr, "WHILE") == 0)
                            return T_WHILE;

                        else if(strCmpConstStr(attr, "WRITE") == 0)
                            return T_WRITE;

                        else
                            return T_IDENTIFIER;
                    }
                }
                break;

            case INT_NUMBER: //uz prislo 12
                {
                    if (isdigit(c))
                    {
                        if(strAddChar(attr, c) != STR_SUCCESS)
                            exit(ERR_INTERN);
                    }
                    else if (c == '.')
                    {
                        if(strAddChar(attr, c) != STR_SUCCESS)
                            exit(ERR_INTERN);
                        state = REAL_NUMBER_POINT; //prave prisla bodka, dalsie MUSI byt cislo
                    }
                    else if (c == 'e' || c == 'E')
                    {
                        if(strAddChar(attr, c) != STR_SUCCESS)
                            exit(ERR_INTERN);
                        state = EXPONENT_SIGN;
                    }
                    else if (isalpha(c) || c == '_')
                        return T_ERR_LEX;
                    else
                    {
                        UNGETC(c, f);
                        return T_INTEGER_NUM;
                    }
                }
                break;

            case REAL_NUMBER_POINT: //uz prislo 12.
                {
                    if (isdigit(c))
                    {
                        if(strAddChar(attr, c) != STR_SUCCESS)
                            exit(ERR_INTERN);
                        state = REAL_NUMBER;
                    }
                    else
                        return T_ERR_LEX;
                }
                break;
            
            case REAL_NUMBER: //uz prislo 12.3
                {
                    if (isdigit(c))
                    {
                        if(strAddChar(attr, c) != STR_SUCCESS)
                            exit(ERR_INTERN);
                    }
                    else if (c == 'e' || c == 'E')
                    {
                        if(strAddChar(attr, c) != STR_SUCCESS)
                            exit(ERR_INTERN);
                        state = EXPONENT_SIGN;
                    }
                    else if (c == '.' || c == '_' || isalpha(c))
                        return T_ERR_LEX;

                    else
                    {
                        UNGETC(c, f);
                        return T_DOUBLE_NUM;
                    }
                }
                break;

            case EXPONENT_SIGN:    //uz prislo 12.3e
                {
                    if (isdigit(c))
                    {
                        if(strAddChar(attr, c) != STR_SUCCESS)
                            exit(ERR_INTERN);
                        state = EXPONENT;
                    }
                    else if (c == '+' || c == '-')
                    {
                        if(strAddChar(attr, c) != STR_SUCCESS)
                            exit(ERR_INTERN);
                        state = EXPONENT_NUMBER;    //dalsi znak MUSI byt cislo
                    }
                    else
                        return T_ERR_LEX;
                }
                break;
                
            case EXPONENT_NUMBER:
                {
                    if (isdigit(c))
                    {
                        if(strAddChar(attr, c) != STR_SUCCESS)
                            exit(ERR_INTERN);
                        state = EXPONENT;
                    }
                    else
                        return T_ERR_LEX;
                }
                break;

            case EXPONENT: //uz prislo 12.3e+1
                {
                    if (isdigit(c))
                    {
                        if(strAddChar(attr, c) != STR_SUCCESS)
                            exit(ERR_INTERN);
                    }
                    else if (isalpha(c) || c == '.' || c == '_')
                        return T_ERR_LEX;

                    else
                    {
                        UNGETC(c, f);
                        return T_DOUBLE_NUM;
                    }
                }
                break;
                
            case STRING_ST:
                {
                    if (c == '\n')
                        return T_ERR_LEX;

                    else if (c == '\'')
                        state = POSS_ESCAPE;
                    else
                        if(c > 31)
                        {
                            if(strAddChar(attr, c) != STR_SUCCESS)
                                exit(ERR_INTERN);
                        }
                        else
                            return T_ERR_LEX;
                }
                break;

            case POSS_ESCAPE:
                {
                    if (c == '#')
                    {
                        ascii = 0;
                        state = ASCII_ESCAPE;
                    }
                    
                    else if (c == '\'')
                    {
                        if(strAddChar(attr, c) != STR_SUCCESS)
                            exit(ERR_INTERN);
                        state = STRING_ST;
                    }
                    else
                    {
                        UNGETC(c, f);
                        return T_STR_LIT;
                    }
                }
                break;

            case ASCII_ESCAPE:
                {
                    if (isdigit(c))
                    {
                        ascii = ascii*10 + c - '0';
                        state = ASCII_NUM;
                    }
                    else
                        return T_ERR_LEX;
                }
                break;

            case ASCII_NUM:
                {
                    if (isdigit(c))
                    {
                        ascii = ascii*10 + c - '0';
                    }
                    else if (c == '\'')
                    {
                        if(ascii > 0 && ascii <= 255)
                        {
                            if(strAddChar(attr, ascii) != STR_SUCCESS)
                                exit(ERR_INTERN);
                            state = STRING_ST;
                        }
                        else
                            return T_ERR_LEX;
                    }
                    else
                        return T_ERR_LEX;
                }
        }
    }
}
