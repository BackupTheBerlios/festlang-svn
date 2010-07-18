# This file was created automatically by SWIG 1.3.27.
# Don't modify this file, modify the SWIG interface instead.

import _EST_FVector

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


class floatvector(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, floatvector, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, floatvector, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ EST_TVector<float > instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, floatvector, 'this', _EST_FVector.new_floatvector(*args))
        _swig_setattr(self, floatvector, 'thisown', 1)
    def __del__(self, destroy=_EST_FVector.delete_floatvector):
        try:
            if self.thisown: destroy(self)
        except: pass

    def resize(*args): return _EST_FVector.floatvector_resize(*args)
    def num_columns(*args): return _EST_FVector.floatvector_num_columns(*args)
    def length(*args): return _EST_FVector.floatvector_length(*args)
    def n(*args): return _EST_FVector.floatvector_n(*args)
    def a_no_check(*args): return _EST_FVector.floatvector_a_no_check(*args)
    def a_no_check_1(*args): return _EST_FVector.floatvector_a_no_check_1(*args)
    def a_check(*args): return _EST_FVector.floatvector_a_check(*args)
    def a(*args): return _EST_FVector.floatvector_a(*args)
    def fill(*args): return _EST_FVector.floatvector_fill(*args)
    def empty(*args): return _EST_FVector.floatvector_empty(*args)
    def __eq__(*args): return _EST_FVector.floatvector___eq__(*args)
    def __ne__(*args): return _EST_FVector.floatvector___ne__(*args)
    def __setitem__(*args): return _EST_FVector.floatvector___setitem__(*args)
    def __getitem__(*args): return _EST_FVector.floatvector___getitem__(*args)
    def copy_section(*args): return _EST_FVector.floatvector_copy_section(*args)
    def set_section(*args): return _EST_FVector.floatvector_set_section(*args)

class floatvectorPtr(floatvector):
    def __init__(self, this):
        _swig_setattr(self, floatvector, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, floatvector, 'thisown', 0)
        self.__class__ = floatvector
_EST_FVector.floatvector_swigregister(floatvectorPtr)

class floatsimplevector(floatvector):
    __swig_setmethods__ = {}
    for _s in [floatvector]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, floatsimplevector, name, value)
    __swig_getmethods__ = {}
    for _s in [floatvector]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, floatsimplevector, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ EST_TSimpleVector<float > instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, floatsimplevector, 'this', _EST_FVector.new_floatsimplevector(*args))
        _swig_setattr(self, floatsimplevector, 'thisown', 1)
    def resize(*args): return _EST_FVector.floatsimplevector_resize(*args)
    def copy_section(*args): return _EST_FVector.floatsimplevector_copy_section(*args)
    def set_section(*args): return _EST_FVector.floatsimplevector_set_section(*args)
    def zero(*args): return _EST_FVector.floatsimplevector_zero(*args)
    def empty(*args): return _EST_FVector.floatsimplevector_empty(*args)
    def __del__(self, destroy=_EST_FVector.delete_floatsimplevector):
        try:
            if self.thisown: destroy(self)
        except: pass


class floatsimplevectorPtr(floatsimplevector):
    def __init__(self, this):
        _swig_setattr(self, floatsimplevector, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, floatsimplevector, 'thisown', 0)
        self.__class__ = floatsimplevector
_EST_FVector.floatsimplevector_swigregister(floatsimplevectorPtr)

class EST_FVector(floatsimplevector):
    __swig_setmethods__ = {}
    for _s in [floatsimplevector]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, EST_FVector, name, value)
    __swig_getmethods__ = {}
    for _s in [floatsimplevector]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, EST_FVector, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ EST_FVector instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, EST_FVector, 'this', _EST_FVector.new_EST_FVector(*args))
        _swig_setattr(self, EST_FVector, 'thisown', 1)
    def __iadd__(*args): return _EST_FVector.EST_FVector___iadd__(*args)
    def __imul__(*args): return _EST_FVector.EST_FVector___imul__(*args)
    def __idiv__(*args): return _EST_FVector.EST_FVector___idiv__(*args)
    def est_save(*args): return _EST_FVector.EST_FVector_est_save(*args)
    def save(*args): return _EST_FVector.EST_FVector_save(*args)
    def load(*args): return _EST_FVector.EST_FVector_load(*args)
    def est_load(*args): return _EST_FVector.EST_FVector_est_load(*args)
    def randomise(*args): return _EST_FVector.EST_FVector_randomise(*args)
    def sum(*args): return _EST_FVector.EST_FVector_sum(*args)
    def __del__(self, destroy=_EST_FVector.delete_EST_FVector):
        try:
            if self.thisown: destroy(self)
        except: pass


class EST_FVectorPtr(EST_FVector):
    def __init__(self, this):
        _swig_setattr(self, EST_FVector, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, EST_FVector, 'thisown', 0)
        self.__class__ = EST_FVector
_EST_FVector.EST_FVector_swigregister(EST_FVectorPtr)


add = _EST_FVector.add

subtract = _EST_FVector.subtract

sqrt = _EST_FVector.sqrt

topower = _EST_FVector.topower

polynomial_value = _EST_FVector.polynomial_value


polynomial_fit = _EST_FVector.polynomial_fit

