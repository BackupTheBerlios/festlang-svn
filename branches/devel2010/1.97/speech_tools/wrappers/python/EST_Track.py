# This file was created automatically by SWIG 1.3.27.
# Don't modify this file, modify the SWIG interface instead.
import EST_FVector

import _EST_Track

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


class EST_Track(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, EST_Track, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, EST_Track, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ EST_Track instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    __swig_getmethods__["default_frame_shift"] = _EST_Track.EST_Track_default_frame_shift_get
    if _newclass:default_frame_shift = property(_EST_Track.EST_Track_default_frame_shift_get)
    __swig_getmethods__["default_sample_rate"] = _EST_Track.EST_Track_default_sample_rate_get
    if _newclass:default_sample_rate = property(_EST_Track.EST_Track_default_sample_rate_get)
    def __init__(self, *args):
        _swig_setattr(self, EST_Track, 'this', _EST_Track.new_EST_Track(*args))
        _swig_setattr(self, EST_Track, 'thisown', 1)
    def __del__(self, destroy=_EST_Track.delete_EST_Track):
        try:
            if self.thisown: destroy(self)
        except: pass

    def resize(*args): return _EST_Track.EST_Track_resize(*args)
    def set_num_channels(*args): return _EST_Track.EST_Track_set_num_channels(*args)
    def set_num_frames(*args): return _EST_Track.EST_Track_set_num_frames(*args)
    def set_channel_name(*args): return _EST_Track.EST_Track_set_channel_name(*args)
    def set_aux_channel_name(*args): return _EST_Track.EST_Track_set_aux_channel_name(*args)
    def copy_setup(*args): return _EST_Track.EST_Track_copy_setup(*args)
    def name(*args): return _EST_Track.EST_Track_name(*args)
    def set_name(*args): return _EST_Track.EST_Track_set_name(*args)
    def frame(*args): return _EST_Track.EST_Track_frame(*args)
    def channel(*args): return _EST_Track.EST_Track_channel(*args)
    def sub_track(*args): return _EST_Track.EST_Track_sub_track(*args)
    def copy_sub_track(*args): return _EST_Track.EST_Track_copy_sub_track(*args)
    def copy_sub_track_out(*args): return _EST_Track.EST_Track_copy_sub_track_out(*args)
    def copy_channel_out(*args): return _EST_Track.EST_Track_copy_channel_out(*args)
    def copy_frame_out(*args): return _EST_Track.EST_Track_copy_frame_out(*args)
    def copy_channel_in(*args): return _EST_Track.EST_Track_copy_channel_in(*args)
    def copy_frame_in(*args): return _EST_Track.EST_Track_copy_frame_in(*args)
    def channel_position(*args): return _EST_Track.EST_Track_channel_position(*args)
    def has_channel(*args): return _EST_Track.EST_Track_has_channel(*args)
    def a(*args): return _EST_Track.EST_Track_a(*args)
    def t(*args): return _EST_Track.EST_Track_t(*args)
    def ms_t(*args): return _EST_Track.EST_Track_ms_t(*args)
    def fill_time(*args): return _EST_Track.EST_Track_fill_time(*args)
    def fill(*args): return _EST_Track.EST_Track_fill(*args)
    def sample(*args): return _EST_Track.EST_Track_sample(*args)
    def shift(*args): return _EST_Track.EST_Track_shift(*args)
    def start(*args): return _EST_Track.EST_Track_start(*args)
    def end(*args): return _EST_Track.EST_Track_end(*args)
    def load(*args): return _EST_Track.EST_Track_load(*args)
    def save(*args): return _EST_Track.EST_Track_save(*args)
    def empty(*args): return _EST_Track.EST_Track_empty(*args)
    def index(*args): return _EST_Track.EST_Track_index(*args)
    def index_below(*args): return _EST_Track.EST_Track_index_below(*args)
    def num_frames(*args): return _EST_Track.EST_Track_num_frames(*args)
    def length(*args): return _EST_Track.EST_Track_length(*args)
    def num_channels(*args): return _EST_Track.EST_Track_num_channels(*args)
    def num_aux_channels(*args): return _EST_Track.EST_Track_num_aux_channels(*args)
    def equal_space(*args): return _EST_Track.EST_Track_equal_space(*args)
    def single_break(*args): return _EST_Track.EST_Track_single_break(*args)
    def set_equal_space(*args): return _EST_Track.EST_Track_set_equal_space(*args)
    def set_single_break(*args): return _EST_Track.EST_Track_set_single_break(*args)
    def __iadd__(*args): return _EST_Track.EST_Track___iadd__(*args)
    def __ior__(*args): return _EST_Track.EST_Track___ior__(*args)
    def load_channel_names(*args): return _EST_Track.EST_Track_load_channel_names(*args)
    def save_channel_names(*args): return _EST_Track.EST_Track_save_channel_names(*args)
    def channel_name(*args): return _EST_Track.EST_Track_channel_name(*args)
    def aux_channel_name(*args): return _EST_Track.EST_Track_aux_channel_name(*args)

class EST_TrackPtr(EST_Track):
    def __init__(self, this):
        _swig_setattr(self, EST_Track, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, EST_Track, 'thisown', 0)
        self.__class__ = EST_Track
_EST_Track.EST_Track_swigregister(EST_TrackPtr)



mean = _EST_Track.mean

meansd = _EST_Track.meansd

normalise = _EST_Track.normalise

