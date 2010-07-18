# This file was created automatically by SWIG 1.3.27.
# Don't modify this file, modify the SWIG interface instead.
import EST_Wave
import EST_Track
import EST_FVector

import _EST_pitchmark

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



neg_zero_cross_pick = _EST_pitchmark.neg_zero_cross_pick

pm_fill = _EST_pitchmark.pm_fill

pm_min_check = _EST_pitchmark.pm_min_check


pitchmark = _EST_pitchmark.pitchmark

pm_to_f0 = _EST_pitchmark.pm_to_f0

