#ifndef PTYPE_H
#define PTYPE_H

// This struct is used for representing types.
// It allows to represent essentially a tree.
// The way some special types (like procedures and functions)
// must be represented is not clear and the programmer
// can decide how he/she prefers to do that.
// But we recommend to use the names "int", "bool",
// "array", "struct", "function" and "procedure" for kind.
// In the last two cases we recommend to use down
// to keep the list of parameters as brothers, with
// "parref" and "parval" for kind. In the case of
// "function" we recommend to use right for the
// return value type.
typedef struct ttypenode {
  string kind;
  struct ttypenode *down;
  struct ttypenode *right;
} *ptype,stype;

ptype create_type(string kind,ptype down,ptype right);

void write_type(ptype t);

ptype typecopy(ptype t);

bool equivalent_types(ptype t1,ptype t2);

#endif

