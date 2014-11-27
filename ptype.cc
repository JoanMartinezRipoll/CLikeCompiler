#include <iostream>
#include <string>
#include <map>
#include <list>

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include "ptype.hh"

ptype create_type(string kind,ptype down,ptype right)
{
  ptype t=new(stype);

  t->kind=kind;
  t->down=down;
  t->right=right;
  return t;
}
void write_type(ptype t)
{
  if (t==0) return;

  cout<<t->kind;
 
  if (t->down!=0) {
    cout<<"(";
    write_type(t->down);
    cout<<")";
  }
  if (t->right!=0) {
    cout<<",";
    write_type(t->right);
  }
}

ptype typecopy(ptype t)
{
  if (t==0) return 0;

  // Instead of the following line
  // return type_create(t->kind,typecopy(t->down),typecopy(t->right),t->passmode);
  // we do a more generic implementation able to additions of fields in ptype.
  ptype newt=new stype;
  *newt=*t;
  newt->down=typecopy(t->down);
  newt->right=typecopy(t->right);
  return newt;
}

bool equivalent_types(ptype t1,ptype t2)
{
  if (t1==0 && t2==0) return true;
  if (t1==0) return false;
  if (t2==0) return false;

  bool equiv=t1->kind==t2->kind && 
    equivalent_types(t1->down,t2->down) &&
    equivalent_types(t1->right,t2->right);
  return equiv;
}
