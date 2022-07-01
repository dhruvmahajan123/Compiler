/*
Group No.26
Arshit Modi      2018A7PS0191P
Devanshu         2018A7PS0194P
Guntaas Singh    2018A7PS0269P
Siddarth Agrawal 2018A7PS0359P
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "grammar.h"
#include "lexer.h"
#include "Tree.h"

TreeNode* createNode(char* sym, bool isTerminal) {
  TreeNode* newNode = (TreeNode*) malloc(sizeof(TreeNode));
  newNode -> sym = sym;
  newNode -> isTerminal = isTerminal;
  newNode -> tag = 4;
  newNode -> parent = NULL;
  newNode -> leftChild = NULL;
  newNode -> rightChild = NULL;
  newNode -> leftSib = NULL;
  newNode -> rightSib = NULL;
  return newNode;
}

void addChild(TreeNode* parent, TreeNode* child){
  if(parent -> rightChild) {
    parent -> rightChild -> rightSib = child;
    child -> leftSib = parent -> rightChild;
  } else {
    parent -> leftChild = child;
  }
  parent -> rightChild = child;
  child -> parent = parent;
  child -> depth = parent -> depth + 1;
  return;
}

void deleteAllChildren(TreeNode* p) {
    TreeNode* temp = p -> leftChild;
    TreeNode* extra = temp;
    while(temp != NULL) {
        temp = temp -> rightSib;
        free(extra);
        extra = temp;
    }
    p -> leftChild = NULL;
    p -> rightChild = NULL;
}
