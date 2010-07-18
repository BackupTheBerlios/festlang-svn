# This file was created automatically by SWIG 1.3.27.
# Don't modify this file, modify the SWIG interface instead.
import EST_Item
import EST_Relation

import _EST_Utterance

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


class EST_Utterance(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, EST_Utterance, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, EST_Utterance, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ EST_Utterance instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, EST_Utterance, 'this', _EST_Utterance.new_EST_Utterance(*args))
        _swig_setattr(self, EST_Utterance, 'thisown', 1)
    def __del__(self, destroy=_EST_Utterance.delete_EST_Utterance):
        try:
            if self.thisown: destroy(self)
        except: pass

    def init(*args): return _EST_Utterance.EST_Utterance_init(*args)
    def clear(*args): return _EST_Utterance.EST_Utterance_clear(*args)
    def clear_relations(*args): return _EST_Utterance.EST_Utterance_clear_relations(*args)
    def set_highest_id(*args): return _EST_Utterance.EST_Utterance_set_highest_id(*args)
    def next_id(*args): return _EST_Utterance.EST_Utterance_next_id(*args)
    def load(*args): return _EST_Utterance.EST_Utterance_load(*args)
    def save(*args): return _EST_Utterance.EST_Utterance_save(*args)
    def evaluate_all_features(*args): return _EST_Utterance.EST_Utterance_evaluate_all_features(*args)
    def num_relations(*args): return _EST_Utterance.EST_Utterance_num_relations(*args)
    def relation_present(*args): return _EST_Utterance.EST_Utterance_relation_present(*args)
    def relation(*args): return _EST_Utterance.EST_Utterance_relation(*args)
    def id(*args): return _EST_Utterance.EST_Utterance_id(*args)
    def create_relation(*args): return _EST_Utterance.EST_Utterance_create_relation(*args)
    def remove_relation(*args): return _EST_Utterance.EST_Utterance_remove_relation(*args)
    def sub_utterance(*args): return _EST_Utterance.EST_Utterance_sub_utterance(*args)

class EST_UtterancePtr(EST_Utterance):
    def __init__(self, this):
        _swig_setattr(self, EST_Utterance, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, EST_Utterance, 'thisown', 0)
        self.__class__ = EST_Utterance
_EST_Utterance.EST_Utterance_swigregister(EST_UtterancePtr)


sub_utterance = _EST_Utterance.sub_utterance

get_utt = _EST_Utterance.get_utt


utterance_merge = _EST_Utterance.utterance_merge

