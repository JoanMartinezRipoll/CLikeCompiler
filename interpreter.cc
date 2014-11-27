#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <list>


using namespace std;

#include "ptype.hh"
#include "symtab.hh"

#include "myASTnode.hh"
#include <cstdlib>
#include <cmath>
#include "interpreter.hh"


map<string,AST *> taulaFuncions;
symtab symboltable;
string returnValue;
string returnType="";

void errordeclaredident(int l, string s) {
  cout<<"L. "<<l<<": Parameter "<<s<<" previously declared"<<endl;
  abort();
}

void errordeclaredfunction(int l, string s) {
  cout<<"L. "<<l<<": Method "<<s<<" previously declared"<<endl;
  abort();
}

void errornumparam(int l) {
  cout<<"L. "<<l<<": Invalid number of arguments for method call"<<endl;
  abort();
}

void errorNoReturn(int l, string s) {
  cout<<"L. "<<l<<": Method "<<s<<" returns void."<<endl;
  abort();
}

void errornotintwoviewscope (int l, string s) {
  cout<<"L. "<<l<<": Identifier "<<s<<" not visible in scope."<<endl;
  abort();
}

void errorCapValorAssignat (int l, string s) {
    cout<<"L. "<<l<<": Identifier "<<s<<" withoud any value assigned."<<endl;
  abort();
}

void erroroperacionopermesa(int l,string op, string s){
  cout<<"L. "<<l<<": Operation "<< op <<" for type "<<s<<" not allowed"<<endl;
  abort();
}


void errornondeclaredfunction(int l, string s) {
  cout<<"L. "<<l<<": Method "<<s<<" not declared."<<endl;
  abort();
}

void errorincompatibleparam(int l,int n) {
  cout<<"L. "<<l<<": Parameter "<<n<<" of method call of incompatible type."<<endl;
  abort();
}

void errornondeclaredident(int l, string s) {
  cout<<"L. "<<l<<": Identifier "<<s<<" not declared."<<endl;
  abort();
}

void errorincompatibleoperands(int l, string s1, string s2) {
  cout<<"L. "<<l<<": Operation not allowed , operands "<<s1<<" and "<<s2<< " of different type"<<endl;
  abort();
}

void errorincompatibleassig(int l, string s1, string s2) {
  cout<<"L. "<<l<<": Assignation not allowed, operands "<<s1<<" and "<<s2<< " of different type"<<endl;
  abort();
}

void errorincompatiblefunctionreturn(int l, string s1, string s2){
   cout<<"L. "<<l<<": Operand "<<s1<<" and method return value "<<s2<< " de tipus diferents"<<endl;
  abort();
}
void errorincompatiblereturn (int l, string s1, string s2){
   cout<<"L. "<<l<<": Method return type "<<s1<<" expected, but "<<s2<< " received"<<endl;
  abort();
}


string toString(int value){
  stringstream strm;
  strm << value ;
  return strm.str(); 
}


static void InsertintoST(int line,string kind,string id,ptype tp)
{
  infosym p;
  
  if (symboltable.find(id) && symboltable.jumped_scopes(id)==0) { 
	if (kind=="function") errordeclaredfunction(line,id);
	else errordeclaredident(line,id);
  }
  else {   
    symboltable.createsymbol(id);   
    symboltable[id].kind=kind;
    symboltable[id].tp=tp;
  }
}

int getNombreParams(ptype tp) {
  int params = 0;
  while (tp != 0) {
    params++;
    tp = tp->right;
  }
  return params;	
}

bool isbasickind(string kind) {
  return kind=="int" || kind=="bool" || kind=="string";
}
void check_params(AST *a,ptype tp,int line,int numparam)
{
  //check_params (treeOfCall, firstArgumentOfDeclaration, line, argumentsOfDeclaration)
  /*arbre de la crida\__ (
   *	  \__id(fact)
   *	  \__list
   *		\__+
   *		    \__id(a)
   *		    \__intconst(1)
   * 
   */
  // ptype is the first argument
  
  int nombreParametres = 0;
  AST *iParam = child(child(a,1),0);
  while (iParam != 0) {
    ++nombreParametres;
    iParam = iParam->right;
  }
  // Check that the number of arguments of the method are equal to the number of arguments used to call it.
  if (nombreParametres != numparam) {
    errornumparam(a->line);
  }
  iParam = child(child(a,1),0);
  ptype p=tp;
  int i=0;
  //If the number of arguments is equal, check that types are equal too.
  while(i<nombreParametres)
  {
    evaluateTipus(iParam);
    if (p->kind!=iParam->tp->kind) errorincompatibleparam(a->line, i+1);
    p=p->right;
    iParam = iParam->right;
    i++;
  }
}


void create_params(AST *a)
{
  // Save the type and put into right the next argument
  if (a->right) {
    create_params(a->right);
    a->tp = create_type(a->kind, 0, a->right->tp);
  }
  else {
    
    a->tp = create_type(a->kind,0, 0);
  }	
}

void create_header(AST *a)
{
  //      \__function
  //          |          \__id(q)
  //          |          |     \__list
  //          |          |     \__int
  //          |          \__list
  //          |                 \__return
  //          |                          \__intconst(34)
  //          \__function
  //                     \__id(fact)
  //                     |        \__list
  //                     |        |      \__int
  //                     |        |      |     \__id(n)
  //                     |        |      \__string
  //                     |        |               \__id(m)
  //                     |        \__int
  
  
  AST *primerParam = child(child(child(a,0),0),0);
  //Save the list of arguments at down and the return at right
  
  if (a->kind == "function") {
    child(child(a,0),1)->tp = create_type(child(child(a,0),1)->kind,0, 0);
    //has arguments
    if (primerParam) {
      create_params(primerParam);
      // It is known that the return type will always be of type int
      
      a->tp = create_type("function", primerParam->tp,child(child(a,0),1)->tp);
    }
    else {
      a->tp = create_type("function", 0, child(child(a,0),1)->tp);
      
    }
  }
  
}


void insert_header(AST *a)
{
  create_header(a);
  
  //InsertintoST(int line,string kind,string id,ptype tp,valor)
  InsertintoST(a->line, "function", child(a,0)->text, a->tp);
}

void insert_headers(AST *a)
{
  
  while (a!=0) {
    insert_header(a);
    a=a->right;
  }
  
}

string execute_function(string a, string valors[]){
  /*
   * function
   * \__id(f)
   * |     \__list
   * |     |      \__int
   * |     |      |     \__id(a)
   * |     |      \__int
   * |     |            \__id(b)
   * |     \__int
   * \__list
   * \__return
   *	  \__+
   *	      \__id(a)
   *	      \__id(b)
   */
  
  AST *funcioAExecutar=taulaFuncions[a];  
  int iParam=0;
  string returnFuncioType= symboltable[child(funcioAExecutar,0)->text].tp->right->kind;
  
 //insertar les variables a la st
  for (AST *a1=child(child(child(funcioAExecutar,0),0),0) ;a1!=0;a1=a1->right) {
      InsertintoST(child(a1,0)->line,"idvarlocal",child(a1,0)->text,create_type(a1->kind,0,0));
      symboltable[child(a1,0)->text].value=valors[iParam];
      iParam++;
  }
  
  
  for (AST *a1=child(child(funcioAExecutar,1),0);a1!=0;a1=a1->right) {
    
    execute(a1);
    if(returnType!="") {
        if (returnType!=returnFuncioType) errorincompatiblereturn (a1->line, returnFuncioType, returnType);
	returnType="";
      return returnValue;
    }
  }
  errorNoReturn(child(child(funcioAExecutar,1),0)->line,a);
}


string preparaIExecutaFuncio(AST *a){
  /*
    * (
    * |      |       \__id(f)
    * |      |       \__list
    * |      |              \__intconst(1)
    * |      |              \__intconst(1)*/
  
      child(a,0)->tp=symboltable[child(a,0)->text].tp;
     
      // A method must check the parameters
      
      int nombreParams = getNombreParams(child(a,0)->tp->down);		//For methods, the list of params is set at down.
      check_params(a, child(a,0)->tp->down, a->line, nombreParams);
      
      
      ptype p=child(a,0)->tp->down; //To obtain the type of the call
      string valors[nombreParams];
      AST *params=child(a,1);
      int iParam =0;	  
      while (iParam<nombreParams){
	valors[iParam] = evaluate(child(params,iParam));
        iParam++;
	p=p->right;
      }
      a->sc = symboltable.push();
      symboltable.setidtable(child(a,0)->text);
      string valorRetorn= execute_function(child(a,0)->text, valors);
      symboltable.pop();
      return valorRetorn;
}



string obtainIdValue (string id) {
  string value;
  string actualIdTable = symboltable.top()->idtable;
   //Found at the current scope
    if (symboltable.jumped_scopes(id)==0) value=symboltable[id].value;
 //Search at main
  else  value = symboltable.obtainValueFromScope(id, "main")->value;
  return value;
}


void evaluateTipus(AST *a) {
  if(returnType!="") return;
  if (a == NULL) return;
  else if (a->kind == "intconst"){
    a->tp=create_type("int",0,0);
    
  }
  else if (a->kind == "string"){
    a->tp=create_type("string",0,0);
    
  }
  else if (a->kind == "id") {
    infosym *i_sym;
    ptype p;
    string idTable = symboltable.top()->idtable;
    i_sym= symboltable.obtainValueFromScope(a->text, idTable);    
    if (i_sym==NULL && idTable!="main")
      {
	i_sym= symboltable.obtainValueFromScope(a->text, "main");
      }
    if (i_sym==NULL) errornondeclaredident(a->line,a->text);
    a->tp = i_sym->tp;      
  }
  
  
  /*
   * 		(
  |      |           \__id(fact)
  |      |           \__list
  |      |                  \__+
  |      |                      \__id(a)
  |      |                      \__intconst(1)

   * */
  else if (a->kind == "(") {
    if (!symboltable.find(child(a,0)->text)) errornondeclaredfunction(a->line, child(a,0)->text);
    else {
      a->tp=symboltable[child(a,0)->text].tp->right;// This is a method and the type must be the returned type of the method.
    }
  }
  
  else {
    evaluateTipus(child(a,0));
    evaluateTipus(child(a,1));
    if(child(a,1)!=NULL) {
      if (child(a,0)->tp->kind!=child(a,1)->tp->kind) {
	  if (child(a,0)->kind=="(") errorincompatiblefunctionreturn(a->line,child(a,1)->text,child(child(a,0),0)->text);
	  else if (child(a,1)->kind=="(") errorincompatiblefunctionreturn(a->line,child(a,0)->text,child(child(a,1),0)->text);
	  else {
	    if (a->kind!="=") errorincompatibleoperands(a->line,child(a,0)->text,child(a,1)->text);
	    else errorincompatibleassig(a->line,child(a,0)->text,child(a,1)->text);
	  }
      }
    }
    a->tp=child(a,0)->tp;
  }
}


int evaluate2IntOperation(string kind,int x1, int x2){
  
  if (kind == "+") return x1 + x2;
  else if (kind == "-") return x1 - x2;
  else if (kind == "*")  return x1 * x2;
  else if (kind == "/") return x1 / x2;
  else if (kind=="=="){
    if (x1==x2) return 1;
    else  return 0;
  }
  else if (kind=="!="){
    if (x1!=x2) return 1;
    else  return 0;
  }
  else if (kind == "<"){
    if (x1< x2) return 1;
    else  return 0;
  }
  else if (kind == ">"){
    if (x1 > x2) return 1;
    else  return 0;
  }
  
  else cout << "Operacio no coneguda" << endl;    
}


string evaluate(AST *a) { 
    if(returnType!="") return "0";
    if (a == NULL) { return "0";}
    else {
	  if (a->tp->kind == "int") {
		if (a->kind == "intconst"){
		  return a->text;    
		}
		else if (a->kind == "id") {
		  string value = obtainIdValue (a->text);
		  if (value=="") errorCapValorAssignat(a->line,a->text);
		  return value;		   
		}
		
		else if (a->kind == "(") {		
		  return preparaIExecutaFuncio(a);
		}  
		
		else {
		  //Below, all the Integer operations with two arguments, above only one argument.
		  int x1 = atoi(evaluate(child(a,0)).c_str());
		  int x2 = atoi(evaluate(child(a,1)).c_str());
		  int result = evaluate2IntOperation (a->kind,x1,x2);
		  return toString(result);	  
		}
	  }
      
	  else if (a->tp->kind=="string") {
		  if (a->kind == "string") return a->text;
		  else if (a->kind == "id"){
		     string value = obtainIdValue (a->text);
		     if (value=="") errorCapValorAssignat(a->line,a->text);
		     return value;	
		  }
		  else if (a->kind=="+"){
		    string s1= evaluate(child(a,0));
		    string s2= evaluate(child(a,1));
		    a->tp=create_type("string",0,0);
		    stringstream strm;
		    strm << s1 << s2 ;
		    string s=strm.str();
		    s.erase(s1.length()-1,2);
		    return s;  
		  }
		  else if (a->kind=="=="){
		    string s1= evaluate(child(a,0));
		    string s2= evaluate(child(a,1));
		    if(s1.compare(s2)==0) return "1";
		    else return "0";
		  }
		    else if (a->kind=="!="){
		    string s1= evaluate(child(a,0));
		    string s2= evaluate(child(a,1));
		    if(s1.compare(s2)==0) return "0";
		    else return "1";
		  }
		  else if (a->kind == "(") {		
		    return preparaIExecutaFuncio(a);
		  }			    
		  else erroroperacionopermesa(a->line,a->kind,"string");
			    
	  }
    }
}



void execute(AST *a) {
  if (returnType!="") return;
  if (a == NULL) return;
  else if (a->kind =="main"){
    a->sc=symboltable.push();
    symboltable.setidtable("main");
    insert_headers(child(child(a,1),0));
    //treat the method
    execute(child(a,1));
    //treat the instructions
    execute(child(a,0));
}
  else if (a->kind=="list") {
    for (AST *a1=a->down;a1!=0;a1=a1->right) {
    execute(a1);
    }
  }
  
  else if (a->kind == "return") {
    evaluateTipus(child(a,0));    
    returnValue = evaluate(child(a,0));
    returnType=child(a,0)->tp->kind;
  }
  
  else if (a->kind == "="){
    evaluateTipus(a);
    string value = evaluate(child(a,1));
    if (symboltable.jumped_scopes(child(a,0)->text)==0) symboltable[child(a,0)->text].value= value;
    else symboltable.insertIdIntoScope(child(a,0)->text, "main",value);   
  }
  
  else if (a->kind == "write") {
      evaluateTipus(child(a,0));
      cout << evaluate(child(a,0)) << endl;
  }
    
  else if (a->kind == "if") {
      evaluateTipus(child(a,0));
      if(evaluate(child(a,0))=="1") execute(child(a,1)); //if en cas cert
  }
    
  else if (a->kind =="while"){
      evaluateTipus(child(a,0));
      while(evaluate(child(a,0))=="1") 
      {
	execute(child(a,1));
      }
  }
    
  else if (a->kind =="function"){
      
      /*
       *    function
       *                    \__id(f)
       *                    |     \__list
       *                    |            \__id(a)
       *                    |            \__id(b)
       *                    \__list
       *                           \__=
       *                           |   \__id(c)
       *                           |   \__intconst(44)
       *                           \__return
       *                                    \__+
       *                                        \__id(a)
       *                                        \__id(b)
       */
      
      
      taulaFuncions[child(a,0)->text] = a;
    }
   
   else if (isbasickind(a->kind)) {
      // 	     
      //   \__list
      //   |      \__int
      //   |      |     \__=
      //   |      |         \__id(x)
      //   |      |         \__intconst(5)
      //   |      \__int
      //   |      |     \__id(y)
      //   |      \__=
      //   |      |   \__id(y)
      //   |      |   \__intconst(2)
      
      a->tp=create_type(a->kind,0,0);
      
      
      //if(a->child(a,0)->ki
      if(child(a,0)->kind=="="){
	InsertintoST(a->line,"idvarlocal",child(child(a,0),0)->text,a->tp);
	execute(child(a,0));	       
      }
      else{
	InsertintoST(a->line,"idvarlocal",child(a,0)->text,a->tp);
      }
      
    }
    
    else {
      cout<<"No case defined for kind "<<a->kind<<endl;
    }
}
    
