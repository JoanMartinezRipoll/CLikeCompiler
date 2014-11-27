#ifndef INTERPRETE_H
#define INTERPRETE_H


/// main functions
void evaluateTipus(AST *a);
string evaluate(AST *a);
void execute(AST *a);

/// other functions
bool isbasickind(string kind);

void check_params(AST *a,ptype tp,int line,int numparam);
void insert_vars(AST *a);
void construct_struct(AST *a);
void insert_headers(AST *a);

#endif