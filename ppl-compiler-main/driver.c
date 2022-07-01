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


int main(int argc, char* argv[]) {
	if(argc != 2) {
		printf("Usage: ./driver <path to source code>\n");
		return 0;
	}
	Rule* grammar;
	TreeNode* root = NULL;
	TypeExpTable* table = NULL;
	while(true) {
		printf(">> ");
		int input;
		scanf("%d", &input);

		if(input == 0) {
			break;
		} else if(input == 1) {
			// read grammar from grammar.txt
			char* grammar_path = "grammar.txt";
			int num_rules = 62;
			grammar = readGrammar(grammar_path, num_rules);

			// tokenise source code to get token stream
			FILE* fptr=fopen(argv[1], "r");
		    TokenList* tokenStream=tokeniseSourcecode(fptr);
		    fclose(fptr);
		    // printTokenStream(tokenStream);
		    
		    // create parse tree
		    root = createParseTree(grammar, tokenStream);

	    	printf("Parse tree created successfully.\n");
		} else if(input == 2) {
			if(root) {
				table = traverseParseTree(root);
			} else {
				printf("Parse tree yet to be created. Use option 1.\n");
			}
		} else if(input == 3) {
			if(root) {
				printParseTree(root, grammar);
			} else {
				printf("Parse tree yet to be created. Use option 1.\n");
			}
		} else if(input == 4) {
			if(table) {
				printTypeExpressionTable(table);
			} else {
				printf("Type expression table yet to be populated. Use option 2.\n");
			}
		} else {
			printf("Invalid option.\n");
		}
	  }
}
