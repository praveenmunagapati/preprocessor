#ifndef __TypeUtils_hpp__
#define __TypeUtils_hpp__

// Utility functions for working with types
#include "CellType.hpp"
#include "StringType.hpp"
#include "ThreadContext.hpp"
#include "ListType.hpp"

namespace FM
{
  inline Object makeCellFromList(ThreadContext *ctxt, const Object &t) {
    assert(t.typeCode() == TypeListArray);
    Object p = ctxt->_cell->makeMatrix(1,t.count());
    Object *q = ctxt->_cell->rw(p);
    const Object *h = ctxt->_list->ro(t);
    for (size_t i=0;i<t.count();i++)
      q[i] = h[i];
    return p;
  }

  inline Object makeListFromCell(ThreadContext *ctxt, const Object &t) {
    assert(t.typeCode() == TypeCellArray);
    Object p = ctxt->_list->makeMatrix(t.count(),1);
    Object *q = ctxt->_list->rw(p);
    const Object *h = ctxt->_cell->ro(t);
    for (size_t i=0;i<t.count();i++)
      q[i] = h[i];
    return p;
  }

  inline Object makeCellFromStrings(ThreadContext *ctxt, const FMStringList &t) {
    Object p = ctxt->_cell->makeMatrix(1,t.size());
    Object *q = ctxt->_cell->rw(p);
    for (size_t i=0;i<t.size();i++)
      q[i] = ctxt->_string->makeString(t[i]);
    return p;
  }

  inline FMStringList makeStringsFromCell(ThreadContext *ctxt, const Object &t) {
    assert(t.type()->code() == TypeCellArray);
    FMStringList ret;
    const Object *tptr = ctxt->_cell->ro(t);
    for (dim_t i=0;i<t.dims().count();i++)
      ret << ctxt->_string->getString(tptr[i]);
    return ret;
  }

  inline void addIndexToList(ThreadContext *ctxt, Object &list, ndx_t val) {
    ctxt->_list->push(list,ctxt->_index->makeScalar(val));
  }

  inline void addStringToList(ThreadContext *ctxt, Object &list, const FMString &string) {
    ctxt->_list->push(list,ctxt->_string->makeString(string));
  }
  
  inline ndx_t indexOfStringInList(ThreadContext *ctxt, const Object &list, const FMString &string) {
    return ctxt->_list->indexOf(list,ctxt->_string->makeString(string));
  }
}

#endif