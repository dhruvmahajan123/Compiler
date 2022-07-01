/*
Group No.26
Arshit Modi      2018A7PS0191P
Devanshu         2018A7PS0194P
Guntaas Singh    2018A7PS0269P
Siddarth Agrawal 2018A7PS0359P
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"
#define CODE_BUFF_SIZE 300
char* getTokenName(char str[], bool forGrammar, bool* isTerminal) {
  if(isTerminal && !forGrammar) {
    printf("getTokenName: Must set forGrammar if want to check symbol type (isTerminal).\n");
    return NULL;
  }
  if(forGrammar) {
    if(isTerminal)
      *isTerminal = false;
    if(strcmp(str, "PROGRAM") == 0)
      return "PROGRAM";
    if(strcmp(str, "PROGRAMBODY") == 0)
      return "PROGRAMBODY";
    if(strcmp(str, "DECLARATION_LIST") == 0)
      return "DECLARATION_LIST";
    if(strcmp(str, "ASSIGNMENT_LIST") == 0)
      return "ASSIGNMENT_LIST";
    if(strcmp(str, "DECLARATION_STMT") == 0)
      return "DECLARATION_STMT";
    if(strcmp(str, "ASSIGNMENT_STMT") == 0)
      return "ASSIGNMENT_STMT";
    if(strcmp(str, "PRIM_DECLARATION_STMT") == 0)
      return "PRIM_DECLARATION_STMT";
    if(strcmp(str, "RECTARR_DECLARATION_STMT") == 0)
      return "RECTARR_DECLARATION_STMT";
    if(strcmp(str, "JAGGARR_DECLARATION_STMT") == 0)
      return "JAGGARR_DECLARATION_STMT";
    if(strcmp(str, "PRIM_TYPE") == 0)
      return "PRIM_TYPE";
    if(strcmp(str, "ID_LIST") == 0)
      return "ID_LIST";
    if(strcmp(str, "RANGE_LIST") == 0)
      return "RANGE_LIST";
    if(strcmp(str, "RANGE") == 0)
      return "RANGE";
    if(strcmp(str, "RANGE_VAL") == 0)
      return "RANGE_VAL";
    if(strcmp(str, "JAGGARR2D_DECLARATION_STMT") == 0)
      return "JAGGARR2D_DECLARATION_STMT";
    if(strcmp(str, "JAGGARR3D_DECLARATION_STMT") == 0)
      return "JAGGARR3D_DECLARATION_STMT";
    if(strcmp(str, "JAGGARR2D_INIT_LIST") == 0)
      return "JAGGARR2D_INIT_LIST";
    if(strcmp(str, "JAGGARR2D_INIT_STMT") == 0)
      return "JAGGARR2D_INIT_STMT";
    if(strcmp(str, "JAGGARR2D_VAL_LIST") == 0)
      return "JAGGARR2D_VAL_LIST";
    if(strcmp(str, "JAGGARR3D_INIT_LIST") == 0)
      return "JAGGARR3D_INIT_LIST";
    if(strcmp(str, "JAGGARR3D_INIT_STMT") == 0)
      return "JAGGARR3D_INIT_STMT";
    if(strcmp(str, "JAGGARR3D_ROW_LIST") == 0)
      return "JAGGARR3D_ROW_LIST";
    if(strcmp(str, "JAGGARR3D_VAL_LIST") == 0)
      return "JAGGARR3D_VAL_LIST";
    if(strcmp(str, "EXPRESSION") == 0)
      return "EXPRESSION";
    if(strcmp(str, "BOOL_EXPRESSION") == 0)
      return "BOOL_EXPRESSION";
    if(strcmp(str, "TERM") == 0)
      return "TERM";
    if(strcmp(str, "FACTOR") == 0)
      return "FACTOR";
    if(strcmp(str, "INDEX_LIST") == 0)
      return "INDEX_LIST";
    if(strcmp(str, "INDEX") == 0)
      return "INDEX";
    if(strcmp(str, "BOOL_TERM") == 0)
      return "BOOL_TERM";
    if(strcmp(str, "ARR_ID") == 0)
      return "ARR_ID";
    if(isTerminal)
      *isTerminal = true;
    if(strcmp(str, "CONSTANT") == 0)
      return "CONSTANT";
  }

  if(strlen(str)==1){
    if(str[0]=='+')
      return "TK_PLUS";
    if(str[0]=='-')
      return "TK_MINUS";
    if(str[0]=='/')
      return "TK_DIV";
    if(str[0]=='*')
      return "TK_STAR";
    if(str[0]==';')
      return "TK_SEMICOLON";
    if(str[0]==':')
      return "TK_COLON";
    if(str[0]=='(')
      return "BR_OP";
    if(str[0]==')')
      return "BR_CL";
    if(str[0]=='[')
      return "SQ_OP";
    if(str[0]==']')
      return "SQ_CL";
    if(str[0]=='{')
      return "CR_OP";
    if(str[0]=='}')
      return "CR_CL";
    if(str[0]=='=')
      return "TK_EQUALS";
  }

  if(strcmp(str,"program")==0)
    return "TK_PROGRAM";
  if(strcmp(str,"integer")==0)
    return "TK_INTEGER";
  if(strcmp(str,"real")==0)
    return "TK_REAL";
  if(strcmp(str,"boolean")==0)
    return "TK_BOOLEAN";
  if(strcmp(str,"array")==0)
    return "TK_ARRAY";
  if(strcmp(str,"jagged")==0)
    return "TK_JAGGED";
  if(strcmp(str,"declare")==0)
    return "TK_DECLARE";
  if(strcmp(str,"list")==0)
    return "TK_LIST";
  if(strcmp(str,"of")==0)
    return "TK_OF";
  if(strcmp(str,"variables")==0)
    return "TK_VARIABLES";
  if(strcmp(str,"size")==0)
    return "TK_SIZE";
  if(strcmp(str,"values")==0)
    return "TK_VALUES";

  if(strcmp(str,"&&&")==0)
    return "TK_AND";
  if(strcmp(str,"|||")==0)
    return "TK_OR";
  if(strcmp(str,"..")==0)
    return "TK_DOTDOT";

  if(strcmp(str,"R1")==0)
    return "TK_ROW";

  if(str[0] > 47 && str[0] < 58)
    return "CONSTANT";

  return "ID";
}


TokenList* tokeniseSourcecode(FILE *fptr){
  TokenList* head = NULL;
  int line_no=1;
  head = (TokenList*)malloc(sizeof(TokenList));
  TokenList *temp = NULL;
  char codeBuffer[CODE_BUFF_SIZE];
  int cnt=0;
  while(fgets(codeBuffer, CODE_BUFF_SIZE, fptr)) {
    int clen = strlen(codeBuffer);
    int cptr = 0;
  //  printf("%s%ld\n",codeBuffer,strlen(codeBuffer));


  cnt++;
  if(clen>0 &&codeBuffer[clen-1]=='\n')
  clen=clen-1;

    while(cptr < clen) {
      TokenList *trial;
      if(temp) {
        temp -> next = (TokenList*) malloc(sizeof(TokenList));
        trial=temp;
        temp = temp -> next;
      } else {
        head = temp = (TokenList*) malloc(sizeof(TokenList));
      }
      sscanf(codeBuffer + cptr, "%s", temp->lexeme);
      if(strlen(temp->lexeme)==0){
        free(temp);
        trial->next=NULL;
        temp=trial;
        break;
      }
      int i=0;
      while(codeBuffer[i+cptr]!=temp->lexeme[0]){
        i++;
      }

      cptr += strlen(temp -> lexeme)+i+1;
      temp -> line_no = line_no;
      temp -> token = getTokenName(temp -> lexeme, false, NULL);
    }
    line_no++;
    //fgetc(fptr);
    if(feof(fptr))
      break;
  }
  return head;
}

void printTokenStream(TokenList* head){
    int c = 0;
    while(head != NULL){
      printf("%d %s %s\n", head -> line_no, head -> lexeme, head -> token);
      head = head -> next;
      c++;
    }
    printf("Length of token stream: %d\n",c);
}
// int main(){
//   FILE* fptr=fopen("t5.txt","r");
//   TokenList* head=tokeniseSourcecode(fptr);
//  printTokenStream(head);
//   fclose(fptr);

// }
