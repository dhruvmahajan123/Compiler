#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

int check_kword(char str[]) {
  if(strlen(str)==1){
    if(str[0]=='+' || str[0]=='-' || str[0]=='/' || str[0]=='*' || 
       str[0]==';' || str[0]==':' || str[0]=='(' || str[0]==')' ||
       str[0]=='[' || str[0]==']' || str[0]=='{' || str[0]=='}' ||
       str[0]=='=')
      return 1;
    return 0;
  }
  if(strcmp(str,"program")==0)
    return 1; 
  if(strcmp(str,"integer")==0)
    return 1;
  if(strcmp(str,"real")==0)
    return 1;
  if(strcmp(str,"boolean")==0)
    return 1;
  if(strcmp(str,"array")==0)
    return 1;
  if(strcmp(str,"jagged")==0)
    return 1;
  if(strcmp(str,"declare")==0)
    return 1;
  if(strcmp(str,"list")==0)
    return 1;
  if(strcmp(str,"of")==0)
    return 1;
  if(strcmp(str,"variables")==0)
    return 1;
  if(strcmp(str,"size")==0)
    return 1;
  if(strcmp(str,"values")==0)
    return 1;
  if(strcmp(str,"&&&")==0)
    return 1;
  if(strcmp(str,"|||")==0)
    return 1;
  if(strcmp(str,"..")==0)
    return 1;
  if(strcmp(str,"R1")==0)
    return 1; 
  return 0;
}

TokenList* tokeniseSourcecode(FILE *fptr){
  TokenList* head;
  int line_no=1;
  head = (TokenList*)malloc(sizeof(TokenList));
  TokenList *temp = head;
  while(1){
    fscanf(fptr, "%[^ \n]", temp -> lexemes);
    char break_char = fgetc(fptr);

    temp -> line_no = line_no;

    if(strlen(temp -> lexemes) == 0) {
      if(break_char == '\n')
        line_no++;
      continue;
    }

    if(check_kword(temp -> lexemes))  //keyword check
      strcpy(temp -> tokens, temp -> lexemes);
    else if(temp -> lexemes[0] > 47 && temp -> lexemes[0] < 58) //identifiers cannot start with a number
      strcpy(temp -> tokens, "constant");
    else
      strcpy(temp -> tokens, "var");
    
    if(feof(fptr))
      return head;
    
    temp -> next = (TokenList*) malloc(sizeof(TokenList));
    temp = temp -> next;

    if(break_char == '\n') 
      line_no++;
  }
}

int main(){
    FILE* fptr=fopen("test.txt","r");
    TokenList *head=tokeniseSourcecode(fptr);
    fclose(fptr);
    int c = 0;

    while(head != NULL){
      printf("%d %s %s\n", head -> line_no, head -> lexemes, head -> tokens);
      head = head -> next;
      c++;
    }
    printf("Length of token stream: %d\n",c);

    //printf("%s",cd);
}