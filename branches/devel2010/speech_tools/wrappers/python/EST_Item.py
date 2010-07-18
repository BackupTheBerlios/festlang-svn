# This file was created automatically by SWIG 1.3.27.
# Don't modify this file, modify the SWIG interface instead.

import _EST_Item

# This file is compatible with both classic and new-style classes.
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


class EST_Item(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, EST_Item, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, EST_Item, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ EST_Item instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, EST_Item, 'this', _EST_Item.new_EST_Item(*args))
        _swig_setattr(self, EST_Item, 'thisown', 1)
    def __del__(self, destroy=_EST_Item.delete_EST_Item):
        try:
            if self.thisown: destroy(self)
        except: pass

    def F(*args): return _EST_Item.EST_Item_F(*args)
    def S(*args): return _EST_Item.EST_Item_S(*args)
    def I(*args): return _EST_Item.EST_Item_I(*args)
    def A(*args): return _EST_Item.EST_Item_A(*args)
    def set(*args): return _EST_Item.EST_Item_set(*args)
    def set_function(*args): return _EST_Item.EST_Item_set_function(*args)
    def f_remove(*args): return _EST_Item.EST_Item_f_remove(*args)
    def evaluate_features(*args): return _EST_Item.EST_Item_evaluate_features(*args)
    def f_present(*args): return _EST_Item.EST_Item_f_present(*args)
    def length(*args): return _EST_Item.EST_Item_length(*args)
    def as_relation(*args): return _EST_Item.EST_Item_as_relation(*args)
    def in_relation(*args): return _EST_Item.EST_Item_in_relation(*args)
    def relation_name(*args): return _EST_Item.EST_Item_relation_name(*args)
    def relation(*args): return _EST_Item.EST_Item_relation(*args)
    def same_item(*args): return _EST_Item.EST_Item_same_item(*args)
    def unref_all(*args): return _EST_Item.EST_Item_unref_all(*args)
    def prepend_daughter(*args): return _EST_Item.EST_Item_prepend_daughter(*args)
    def append_daughter(*args): return _EST_Item.EST_Item_append_daughter(*args)
    def daughter1(*args): return _EST_Item.EST_Item_daughter1(*args)
    def daughtern(*args): return _EST_Item.EST_Item_daughtern(*args)
    def next_sibling(*args): return _EST_Item.EST_Item_next_sibling(*args)
    def prev_sibling(*args): return _EST_Item.EST_Item_prev_sibling(*args)
    def parent(*args): return _EST_Item.EST_Item_parent(*args)
    def features(*args): return _EST_Item.EST_Item_features(*args)
    def leafs(*args): return _EST_Item.EST_Item_leafs(*args)
    def daughters(*args): return _EST_Item.EST_Item_daughters(*args)

class EST_ItemPtr(EST_Item):
    def __init__(self, this):
        _swig_setattr(self, EST_Item, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, EST_Item, 'thisown', 0)
        self.__class__ = EST_Item
_EST_Item.EST_Item_swigregister(EST_ItemPtr)


as = _EST_Item.as

next_item = _EST_Item.next_item

first_leaf = _EST_Item.first_leaf

last_leaf = _EST_Item.last_leaf

next_leaf = _EST_Item.next_leaf

num_leaves = _EST_Item.num_leaves

remove_item = _EST_Item.remove_item

copy_node_tree = _EST_Item.copy_node_tree

copy_node_tree_contents = _EST_Item.copy_node_tree_contents

item_jump = _EST_Item.item_jump


