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
#include "grammar.h"
#include "lexer.h"
#include "stack.h"
#include "Tree.h"
#include "parser.h"
#include "traverseParseTree.h"

TypeExpTable* traverseParseTree(TreeNode* root) {
  printf("ERRORS:\n");
  printf("%-10s%-15s%-10s%-20s%-130s%-20s%-130s%-8s%s\n", "LINE NUM", "CATEGORY", "OPERATOR",
    "LEXEME OP1", "TYPE OP1", "LEXEME OP2", "TYPE OP2", "DEPTH", "MESSAGE");
  traverseDecList(root);
  TypeExpTable* table = (TypeExpTable*) malloc(sizeof(TypeExpTable));
  populateTable(root, table);
  traverseAsgList(root, table);
  return table;
}

void traverseDecList(TreeNode* root) {
    TreeNode* programBody = root -> rightChild -> leftSib;
    TreeNode* decList = programBody -> leftChild;
    while(decList -> leftChild != decList -> rightChild)
    {
        processDecStmt(decList -> leftChild);  // process decStmt
        decList = decList -> rightChild; //move to next decListNode
    }
    processDecStmt(decList -> leftChild); //process last decStmt
}

void processDecStmt(TreeNode* decStmt) {
  if(strcmp(decStmt -> leftChild -> sym , "PRIM_DECLARATION_STMT")==0) {
    decStmt -> leftChild -> tag = 0; //setting tag to prim
    processPrimDecStmt(decStmt -> leftChild);
  } else if(strcmp(decStmt -> leftChild -> sym, "RECTARR_DECLARATION_STMT")==0) {
    decStmt -> leftChild -> tag = 1;  // setting tag to rect
    processRectDecStmt(decStmt -> leftChild); //pending: variable
  } else if(strcmp(decStmt -> leftChild -> leftChild -> sym, "JAGGARR2D_DECLARATION_STMT")==0) {
    decStmt -> leftChild -> tag = 2;  // setting tag to jagg2
    processJagg2DDecStmt(decStmt-> leftChild->leftChild);
    decStmt-> leftChild->t = decStmt-> leftChild->leftChild->t;
  } else if(strcmp(decStmt -> leftChild -> leftChild -> sym, "JAGGARR3D_DECLARATION_STMT")==0) {
    decStmt -> leftChild -> tag = 3;  // setting tag to jagg3
    processJagg3DDecStmt(decStmt -> leftChild->leftChild);
    decStmt-> leftChild->t= decStmt-> leftChild->leftChild->t;

  }
  // copy to decStmt
  decStmt -> tag = decStmt -> leftChild -> tag;
  decStmt -> t = decStmt -> leftChild -> t;
  // propagate to descendants
  propagateTypeExp(decStmt -> leftChild);

}

void processPrimDecStmt(TreeNode* primDecStmt) {
  TreeNode* primTypeNode = primDecStmt -> rightChild -> leftSib -> leftChild;
  if(strcmp(primTypeNode -> lexeme, "integer") == 0)
    primDecStmt -> t.p.primitiveType = 0;
  if(strcmp(primTypeNode -> lexeme, "real") == 0)
    primDecStmt -> t.p.primitiveType = 1;
  if(strcmp(primTypeNode-> lexeme, "boolean") == 0)
    primDecStmt -> t.p.primitiveType = 2;
}

void processRectDecStmt(TreeNode* rectDecStmt){
  TreeNode* temp = rectDecStmt -> leftChild;
  while(strcmp(temp -> sym, "RANGE_LIST") != 0)
    temp = temp -> rightSib;
  TreeNode* rangeList = temp;

//code to calculate number of dimensions = n
  int dimensions = 0;
  while(temp -> leftChild != temp -> rightChild){
      dimensions++;
      temp = temp -> rightChild;
  }
  dimensions++;
  rectDecStmt -> t.r.dimensions = dimensions;

  rectDecStmt -> t.r.statDyn = 1;

  // allocating memory to n ranges
  rectDecStmt -> t.r.range = (int**) malloc(sizeof(int*) * dimensions);
  for(int i = 0 ; i < dimensions ; i++){
    rectDecStmt -> t.r.range[i] = (int*) malloc(sizeof(int) * 2);
  }
  rectDecStmt -> t.r.dynRange = (char***) malloc(sizeof(char**) * dimensions);
  for(int i = 0 ; i < dimensions ; i++){
    rectDecStmt -> t.r.dynRange[i] = (char**) malloc(sizeof(char*) * 2);
    rectDecStmt -> t.r.dynRange[i][0] = NULL;
    rectDecStmt -> t.r.dynRange[i][1] = NULL;
  }
  // assigning values to ranges
  int x = 0;
  while(rangeList-> leftChild != rangeList -> rightChild){
      // only constant lower and upper bounds supported right now
      rectDecStmt -> t.r.range[x][0] = atoi(rangeList -> leftChild -> leftChild -> rightSib -> leftChild -> lexeme);  //range list - ->  range - ->  second child gives range_val -> id or constant
      if(rectDecStmt -> t.r.range[x][0] == 0) {
        bool isLiteral = true;
        for(int i = 0; i < strlen(rangeList -> leftChild -> leftChild -> rightSib -> leftChild -> lexeme); i++)
          if(!isdigit(rangeList -> leftChild -> leftChild -> rightSib -> leftChild -> lexeme[i]))
            isLiteral = false;
        if(!isLiteral) {
          rectDecStmt -> t.r.dynRange[x][0] = (char*) malloc(sizeof(char) * (strlen(rangeList -> leftChild -> leftChild -> rightSib -> leftChild -> lexeme) + 1));
          strcpy(rectDecStmt -> t.r.dynRange[x][0], rangeList -> leftChild -> leftChild -> rightSib -> leftChild -> lexeme);
          rectDecStmt -> t.r.statDyn = 2;
        }

      }
      rectDecStmt -> t.r.range[x][1] = atoi(rangeList -> leftChild -> leftChild -> rightSib -> rightSib -> rightSib -> leftChild -> lexeme);  //range list - ->  range - ->  second child gives second subscript of range
      if(rectDecStmt -> t.r.range[x][1] == 0) {
        bool isLiteral = true;
        for(int i = 0; i < strlen(rangeList -> leftChild -> leftChild -> rightSib -> rightSib -> rightSib -> leftChild -> lexeme); i++)
          if(!isdigit(rangeList -> leftChild -> leftChild -> rightSib -> rightSib -> rightSib -> leftChild -> lexeme[i]))
            isLiteral = false;
        if(!isLiteral) {
          rectDecStmt -> t.r.dynRange[x][1] = (char*) malloc(sizeof(char) * (strlen(rangeList -> leftChild -> leftChild -> rightSib -> rightSib -> rightSib -> leftChild -> lexeme) + 1));
          strcpy(rectDecStmt -> t.r.dynRange[x][1], rangeList -> leftChild -> leftChild -> rightSib -> rightSib -> rightSib -> leftChild -> lexeme);
          rectDecStmt -> t.r.statDyn = 2;
        }
      }
      rangeList = rangeList -> rightChild;
      x++;
  }
  rectDecStmt -> t.r.range[x][0] = atoi(rangeList -> leftChild -> leftChild -> rightSib -> leftChild -> lexeme);  //range list - ->  range - ->  second child gives
  if(rectDecStmt -> t.r.range[x][0] == 0) {
    bool isLiteral = true;
    for(int i = 0; i < strlen(rangeList -> leftChild -> leftChild -> rightSib -> leftChild -> lexeme); i++)
      if(!isdigit(rangeList -> leftChild -> leftChild -> rightSib -> leftChild -> lexeme[i]))
        isLiteral = false;
    if(!isLiteral) {
      rectDecStmt -> t.r.dynRange[x][0] = (char*) malloc(sizeof(char) * (strlen(rangeList -> leftChild -> leftChild -> rightSib -> leftChild -> lexeme) + 1));
      strcpy(rectDecStmt -> t.r.dynRange[x][0], rangeList -> leftChild -> leftChild -> rightSib -> leftChild -> lexeme);
      rectDecStmt -> t.r.statDyn = 2;
    }
  }
  rectDecStmt -> t.r.range[x][1] = atoi(rangeList -> leftChild -> leftChild -> rightSib -> rightSib -> rightSib -> leftChild -> lexeme);  //range list - ->  range - ->  second child gives second subscript of range
  if(rectDecStmt -> t.r.range[x][1] == 0) {
    bool isLiteral = true;
    for(int i = 0; i < strlen(rangeList -> leftChild -> leftChild -> rightSib -> rightSib -> rightSib -> leftChild -> lexeme); i++)
      if(!isdigit(rangeList -> leftChild -> leftChild -> rightSib -> rightSib -> rightSib -> leftChild -> lexeme[i]))
        isLiteral = false;
    if(!isLiteral) {
      rectDecStmt -> t.r.dynRange[x][1] = (char*) malloc(sizeof(char) * (strlen(rangeList -> leftChild -> leftChild -> rightSib -> rightSib -> rightSib -> leftChild -> lexeme) + 1));
      strcpy(rectDecStmt -> t.r.dynRange[x][1], rangeList -> leftChild -> leftChild -> rightSib -> rightSib -> rightSib -> leftChild -> lexeme);
      rectDecStmt -> t.r.statDyn = 2;
    }
  }
}

void processJagg2DDecStmt(TreeNode * jaggDecStmt){
  TreeNode * range = jaggDecStmt->leftChild;
  while(strcmp(range->sym, "RANGE") != 0)
    range = range->rightSib;

  jaggDecStmt->t.j2.range0[0] = atoi(range -> leftChild -> rightSib -> leftChild -> lexeme);
  jaggDecStmt->t.j2.range0[1] = atoi(range -> leftChild -> rightSib -> rightSib -> rightSib -> leftChild -> lexeme);

  int numRows = jaggDecStmt->t.j2.range0[1] - jaggDecStmt->t.j2.range0[0] + 1;
  jaggDecStmt->t.j2.range1 = (int*) malloc(sizeof(int)* numRows);

  bool flag=true;
  TreeNode* init=jaggDecStmt->rightChild; //init = JAGGARR2D_INIT_LIST
  int x = 0;
  do
  {
    if(x)
      init = init->rightChild;
    TreeNode* temp=init->leftChild->leftChild;//temp = "R1"
    TreeNode* valList = temp->parent->rightChild->leftSib; //valList = JAGGARR2D_VAL_LIST
    while(strcmp(temp->sym, "TK_SIZE") !=  0) {
      temp = temp->rightSib;
    }
    jaggDecStmt->t.j2.range1[x] = atoi(temp->rightSib->lexeme);
    int num = atoi(temp->rightSib->lexeme);
    while(valList->leftChild != valList->rightChild){
      num--;
      if(num<0){
      flag=false;
      }
      valList = valList->rightChild;
    }
      num--;
      if(num!=0)
      flag=false;
    x++;
     if(flag == false)
      printError(init->leftChild,false,NULL,NULL,NULL,init->leftChild -> depth,"type definition error");
    flag=true;
  }while(init->leftChild != init->rightChild);
}

void processJagg3DDecStmt(TreeNode * jaggDecStmt){
  TreeNode * range = jaggDecStmt->leftChild;
  while(strcmp(range->sym, "RANGE") != 0)
   range = range->rightSib;

  jaggDecStmt->t.j3.range0[0] = atoi(range -> leftChild -> rightSib -> leftChild -> lexeme);
  jaggDecStmt->t.j3.range0[1] = atoi(range -> leftChild -> rightSib -> rightSib -> rightSib -> leftChild -> lexeme);

  int numRows = jaggDecStmt->t.j3.range0[1] - jaggDecStmt->t.j3.range0[0] + 1;
  jaggDecStmt->t.j3.range1 = (int**) malloc(sizeof(int*) * numRows);

  TreeNode* init=jaggDecStmt->rightChild; //init = JAGGARR3D_INIT_LIST
  int x = 0;
  bool pass1 = true;
  bool flag = true;

  do
  {
    if(init->leftChild == init->rightChild)
      pass1 = false;

    TreeNode* temp=init->leftChild->leftChild;//temp = "R1"
    while(strcmp(temp->sym, "TK_SIZE") != 0) {
      temp = temp->rightSib;
    }
    jaggDecStmt->t.j3.range1[x] = (int*) malloc(sizeof(int)*(atoi(temp->rightSib->lexeme)+1));
    jaggDecStmt->t.j3.range1[x][0] = atoi(temp->rightSib->lexeme); //size of row list stored at first place

    int num_rowLists=jaggDecStmt->t.j3.range1[x][0];

    TreeNode* temp2 = temp->parent->rightChild->leftSib;  // temp2 = JAGGARR3D_ROW_LIST
    int a=1;
    bool pass2 = true;
    do
    {
      int y = 0;
      if(temp2->leftChild == temp2->rightChild)
        pass2 = false;
      TreeNode* temp3=temp2->leftChild;//temp3 = JAGGARR3D_VAL_LIST
      bool pass3 = true;

      do{
        if(temp3->leftChild == temp3->rightChild)
          pass3 = false;
        y++;
        temp3=temp3->rightChild;
      }
      while(pass3);

      jaggDecStmt->t.j3.range1[x][a] = y;
      a++;
      temp2 = temp2->rightChild;

      num_rowLists--;
      if(num_rowLists<0)
      {
        flag = false;
      }

    }while(pass2);

    if(flag == false)
      printError(init->leftChild,false,NULL,NULL,NULL,init->leftChild -> depth,"type definition error");
    if(num_rowLists>0)
    {
      printError(init->leftChild,false,NULL,NULL,NULL,init->leftChild -> depth,"type definition error");
    }
    flag=true;
    x++;
    init = init->rightChild;
  }while(pass1);
}


void propagateTypeExp(TreeNode* node) {
  node -> t = node -> parent -> t;
  node -> tag = node -> parent -> tag;
  TreeNode* child = node -> leftChild;
  while(child) {
    propagateTypeExp(child);
    child = child -> rightSib;
  }
}

void populateTable(TreeNode* root , TypeExpTable* head){
  TypeExpTable* popu = head;
  TypeExpTable* prepopu = popu;
  TreeNode* temp = root->rightChild->leftSib->leftChild; //DECLARATION_lIST
  int flag = 0;
  do{
      if(flag)
        temp = temp->rightChild;
    flag = 1;
    TreeNode* iter = temp->leftChild->leftChild; //TYPE_DECLARATION_STMT
    if(strcmp(iter->sym, "JAGGARR_DECLARATION_STMT")==0)
      iter = iter->leftChild;

    if(strcmp(iter->leftChild->rightSib->sym, "TK_LIST")==0) {
      TreeNode* it = iter->leftChild->rightSib->rightSib->rightSib->rightSib; //id_list
      while(it->leftChild != it->rightChild){
        strcpy(popu->identifier,it->leftChild->lexeme);
        popu->t = it->t;
        popu->tag = it->tag;
        popu->next = (TypeExpTable*)malloc(sizeof(TypeExpTable));
        prepopu = popu;
        popu = popu->next;
        it = it->rightChild;
      }
        strcpy(popu->identifier,it->leftChild->lexeme);
        popu->t = it->t;
        popu->tag = it->tag;
        popu->next = (TypeExpTable*)malloc(sizeof(TypeExpTable));
        prepopu = popu;
        popu = popu->next;
    } else if(strcmp(iter->leftChild->rightSib->sym, "ID")==0){
      strcpy(popu->identifier,iter->leftChild->rightSib->lexeme);
      popu->t = iter->t;
      popu->tag = iter->tag;
      popu->next = (TypeExpTable*)malloc(sizeof(TypeExpTable));
      prepopu = popu;
      popu = popu->next;
    }
  } while(temp->leftChild != temp->rightChild);
  prepopu->next = NULL; //check
}

void printTypeExpressionTable(TypeExpTable* table) {
  printf("%-22s%-6s%-16s%-30s\n", "VARIABLE NAME", "TYPE", "DYNAMIC", "TYPE EXPRESSION");
  for(int i = 0; i < 74; i++)
    printf("-");
  printf("\n");
  while(table) {
    printf("%-20s  ", table -> identifier);
    switch(table -> tag) {
      case 0:
      case 1:
        printf("%-6d", table -> tag);
        break;
      case 2:
      case 3:
        printf("%-6d", 2);
        break;
    }
    //static dynamic
    if(table -> tag != 1){
      printf("%-16s", "NOT_APPLICABLE");
    } else {
      if(table -> t.r.statDyn == 1)
        printf("%-16s", "STATIC");
      else
        printf("%-16s", "DYNAMIC");
    }

    char *ans=printTypeExp(table -> t, table -> tag);
    printf("%s\n",ans);
    free(ans);

    table = table -> next;
  }
}

char* printTypeExp(TypeExp t, typeExpTag tag) {
  char *ans=(char *)malloc(sizeof(char)*300);
  if(tag==0) {
    char* ptstr= NULL;
    switch(t.p.primitiveType) {
      case 0:
        strcat(ans,"<type=integer>     ");
        break;
      case 1:
        strcat(ans,"<type=real>        ");
        ptstr = "real";
        break;
      case 2:
        strcat(ans,"<type=boolean>     ");
        ptstr = "boolean";
        break;
    }
    //printf("<type=%-12s>", ptstr);
  } else if(tag==1) {
     strcat(ans,"<type=rectangularArray, dimensions=");
     char trial[5];
     sprintf(trial,"%d",t.r.dimensions);
     strcat(ans,trial);
     strcat(ans,", ");
    //printf("<type=rectangularArray, dimensions=%d, ", t.r.dimensions);
    for(int i = 1; i <= t.r.dimensions; i++) {
      strcat(ans,"range_R");
      sprintf(trial,"%d",i);
      strcat(ans,trial);
      strcat(ans,"=(");
      if(t.r.dynRange[i-1][0] == NULL) {
        sprintf(trial,"%d",t.r.range[i-1][0]);
        strcat(ans,trial);
      } else {
        strcat(ans, t.r.dynRange[i-1][0]);
      }
      strcat(ans,",");
      if(t.r.dynRange[i-1][1] == NULL) {
        sprintf(trial,"%d",t.r.range[i-1][1]);
        strcat(ans,trial);
      } else {
        strcat(ans, t.r.dynRange[i-1][1]);
      }
      strcat(ans,"), ");
      //printf("range_R%d=(%d, %d), ", i, t.r.range[i-1][0], t.r.range[i-1][1]);
    }
    strcat(ans,"basicElementType=integer>");
  } else if(tag==2){
    char trial[5];
    strcat(ans,"<type=jaggedArray, dimensions=2, range_R1=(");
    sprintf(trial,"%d",t.j2.range0[0]);
    strcat(ans,trial);
    strcat(ans,", ");
    sprintf(trial,"%d",t.j2.range0[1]);
    strcat(ans,trial);
    strcat(ans,"),range_R2 = (");
    for(int i=0;i<t.j2.range0[1]-t.j2.range0[0]+1;i++){
      sprintf(trial,"%d",t.j2.range1[i]);
      strcat(ans,trial);
      if(i==t.j2.range0[1]-t.j2.range0[0])
      strcat(ans,"), basicElementType = integer>");
      else
      strcat(ans,",");
    }
  } else if(tag==3) { //tag==3!
    char trial[5];
    strcat(ans,"<type=jaggedArray, dimensions=3, range_R1=(");
    sprintf(trial,"%d",t.j3.range0[0]);
    strcat(ans,trial);
    strcat(ans,", ");
    sprintf(trial,"%d",t.j3.range0[1]);
    strcat(ans,trial);
    strcat(ans,"),range_R2 = (");
    for(int i=0;i<t.j3.range0[1]-t.j3.range0[0]+1;i++){
      sprintf(trial,"%d",t.j3.range1[i][0]);
      strcat(ans,trial);
      strcat(ans,"[");
      for(int j=0;j<t.j3.range1[i][0];j++){
        sprintf(trial,"%d",t.j3.range1[i][j+1]);
        strcat(ans,trial);
        if(j==t.j3.range1[i][0] && i==t.j3.range0[1]-t.j3.range0[0])
        strcat(ans,"]");
        else if(j==t.j3.range1[i][0]-1)
        strcat(ans,"],");
        else
        strcat(ans,",");
      }
    }
    strcat(ans,"),basicElementType = integer>");

  } else {
    strcat(ans, "***");
  }
  return ans;
}


void traverseAsgList(TreeNode* root, TypeExpTable* table) {
  TreeNode* programBody = root -> rightChild -> leftSib;
  TreeNode* asgList = programBody -> rightChild;
  while(asgList -> leftChild != asgList -> rightChild)
  {
      processAsgStmt(asgList -> leftChild, table);  // process decStmt
      asgList = asgList -> rightChild; //move to next decListNode
  }
  processAsgStmt(asgList -> leftChild, table); //process last decStmt
}

void processAsgStmt(TreeNode* asgStmt, TypeExpTable* table){
  processExpression(asgStmt -> rightChild -> leftSib, table);
  if(strcmp(asgStmt -> leftChild -> sym, "ARR_ID") == 0)  {
    processArrayVariable(asgStmt -> leftChild, table);
  } else {
    getTypeExp(asgStmt -> leftChild, table);
  }
  TypeExp lhs = asgStmt -> leftChild -> t;
  typeExpTag lhsTag = asgStmt -> leftChild -> tag;
  TypeExp rhs = asgStmt -> rightChild -> leftSib -> t;
  typeExpTag rhsTag = asgStmt -> rightChild -> leftSib -> tag;
  if(equalTypeExp(lhs, lhsTag, rhs, rhsTag)) {
    asgStmt -> t = lhs;
    asgStmt -> tag = lhsTag;
  } else {
    printError(asgStmt, true, "TK_EQUALS", asgStmt -> leftChild, asgStmt -> rightChild -> leftSib, asgStmt -> depth,
      "Identifier-Expr type mismatch");
  }
}

void processExpression(TreeNode* expr, TypeExpTable* table) {
  // printf("%s\n", expr -> sym);
  // if(expr -> isTerminal)
  //   printf("--> %s\n", expr -> lexeme);
  if(expr -> leftChild == NULL) {
    if(strcmp(expr -> sym, "CONSTANT")==0) {
      expr -> tag = 0;
      expr -> t.p.primitiveType = 0;
      return;
    }
    if(strcmp(expr -> sym, "ID")==0) {
      // printf("ID Detect\n");
      getTypeExp(expr, table);
      return;
    }
    return;
  }
  if(strcmp(expr -> sym, "ARR_ID")==0) {
    processArrayVariable(expr, table);
    return;
  }
  if(expr -> leftChild == expr -> rightChild) {
    processExpression(expr -> leftChild, table);
    expr -> t = expr -> leftChild -> t;
    expr -> tag = expr -> leftChild -> tag;
    return;
  }
  processExpression(expr -> leftChild, table);
  processExpression(expr -> rightChild, table);
  TreeNode* lhs = expr -> leftChild;
  TreeNode* rhs = expr -> rightChild;
  if(lhs -> tag == 4 || rhs -> tag == 4) {
    expr -> tag = 4;
    return;
  }
  char* operator = expr -> leftChild -> rightSib -> sym;
  if(checkOperands(lhs, operator, rhs)) {
      if(strcmp(operator, "TK_DIV")==0) {
        // always evaluates to real
        expr -> tag = 0;
        expr -> t.p.primitiveType = 1;
      } else {
        expr -> t = expr -> leftChild -> t;
        expr -> tag = expr -> leftChild -> tag;
      }
    return;
  } else {
    expr -> tag = 4;
  }
}

bool isArrayVariable(TreeNode* node) {

  if(node -> leftChild &&
     (strcmp(node -> leftChild -> sym, "ID")==0) &&
     (node -> leftChild -> rightSib) &&
     (strcmp(node -> leftChild -> rightSib -> sym, "SQ_OP")==0) &&
     (node -> leftChild -> rightSib -> rightSib) &&
     (strcmp(node -> leftChild -> rightSib -> rightSib -> sym, "INDEX_LIST")==0) &&
     (node -> leftChild -> rightSib -> rightSib -> rightSib) &&
     (strcmp(node -> leftChild -> rightSib -> rightSib -> rightSib -> sym, "SQ_CL")==0) &&
     (node -> leftChild -> rightSib -> rightSib -> rightSib -> rightSib == NULL))
    return true;
  return false;
}

void processArrayVariable(TreeNode* arrVar, TypeExpTable* table) {
  getTypeExp(arrVar -> leftChild, table);
  TypeExp arr = arrVar -> leftChild -> t;
  typeExpTag arrTag = arrVar -> leftChild -> tag;
  TreeNode* indexList = arrVar -> rightChild -> leftSib;
  bool dynamic = false;
  if(arrTag == 1) {
    int dimensions = arr.r.dimensions;
    int dims = dimensions;
    bool pass = true;
    bool proceed = true;
    while(proceed && dimensions > 0) {
      if(indexList -> leftChild == indexList -> rightChild) {
        proceed = false;
      }
      dimensions--;
      int index = atoi(indexList -> leftChild -> leftChild -> lexeme);
      bool isLiteral = true;
      if(index == 0) {
        for(int i = 0; i < strlen(indexList -> leftChild -> leftChild -> lexeme); i++)
          if(!isdigit(indexList -> leftChild -> leftChild -> lexeme[i]))
            isLiteral = false;
      }
      if(isLiteral && (!(index >= arr.r.range[dims-dimensions-1][0] && index <= arr.r.range[dims-dimensions-1][1]))) {
        pass = false;
        break;
      }
      indexList = indexList -> rightChild;
    }
    if(proceed || dimensions) {
      pass = false;
    }
    if(pass) {
      arrVar -> tag = 0;
      arrVar -> t.p.primitiveType = 0;
    } else {
      printError(arrVar, true, NULL, arrVar, NULL, arrVar -> depth, "RectArray indexing error");
      return;
    }
  } else if(arrTag == 2) {
    int index0 = atoi(indexList -> leftChild -> leftChild -> lexeme);
    int index1 = atoi(indexList -> rightChild -> leftChild -> leftChild -> lexeme);
    if(indexList -> rightChild -> leftChild != indexList -> rightChild -> rightChild) {
      printError(arrVar, true, NULL, arrVar, NULL, arrVar -> depth, "Jagg2DArr indexing error");
      return;
    }
    bool isLiteral0 = true;
    if(index0 == 0) {
      for(int i = 0; i < strlen(indexList -> leftChild -> leftChild -> lexeme); i++)
        if(!isdigit(indexList -> leftChild -> leftChild -> lexeme[i]))
          isLiteral0 = false;
    }
    bool isLiteral1 = true;
    if(index1 == 0) {
      for(int i = 0; i < strlen(indexList -> rightChild -> leftChild -> leftChild -> lexeme); i++)
        if(!isdigit(indexList -> rightChild -> leftChild -> leftChild -> lexeme[i]))
          isLiteral1 = false;
    }
    int low0 = arr.j2.range0[0];
    int high0 = arr.j2.range0[1];
    int size1 = arr.j2.range1[index0 - low0];
    bool pass = (!isLiteral0 || ((index0 >= low0) && (index0 <= high0))) &&
                (!isLiteral1 || (index1 < size1));
    if(pass) {
      arrVar -> tag = 0;
      arrVar -> t.p.primitiveType = 0;
    } else {
      printError(arrVar, true, NULL, arrVar, NULL, arrVar -> depth, "Jagg2DArr indexing error");
      return;
    }

  } else if(arrTag == 3) {
    int index0 = atoi(indexList -> leftChild -> leftChild -> lexeme);
    int index1 = atoi(indexList -> rightChild -> leftChild -> leftChild -> lexeme);
    int index2 = atoi(indexList -> rightChild -> rightChild -> leftChild -> leftChild -> lexeme);
    if(indexList -> rightChild -> rightChild -> leftChild != indexList -> rightChild -> rightChild -> rightChild) {
      printError(arrVar, true, NULL, arrVar, NULL, arrVar -> depth, "Jagg3DArr indexing error");
      return;
    }
    bool isLiteral0 = true;
    if(index0 == 0) {
      for(int i = 0; i < strlen(indexList -> leftChild -> leftChild -> lexeme); i++)
        if(!isdigit(indexList -> leftChild -> leftChild -> lexeme[i]))
          isLiteral0 = false;
    }
    bool isLiteral1 = true;
    if(index1 == 0) {
      for(int i = 0; i < strlen(indexList -> rightChild -> leftChild -> leftChild -> lexeme); i++)
        if(!isdigit(indexList -> rightChild -> leftChild -> leftChild -> lexeme[i]))
          isLiteral1 = false;
    }
    bool isLiteral2 = true;
    if(index2 == 0) {
      for(int i = 0; i < strlen(indexList -> rightChild -> rightChild -> leftChild -> leftChild -> lexeme); i++)
        if(!isdigit(indexList -> rightChild -> rightChild -> leftChild -> leftChild -> lexeme[i]))
          isLiteral2 = false;
    }
    int low0 = arr.j3.range0[0];
    int high0 = arr.j3.range0[1];
    int size1 = arr.j3.range1[index0][0];
    int size2 = arr.j3.range1[index0][index1 + 1];
    bool pass = (!isLiteral0 || ((index0 >= low0) && (index0 <= high0))) &&
                (!isLiteral1 || (index1 < size1)) &&
                (!isLiteral2 || (index2 < size2));
    if(pass) {
      arrVar -> tag = 0;
      arrVar -> t.p.primitiveType = 0;
    } else {
      printError(arrVar, true, NULL, arrVar, NULL, arrVar -> depth, "Jagg3DArr indexing error");
    }
  }
}

void getTypeExp(TreeNode* id, TypeExpTable* table) {
  while(table) {
    if(strcmp(id -> lexeme, table -> identifier) == 0)
      break;
    table = table -> next;
  }
  if(table == NULL) {
    printf("DEBUG: Type expression table entry missing for %s.\n", id -> lexeme);
    return;
  }
  id -> tag = table -> tag;
  id -> t = table -> t;
  // printf("Got tag %d for %s\n", id -> tag, id -> lexeme);
  // if(id -> tag == 0)
  //   printf("prim: %d\n", id -> t.p.primitiveType);
  return;
}

bool equalTypeExp(TypeExp a, typeExpTag atag, TypeExp b, typeExpTag btag) {
  if(atag != btag) return false;
  if(atag == 0) {
    return (a.p.primitiveType == b.p.primitiveType);
  } else if(atag == 1) {
    if(a.r.dimensions != b.r.dimensions)
      return false;
    for(int i = 0; i < a.r.dimensions; i++)
      for(int j = 0; j <= 1; j++)
        if(a.r.range[i][j] != b.r.range[i][j])
          return false;
    return true;
  } else if(atag == 2) {
    for(int j = 0; j <= 1; j++)
      if(a.j2.range0[j] != b.j2.range0[j])
        return false;
    for(int i = 0; i < ((a.j2.range0)[1] - (a.j2.range0)[0] + 1); i++)
      if(a.j2.range1[i] != b.j2.range1[i])
        return false;
    return true;
  } else if(atag == 3) {
    for(int j = 0; j <= 1; j++)
      if(a.j3.range0[j] != b.j3.range0[j])
        return false;
    for(int i = 0; i < ((a.j3.range0)[1] - (a.j3.range0)[0] + 1); i++) {
      if(a.j3.range1[i][0] != b.j3.range1[i][0])
        return false;
      for(int j = 1; j <= b.j3.range1[i][0]; j++)
        if(a.j3.range1[i][j] != b.j3.range1[i][j])
          return false;
    }
    return true;
  }
}

bool checkOperands(TreeNode* lhs, char* op, TreeNode* rhs) {
  TypeExp a = lhs -> t;
  typeExpTag atag = lhs -> tag;
  TypeExp b = rhs -> t;
  typeExpTag btag = rhs -> tag;
  if(atag != btag){
    printError(lhs, true, op, lhs, rhs, lhs -> depth, "Operand type mismatch");
    return false;
  }
  if(atag == 0 && a.p.primitiveType != b.p.primitiveType) {
    printError(lhs, true, op, lhs, rhs, lhs -> depth, "Operand type mismatch");
    return false;
  }
  // allow division of arrays?
  if(strcmp(op, "TK_PLUS")==0 || strcmp(op, "TK_MINUS")==0 || strcmp(op, "TK_STAR")==0) {
    if(atag == 0) {
      if(a.p.primitiveType == 0 || a.p.primitiveType == 1)
        return true;
      else {
        printError(lhs, true, op, lhs, rhs, lhs -> depth, "Arithmetic ops on booleans NA");
        return false;
      }
    } else {
      if(equalTypeExp(a, atag, b, btag))
        return true;
      else {
        printError(lhs, true, op, lhs, rhs, lhs -> depth, "Array operand dim mismatch");
        return false;
      }
    }
  } else if(strcmp(op, "TK_DIV")==0) {
    if(atag == 0) {
      if(a.p.primitiveType == 0 || a.p.primitiveType == 1)
        return true;
      else {
        printError(lhs, true, op, lhs, rhs, lhs -> depth, "Arithmetic ops on booleans NA");
        return false;
      }
    } else {
      printError(lhs, true, op, lhs, rhs, lhs -> depth, "Elementwise division NA");
      return false;
    }
  } else if(strcmp(op, "TK_AND")==0 || strcmp(op, "TK_OR")==0) {
    if(atag == 0) {
      if(a.p.primitiveType == 2)
        return true;
      else {
        printError(lhs, true, op, lhs, rhs, lhs -> depth, "Logical ops on int/real NA");
        return false;
      }
    } else {
      printError(lhs, true, op, lhs, rhs, lhs -> depth, "Elementwise logical ops NA");
      return false;
    }
  } else {
    printError(lhs, true, op, NULL, NULL, lhs -> depth, "Invalid operator");
  }
}

void printError(TreeNode* origin, bool asgnStmt, char* op, TreeNode* lhs, TreeNode* rhs, int depth, char* msg) {
  int line_no = getLineNum(origin);

  char* cat_str = NULL;
  if(asgnStmt)
    cat_str = "ASSIGNMENT";
  else
    cat_str = "DECLARATION";

  char* op_str = NULL;
  if(op == NULL)
    op_str = "***";
  else if(strcmp(op, "TK_PLUS")==0)
    op_str = "+";
  else if(strcmp(op, "TK_MINUS")==0)
    op_str = "-";
  else if(strcmp(op, "TK_STAR")==0)
    op_str = "*";
  else if(strcmp(op, "TK_DIV")==0)
    op_str = "/";
  else if(strcmp(op, "TK_AND")==0)
    op_str = "&&&";
  else if(strcmp(op, "TK_OR")==0)
    op_str = "|||";
  else if(strcmp(op, "TK_EQUALS")==0)
    op_str = "=";

  char* lhs_lexeme = NULL;
  char* lhs_type = NULL;

  if(lhs) {
    lhs_lexeme = getLexeme(lhs);
    lhs_type = printTypeExp(lhs -> t, lhs -> tag);
  } else {
    lhs_lexeme = "***";
    lhs_type = "***";
  }
  char* rhs_lexeme = NULL;
  char* rhs_type = NULL;
  if(rhs) {
    if(strcmp(op, "TK_EQUALS")!=0)
      rhs_lexeme = getLexeme(rhs);
    else
      rhs_lexeme = "***";
    rhs_type = printTypeExp(rhs -> t, rhs -> tag);
  } else {
    rhs_lexeme = "***";
    rhs_type = "***";
  }

  printf("%-10d%-15s%-10s%-20s%-130s%-20s%-130s%-8d%s\n", line_no, cat_str, op_str,
                                                        lhs_lexeme, lhs_type,
                                                        rhs_lexeme, rhs_type,
                                                        depth, msg);
}

int getLineNum(TreeNode* node) {
  // first leaf
  while(node -> leftChild)
    node = node -> leftChild;
  return node -> line_no;
}

char* getLexeme(TreeNode* node) {
  while(node -> leftChild)
    node = node -> leftChild;
  return node -> lexeme;
}
