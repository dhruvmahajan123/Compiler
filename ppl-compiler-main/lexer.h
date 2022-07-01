/*
Group No.26
Arshit Modi      2018A7PS0191P
Devanshu         2018A7PS0194P
Guntaas Singh    2018A7PS0269P
Siddarth Agrawal 2018A7PS0359P
*/
#ifndef LEXER_H
#define LEXER_H
#define LEXEME_LEN 25

typedef struct TokenList {
  int line_no;
  char lexeme[LEXEME_LEN];
  char* token;	// pointer to string literal
  struct TokenList* next;
} TokenList;

char* getTokenName(char str[], bool checkNonTerminals, bool* isTerminal);
TokenList* tokeniseSourcecode(FILE *fptr);
void printTokenStream(TokenList* head);
#endif
