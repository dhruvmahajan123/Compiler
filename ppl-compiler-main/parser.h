/*
Group No.26
Arshit Modi      2018A7PS0191P
Devanshu         2018A7PS0194P
Guntaas Singh    2018A7PS0269P
Siddarth Agrawal 2018A7PS0359P
*/
#ifndef PARSER_H
#define PARSER_H

TreeNode* createParseTree(Rule* grammar, TokenList* tokenStream);
bool applyRule(Rule* grammar, int rule_index, TokenList* tokenStream, TokenList* curr_ptr, Stack* st);
void printParseTree(TreeNode* root, Rule* grammar);

#endif
