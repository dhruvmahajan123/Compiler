/*
Group No.26
Arshit Modi      2018A7PS0191P
Devanshu         2018A7PS0194P
Guntaas Singh    2018A7PS0269P
Siddarth Agrawal 2018A7PS0359P
*/
#ifndef STACK_H
#define STACK_H
#include "Tree.h"

typedef struct StackNode {
  TreeNode* data;
  struct StackNode* next;
} StackNode;

typedef struct Stack {
	StackNode* top;
	int size;
} Stack;

Stack* createStack(void);
void push(Stack* st, TreeNode* data);
TreeNode* pop(Stack* st);
TreeNode* top(Stack* st);
bool isEmpty(Stack* st);
#endif
