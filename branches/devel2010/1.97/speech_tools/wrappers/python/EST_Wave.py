# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import _EST_Wave

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


class EST_Wave(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, EST_Wave, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, EST_Wave, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ EST_Wave instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    __swig_getmethods__["default_sample_rate"] = _EST_Wave.EST_Wave_default_sample_rate_get
    if _newclass:default_sample_rate = property(_EST_Wave.EST_Wave_default_sample_rate_get)
    def __init__(self, *args):
        _swig_setattr(self, EST_Wave, 'this', _EST_Wave.new_EST_Wave(*args))
        _swig_setattr(self, EST_Wave, 'thisown', 1)
    def __del__(self, destroy=_EST_Wave.delete_EST_Wave):
        try:
            if self.thisown: destroy(self)
        except: pass

    def a(*args): return _EST_Wave.EST_Wave_a(*args)
    def a_safe(*args): return _EST_Wave.EST_Wave_a_safe(*args)
    def set_a(*args): return _EST_Wave.EST_Wave_set_a(*args)
    def t(*args): return _EST_Wave.EST_Wave_t(*args)
    def num_samples(*args): return _EST_Wave.EST_Wave_num_samples(*args)
    def num_channels(*args): return _EST_Wave.EST_Wave_num_channels(*args)
    def sample_rate(*args): return _EST_Wave.EST_Wave_sample_rate(*args)
    def set_sample_rate(*args): return _EST_Wave.EST_Wave_set_sample_rate(*args)
    def length(*args): return _EST_Wave.EST_Wave_length(*args)
    def end(*args): return _EST_Wave.EST_Wave_end(*args)
    def have_left_context(*args): return _EST_Wave.EST_Wave_have_left_context(*args)
    def sample_type(*args): return _EST_Wave.EST_Wave_sample_type(*args)
    def set_sample_type(*args): return _EST_Wave.EST_Wave_set_sample_type(*args)
    def file_type(*args): return _EST_Wave.EST_Wave_file_type(*args)
    def set_file_type(*args): return _EST_Wave.EST_Wave_set_file_type(*args)
    def name(*args): return _EST_Wave.EST_Wave_name(*args)
    def set_name(*args): return _EST_Wave.EST_Wave_set_name(*args)
    def resize(*args): return _EST_Wave.EST_Wave_resize(*args)
    def resample(*args): return _EST_Wave.EST_Wave_resample(*args)
    def rescale(*args): return _EST_Wave.EST_Wave_rescale(*args)
    def clear(*args): return _EST_Wave.EST_Wave_clear(*args)
    def copy(*args): return _EST_Wave.EST_Wave_copy(*args)
    def fill(*args): return _EST_Wave.EST_Wave_fill(*args)
    def empty(*args): return _EST_Wave.EST_Wave_empty(*args)
    def load(*args): return _EST_Wave.EST_Wave_load(*args)
    def load_file(*args): return _EST_Wave.EST_Wave_load_file(*args)
    def save(*args): return _EST_Wave.EST_Wave_save(*args)
    def save_file(*args): return _EST_Wave.EST_Wave_save_file(*args)
    def integrity(*args): return _EST_Wave.EST_Wave_integrity(*args)
    def info(*args): return _EST_Wave.EST_Wave_info(*args)
    def play(*args): return _EST_Wave.EST_Wave_play(*args)

class EST_WavePtr(EST_Wave):
    def __init__(self, this):
        _swig_setattr(self, EST_Wave, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, EST_Wave, 'thisown', 0)
        _swig_setattr(self, EST_Wave,self.__class__,EST_Wave)
_EST_Wave.EST_Wave_swigregister(EST_WavePtr)


wave_extract_channel = _EST_Wave.wave_extract_channel

wave_combine_channels = _EST_Wave.wave_combine_channels

wave_subwave = _EST_Wave.wave_subwave

wave_divide = _EST_Wave.wave_divide

wave_extract = _EST_Wave.wave_extract

add_waves = _EST_Wave.add_waves

difference = _EST_Wave.difference

error = _EST_Wave.error

absolute = _EST_Wave.absolute

wave_info = _EST_Wave.wave_info

invert = _EST_Wave.invert

differentiate = _EST_Wave.differentiate

reverse = _EST_Wave.reverse

rms_error = _EST_Wave.rms_error

abs_error = _EST_Wave.abs_error

correlation = _EST_Wave.correlation

