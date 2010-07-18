# This file was created automatically by SWIG 1.3.27.
# Don't modify this file, modify the SWIG interface instead.
import EST_Wave

import _EST_SignalProc

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



FIR_double_filter = _EST_SignalProc.FIR_double_filter

lpc_filter = _EST_SignalProc.lpc_filter

inv_lpc_filter = _EST_SignalProc.inv_lpc_filter

lpc_filter_1 = _EST_SignalProc.lpc_filter_1

lpc_filter_fast = _EST_SignalProc.lpc_filter_fast

inv_lpc_filter_ola = _EST_SignalProc.inv_lpc_filter_ola

simple_mean_smooth = _EST_SignalProc.simple_mean_smooth

design_FIR_filter = _EST_SignalProc.design_FIR_filter

design_lowpass_FIR_filter = _EST_SignalProc.design_lowpass_FIR_filter

design_highpass_FIR_filter = _EST_SignalProc.design_highpass_FIR_filter


FIRfilter = _EST_SignalProc.FIRfilter

FIRlowpass_filter = _EST_SignalProc.FIRlowpass_filter

FIRhighpass_filter = _EST_SignalProc.FIRhighpass_filter

FIRhighpass_double_filter = _EST_SignalProc.FIRhighpass_double_filter

FIRlowpass_double_filter = _EST_SignalProc.FIRlowpass_double_filter

pre_emphasis = _EST_SignalProc.pre_emphasis

post_emphasis = _EST_SignalProc.post_emphasis

