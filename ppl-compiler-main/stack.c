/*
Group No.26
Arshit Modi      2018A7PS0191P
Devanshu         2018A7PS0194P
Guntaas Singh    2018A7PS0269P
Siddarth Agrawal 2018A7PS0359P
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "grammar.h"
#include "lexer.h"
#include "stack.h"

Stack* createStack(void) {
  Stack* st = malloc(sizeof(Stack));
  st -> top = NULL;
  st -> size = 0;
  return st;
}

void push(Stack* st, TreeNode* data){
  StackNode* newNode = (StackNode*) malloc(sizeof(StackNode));
  newNode -> data = data;
  newNode -> next = st -> top;
  st -> top = newNode;
  (st -> size)++;
}

TreeNode* pop(Stack* st){
  if(st -> top == NULL){
    return NULL;
  }
  StackNode* poppedNode = st -> top;
  TreeNode* poppedData = poppedNode -> data;
  st -> top = poppedNode -> next;
  (st -> size)--;
  free(poppedNode);
  return poppedData;
}

TreeNode* top(Stack* st) {
  return st -> top -> data;
}

bool isEmpty(Stack* st) {
  return (st -> size == 0);
}
