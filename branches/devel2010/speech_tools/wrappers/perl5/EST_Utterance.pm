# This file was automatically generated by SWIG
package EST_Utterance;
require Exporter;
require DynaLoader;
@ISA = qw(Exporter DynaLoader);
require EST_Item;
require EST_Relation;
package EST_Utterancec;
bootstrap EST_Utterance;
package EST_Utterance;
@EXPORT = qw( );

# ---------- BASE METHODS -------------

package EST_Utterance;

sub TIEHASH {
    my ($classname,$obj) = @_;
    return bless $obj, $classname;
}

sub CLEAR { }

sub FIRSTKEY { }

sub NEXTKEY { }

sub FETCH {
    my ($self,$field) = @_;
    my $member_func = "swig_${field}_get";
    $self->$member_func();
}

sub STORE {
    my ($self,$field,$newval) = @_;
    my $member_func = "swig_${field}_set";
    $self->$member_func($newval);
}

sub this {
    my $ptr = shift;
    return tied(%$ptr);
}


# ------- FUNCTION WRAPPERS --------

package EST_Utterance;

*utterance_merge = *EST_Utterancec::utterance_merge;
*sub_utterance = *EST_Utterancec::sub_utterance;
*get_utt = *EST_Utterancec::get_utt;

############# Class : EST_Utterance::EST_Utterance ##############

package EST_Utterance::EST_Utterance;
@ISA = qw( EST_Utterance );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = EST_Utterancec::new_EST_Utterance(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        EST_Utterancec::delete_EST_Utterance($self);
        delete $OWNER{$self};
    }
}

*init = *EST_Utterancec::EST_Utterance_init;
*clear = *EST_Utterancec::EST_Utterance_clear;
*clear_relations = *EST_Utterancec::EST_Utterance_clear_relations;
*set_highest_id = *EST_Utterancec::EST_Utterance_set_highest_id;
*next_id = *EST_Utterancec::EST_Utterance_next_id;
*load = *EST_Utterancec::EST_Utterance_load;
*save = *EST_Utterancec::EST_Utterance_save;
*evaluate_all_features = *EST_Utterancec::EST_Utterance_evaluate_all_features;
*num_relations = *EST_Utterancec::EST_Utterance_num_relations;
*relation_present = *EST_Utterancec::EST_Utterance_relation_present;
*relation = *EST_Utterancec::EST_Utterance_relation;
*id = *EST_Utterancec::EST_Utterance_id;
*create_relation = *EST_Utterancec::EST_Utterance_create_relation;
*remove_relation = *EST_Utterancec::EST_Utterance_remove_relation;
*sub_utterance = *EST_Utterancec::EST_Utterance_sub_utterance;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


# ------- VARIABLE STUBS --------

package EST_Utterance;

1;