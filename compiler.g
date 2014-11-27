#header
<<
#include <string>
#include <iostream>
#include <map>
#include <list>

using namespace std;

#include "ptype.hh"
#include "symtab.hh"


// struct to store information about tokens
typedef struct {
  string kind;
  string text;
  int line;
} Attrib;

// function to fill token information (predeclaration)
void zzcr_attr(Attrib *attr, int type, char *text);

// fields for AST nodes
#include "myASTnode.hh"

// macro to create a new AST node (and function predeclaration)
#define zzcr_ast(as,attr,ttype,textt) as=createASTnode(attr,ttype,textt)
AST* createASTnode(Attrib* attr, int ttype, char *textt);
>>

<<
#include <cstdlib>
#include <cmath>
#include "interpreter.hh"

// function to fill token information
void zzcr_attr(Attrib *attr, int type, char *text) {
  if (type == NUM) {
    attr->kind = "intconst";
    attr->text = text;
  }
   else if (type== ID ) {
    attr->kind = "id";
    attr->text=text;
  }
  
  else if (type== STRING ){
    attr->kind="string";
    attr->text=text;
   }

  else {
    attr->kind = text;
    attr->text = "";
  }
  attr->line = zzline;
}

// function to create a new AST node
AST* createASTnode(Attrib* attr, int type, char* text) {
  AST* as = new AST;
  as->kind = attr->kind; 
  as->text = attr->text;
  as->line = attr->line;
  as->right = NULL; 
  as->down = NULL;
  return as;
}

/// create a new "list" AST node with one element
AST* createASTlist(AST *child) {
 AST *as=new AST;
 as->kind="list";
 as->right=NULL;
 as->down=child;
 return as;
}


/// create a new empty "list" AST node 
AST* createASTlist() {
 AST *as=new AST;
 as->kind="list";
 as->right=NULL;
 as->down=NULL;
 return as;
} 

/// get nth child of a tree. Count starts at 0.
/// if no such child, returns NULL
AST* child(AST *a,int n) {
 AST *c=a->down;
 for (int i=0; c!=NULL && i<n; i++) c=c->right;
 return c;
} 

/// print AST, recursively, with indentation
void ASTPrintIndent(AST *a,string s)
{
  if (a==NULL) return;

  cout<<a->kind;
  if (a->text!="") cout<<"("<<a->text<<")";
  cout<<endl;

  AST *i = a->down;
  while (i!=NULL && i->right!=NULL) {
    cout<<s+"  \\__";
    ASTPrintIndent(i,s+"  |"+string(i->kind.size()+i->text.size(),' '));
    i=i->right;
  }
  
  if (i!=NULL) {
      cout<<s+"  \\__";
      ASTPrintIndent(i,s+"   "+string(i->kind.size()+i->text.size(),' '));
      i=i->right;
  }
}

/// print AST 
void ASTPrint(AST *a)
{
  while (a!=NULL) {
    cout<<" ";
    ASTPrintIndent(a,"");
    a=a->right;
  }
}




int main() {
AST *root = NULL;
ANTLR(program(&root), stdin);
ASTPrint(root);
execute(root);
}
>>

#lexclass START
#token NUM "[0-9]+"
#token PLUS "\+"
#token MINUS "\-"
#token TIMES "\*"
#token DIV "\/"
#token PUNTCOMA "\;"
#token MAIN "main"
#token INT "INT"
#token BOOL "BOOL"
#token STRINGDECLARACIO "STRING"
#token BTRUE "TRUE"
#token BFALSE "FALSE"
#token STRING "\" ~[\"]* \" "
#token OPENBRA "\{"
#token CLOSEBRA "\}"
#token WRITE "write"
#token CLOSEPAR "\)"
#token COMMA ","
#token OPENPAR "\("
#token WHILE "while"
#token DO "do"
#token IF "if"
#token ELSE "else"
#token THEN "then"
#token GT ">"
#token EQ "=="
#token NEQ "!="
#token LT "<"
#token ASIG "="
#token RETURN "RETURN"
#token FUNCTION "function"
#token ID "[a-zA-Z][a-zA-Z0-9]*"
#token NEWLINE "\n" << zzline++; zzskip(); >>
#token SPACE "[\ \t]" << zzskip();>>


program: MAIN^ OPENBRA! l_instrs CLOSEBRA! l_dec_func;



l_dec_func: (dec_func) * <<#0=createASTlist(_sibling);>>;
dec_func: FUNCTION^ l_func_vars OPENBRA! l_instrs_func CLOSEBRA!;

l_func_vars:ID^ OPENPAR! vars CLOSEPAR! RETURN! (INT|BOOL|STRINGDECLARACIO);
vars: { dec_var_func (COMMA! dec_var_func )* } <<#0=createASTlist(_sibling);>>; 

dec_var_func:(INT^|BOOL^|STRINGDECLARACIO^) ID;

l_instrs_func: (instruction_func)* <<#0=createASTlist(_sibling);>>;
instruction_func:
  (INT^|BOOL^|STRINGDECLARACIO^) expres_assig PUNTCOMA!
  |ID ASIG^ expr_add PUNTCOMA!
  | WRITE^ expr_add PUNTCOMA!
  | IF^ OPENPAR! exp_compare CLOSEPAR! OPENBRA! l_instrs_func CLOSEBRA! {ELSE! OPENBRA! l_instrs_func CLOSEBRA!}
  | WHILE^ OPENPAR! exp_compare CLOSEPAR! OPENBRA! l_instrs_func CLOSEBRA!
  | RETURN^ expr_add PUNTCOMA!;

l_instrs: (instruction)* <<#0=createASTlist(_sibling);>>;
instruction: 
  (INT^|BOOL^|STRINGDECLARACIO^) expres_assig PUNTCOMA!
  |ID ASIG^ expr_add PUNTCOMA!
  | WRITE^ expr_add PUNTCOMA!
  | IF^ OPENPAR! exp_compare CLOSEPAR! OPENBRA! l_instrs CLOSEBRA! {ELSE! OPENBRA! l_instrs CLOSEBRA!}
  | WHILE^ OPENPAR! exp_compare CLOSEPAR! OPENBRA! l_instrs CLOSEBRA! ;
  
  expres_assig: ID {ASIG^ expr_add};
  
l_par: {exp_compare (COMMA! exp_compare)*}<<#0=createASTlist(_sibling);>>;
exp_compare: expr_add ((EQ^|GT^|LT^|NEQ^) expr_add)*;
expr_add : expr_fact ((PLUS^|MINUS^) expr_fact)* ;
expr_fact: expr_unaria ((TIMES^|DIV^) expr_unaria)*;
expr_unaria: MINUS^ expr_unaria| expr_simple;
expr_simple: NUM|OPENPAR! exp_compare CLOSEPAR!|
      ID^ {OPENPAR^ l_par CLOSEPAR!}|BTRUE|BFALSE|STRING;
