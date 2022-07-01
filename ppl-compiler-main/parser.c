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
#include "Tree.h"
#include "parser.h"
#include "traverseParseTree.h"

TreeNode* createParseTree(Rule* grammar, TokenList* tokenStream) {
	//create stack
	Stack* st = createStack();
	//create tree root
	TreeNode* root = createNode("program", false);
	root -> depth = 0;
	//push start symbol program on to stack
		// printf("Pushed %s\n", root -> sym);
	push(st, root);
	//try applying all rules for program
	bool pass = false;
	for(int i = 0; i < NUM_RULES; i++) {
		if(strcmp(grammar[i].lhs, "PROGRAM") == 0) {
				// printf("Apply rule %d\n", i+1);
			pass = applyRule(grammar, i, tokenStream, tokenStream, st);
			if(pass) {
				strcpy(root -> lexeme, tokenStream -> lexeme);
				root -> line_no = tokenStream -> line_no;
				root -> r = &grammar[i];
				return root;
			}
		}
	}

	//check pass flag
		//if true, return tree root
		//if false, deallocate stack and tree report erroneous code
	printf("Parsing failed: Syntax error\n");
	return NULL;
}

bool applyRule(Rule* grammar, int rule_index, TokenList* tokenStream, TokenList* curr_ptr, Stack* st) {
	//pop top of stack
	TreeNode* lhs = pop(st);
	//add children to parse tree
	SymList* ruleNode = grammar[rule_index].rhs;
	while(ruleNode) {
		TreeNode* newNode = createNode(ruleNode -> val, ruleNode -> isTerminal);
		// printf("Added %s to %s\n", newNode -> sym, lhs -> sym);
		addChild(lhs, newNode);
		ruleNode = ruleNode -> next;
	}
	//push grammar[rule_index].rhs in reverse order
	int push_count = 0;
	TreeNode* currNode = lhs -> rightChild;
	while(currNode) {
		// printf("Pushed %s\n", currNode -> sym);
		push(st, currNode);
		push_count++;
		currNode = currNode -> leftSib;
	}
	//set flag pass to false
	bool pass = false;
	bool terminalPass = true;
	// if a terminal, try consuming
		//if success continue with new top of stack, may also be terminal
		//else return false
	Stack* flushSt = createStack();
	while(!isEmpty(st) && top(st) -> isTerminal) {
		if(curr_ptr -> token == top(st) -> sym) {
			push_count--;
			// printf("Popped %s (%s) [%d]\n", top(st) -> sym, curr_ptr -> lexeme, push_count);
			TreeNode* poppedStackNode = pop(st);
			strcpy(poppedStackNode -> lexeme, curr_ptr -> lexeme);
			poppedStackNode -> line_no = curr_ptr -> line_no;
			push(flushSt, poppedStackNode);
			curr_ptr = curr_ptr -> next;
		}
		else {
			// printf("Match failed. st: %s, tokenStream: %s\n", top(st) -> sym, curr_ptr -> token);
			terminalPass = false;
			break;
		}
	}
	if(terminalPass) {
		// if stack empty AND input completely read return true
		if(isEmpty(st) && curr_ptr == NULL) {
			return true;
		}
		//loop over grammar and try applying all possible rules
			// return true if flag is set at any point
		for(int i = 0; i < NUM_RULES; i++) {
			if(grammar[i].lhs == top(st) -> sym) {
				top(st) -> r = &grammar[i];
				// printf("Applied rule %d\n", i+1);
				top(st) -> line_no = tokenStream -> line_no;
				top(st) -> r = &grammar[i];
				bool res = applyRule(grammar, i, tokenStream, curr_ptr, st);
				if(res) {
					return true;
				}
				// printf("Rule %d failed\n", i+1);
			}
		}
	}

	while(!isEmpty(flushSt)) {
		TreeNode* poppedStackNode = pop(flushSt);
		push(st, poppedStackNode);
		push_count++;
	}
	//pop grammar[rule_index].rhs in sequential order
	while(push_count > 0) {
		pop(st);
		push_count--;
	}
	//remove children from tree
	deleteAllChildren(lhs);
	//push grammar[rule_index].lhs on stack
	push(st, lhs);
	//return false
	return false;
}

void printPreOrder(TreeNode* node, Rule* grammar) {
	printf("%-28s", node -> sym);
	if(node -> isTerminal)
		printf("%-10s", "YES");
	else
		printf("%-10s", "NO");
	if(!(node -> isTerminal) && (node -> tag != 4))
		printf("%-130s", printTypeExp(node -> t, node -> tag));
	else
		printf("%-130s", "***");
	if(node -> isTerminal)
		printf("%-25s", node -> lexeme);
	else
		printf("%-25s", "***");
	if(node -> isTerminal)
		printf("%-12d", node -> line_no);
	else
		printf("%-12s", "***");
	if(!(node -> isTerminal))
		printf("%-14ld", (node -> r) - grammar + 1);
	else
		printf("%-14s", "***");
	printf("%-5d\n", node -> depth);

	TreeNode* curr = node -> leftChild;
	while(curr) {
		printPreOrder(curr, grammar);
		curr = curr -> rightSib;
	}

	return;
}

void printParseTree(TreeNode* root, Rule* grammar) {
	printf("%-28s%-10s%-130s%-25s%-12s%-14s%-5s\n", "SYMBOL", "TERMINAL", "TYPE EXPRESSION", "LEXEME", "LINE NUM", "GRAMMAR RULE", "DEPTH");
	for(int i = 0; i < 225; i++)
		printf("-");
	printf("\n");
	printPreOrder(root, grammar);
	return;
}
