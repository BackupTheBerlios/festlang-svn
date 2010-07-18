# This file was created automatically by SWIG 1.3.27.
# Don't modify this file, modify the SWIG interface instead.

import _EST_Ngrammar

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


class EST_Ngrammar(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, EST_Ngrammar, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, EST_Ngrammar, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ EST_Ngrammar instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    sparse = _EST_Ngrammar.EST_Ngrammar_sparse
    dense = _EST_Ngrammar.EST_Ngrammar_dense
    backoff = _EST_Ngrammar.EST_Ngrammar_backoff
    frequencies = _EST_Ngrammar.EST_Ngrammar_frequencies
    log_frequencies = _EST_Ngrammar.EST_Ngrammar_log_frequencies
    def __init__(self, *args):
        _swig_setattr(self, EST_Ngrammar, 'this', _EST_Ngrammar.new_EST_Ngrammar(*args))
        _swig_setattr(self, EST_Ngrammar, 'thisown', 1)
    def __del__(self, destroy=_EST_Ngrammar.delete_EST_Ngrammar):
        try:
            if self.thisown: destroy(self)
        except: pass

    def clear(*args): return _EST_Ngrammar.EST_Ngrammar_clear(*args)
    def init(*args): return _EST_Ngrammar.EST_Ngrammar_init(*args)
    def num_states(*args): return _EST_Ngrammar.EST_Ngrammar_num_states(*args)
    def samples(*args): return _EST_Ngrammar.EST_Ngrammar_samples(*args)
    def order(*args): return _EST_Ngrammar.EST_Ngrammar_order(*args)
    def get_vocab_length(*args): return _EST_Ngrammar.EST_Ngrammar_get_vocab_length(*args)
    def get_vocab_word(*args): return _EST_Ngrammar.EST_Ngrammar_get_vocab_word(*args)
    def get_pred_vocab_length(*args): return _EST_Ngrammar.EST_Ngrammar_get_pred_vocab_length(*args)
    def get_pred_vocab_word(*args): return _EST_Ngrammar.EST_Ngrammar_get_pred_vocab_word(*args)
    def closed_vocab(*args): return _EST_Ngrammar.EST_Ngrammar_closed_vocab(*args)
    def entry_type(*args): return _EST_Ngrammar.EST_Ngrammar_entry_type(*args)
    def representation(*args): return _EST_Ngrammar.EST_Ngrammar_representation(*args)
    def build(*args): return _EST_Ngrammar.EST_Ngrammar_build(*args)
    def accumulate(*args): return _EST_Ngrammar.EST_Ngrammar_accumulate(*args)
    def load(*args): return _EST_Ngrammar.EST_Ngrammar_load(*args)
    def save(*args): return _EST_Ngrammar.EST_Ngrammar_save(*args)
    def wordlist_index(*args): return _EST_Ngrammar.EST_Ngrammar_wordlist_index(*args)
    def predlist_index(*args): return _EST_Ngrammar.EST_Ngrammar_predlist_index(*args)
    def set_entry_type(*args): return _EST_Ngrammar.EST_Ngrammar_set_entry_type(*args)
    def set_representation(*args): return _EST_Ngrammar.EST_Ngrammar_set_representation(*args)
    def probability(*args): return _EST_Ngrammar.EST_Ngrammar_probability(*args)
    def frequency(*args): return _EST_Ngrammar.EST_Ngrammar_frequency(*args)
    def predict(*args): return _EST_Ngrammar.EST_Ngrammar_predict(*args)
    def find_state_id(*args): return _EST_Ngrammar.EST_Ngrammar_find_state_id(*args)
    def find_next_state_id(*args): return _EST_Ngrammar.EST_Ngrammar_find_next_state_id(*args)
    def reverse_probability(*args): return _EST_Ngrammar.EST_Ngrammar_reverse_probability(*args)
    def prob_dist(*args): return _EST_Ngrammar.EST_Ngrammar_prob_dist(*args)
    def fill_window_start(*args): return _EST_Ngrammar.EST_Ngrammar_fill_window_start(*args)
    def ngram_exists(*args): return _EST_Ngrammar.EST_Ngrammar_ngram_exists(*args)
    def get_backoff_weight(*args): return _EST_Ngrammar.EST_Ngrammar_get_backoff_weight(*args)
    def set_backoff_weight(*args): return _EST_Ngrammar.EST_Ngrammar_set_backoff_weight(*args)
    def print_freqs(*args): return _EST_Ngrammar.EST_Ngrammar_print_freqs(*args)
    def compute_backoff_weights(*args): return _EST_Ngrammar.EST_Ngrammar_compute_backoff_weights(*args)
    def merge(*args): return _EST_Ngrammar.EST_Ngrammar_merge(*args)

class EST_NgrammarPtr(EST_Ngrammar):
    def __init__(self, this):
        _swig_setattr(self, EST_Ngrammar, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, EST_Ngrammar, 'thisown', 0)
        self.__class__ = EST_Ngrammar
_EST_Ngrammar.EST_Ngrammar_swigregister(EST_NgrammarPtr)


fs_build_backoff_ngrams = _EST_Ngrammar.fs_build_backoff_ngrams

fs_backoff_smooth = _EST_Ngrammar.fs_backoff_smooth

Ngram_freqsmooth = _EST_Ngrammar.Ngram_freqsmooth


frequency_of_frequencies = _EST_Ngrammar.frequency_of_frequencies

map_frequencies = _EST_Ngrammar.map_frequencies

Good_Turing_smooth = _EST_Ngrammar.Good_Turing_smooth

Good_Turing_discount = _EST_Ngrammar.Good_Turing_discount

test_stats = _EST_Ngrammar.test_stats

