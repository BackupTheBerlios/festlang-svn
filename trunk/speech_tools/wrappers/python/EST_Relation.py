# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import _EST_Relation

def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name) or (name == "thisown"):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


import EST_Item
class EST_Relation(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, EST_Relation, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, EST_Relation, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ EST_Relation instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, EST_Relation, 'this', _EST_Relation.new_EST_Relation(*args))
        _swig_setattr(self, EST_Relation, 'thisown', 1)
    def __del__(self, destroy=_EST_Relation.delete_EST_Relation):
        try:
            if self.thisown: destroy(self)
        except: pass

    def load(*args): return _EST_Relation.EST_Relation_load(*args)
    def save(*args): return _EST_Relation.EST_Relation_save(*args)
    def evaluate_item_features(*args): return _EST_Relation.EST_Relation_evaluate_item_features(*args)
    def clear(*args): return _EST_Relation.EST_Relation_clear(*args)
    def utt(*args): return _EST_Relation.EST_Relation_utt(*args)
    def set_utt(*args): return _EST_Relation.EST_Relation_set_utt(*args)
    def name(*args): return _EST_Relation.EST_Relation_name(*args)
    def head(*args): return _EST_Relation.EST_Relation_head(*args)
    def root(*args): return _EST_Relation.EST_Relation_root(*args)
    def tail(*args): return _EST_Relation.EST_Relation_tail(*args)
    def first(*args): return _EST_Relation.EST_Relation_first(*args)
    def first_leaf(*args): return _EST_Relation.EST_Relation_first_leaf(*args)
    def last(*args): return _EST_Relation.EST_Relation_last(*args)
    def last_leaf(*args): return _EST_Relation.EST_Relation_last_leaf(*args)
    def append(*args): return _EST_Relation.EST_Relation_append(*args)
    def prepend(*args): return _EST_Relation.EST_Relation_prepend(*args)
    def length(*args): return _EST_Relation.EST_Relation_length(*args)
    def empty(*args): return _EST_Relation.EST_Relation_empty(*args)
    def remove_item(*args): return _EST_Relation.EST_Relation_remove_item(*args)
    def remove_item_feature(*args): return _EST_Relation.EST_Relation_remove_item_feature(*args)
    def items(*args): return _EST_Relation.EST_Relation_items(*args)

class EST_RelationPtr(EST_Relation):
    def __init__(self, this):
        _swig_setattr(self, EST_Relation, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, EST_Relation, 'thisown', 0)
        _swig_setattr(self, EST_Relation,self.__class__,EST_Relation)
_EST_Relation.EST_Relation_swigregister(EST_RelationPtr)


copy_relation = _EST_Relation.copy_relation

