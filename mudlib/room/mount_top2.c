#include "room.h"

object monster;

#undef EXTRA_RESET
#define EXTRA_RESET\
    if (!monster || !living(monster)) {\
	object money, feather;\
	monster = clone_object("obj/monster");\
	monster->set_name("eagle");\
	monster->set_alias("storm eagle");\
	monster->set_level(16);\
	monster->set_hp(200);\
	monster->set_wc(20);\
	monster->set_ac(4);\
	monster->set_al(-100);\
	monster->set_aggressive(1);\
	monster->set_spell_chance(25);\
	monster->set_spell_dam(30);\
	monster->set_spell_mesg(\
"The storm eagle shrieks and sends a bolt of lightning through you!\n");\
	monster->set_short("A storm eagle");\
	monster->set_long(\
"A massive eagle with a wingspan wider than a man is tall.\n"\
"Crackling sparks dance along its feathers and its eyes glow\n"\
"with an inner storm.\n");\
	move_object(monster, this_object());\
	money = clone_object("obj/money");\
	money->set_money(random(150));\
	move_object(money, monster);\
	feather = clone_object("obj/treasure");\
	feather->set_id("feather");\
	feather->set_alias("eagle feather");\
	feather->set_short("A storm eagle feather");\
	feather->set_long(\
"A large feather that crackles with residual static.\n");\
	feather->set_value(250);\
	feather->set_weight(1);\
	move_object(feather, monster);\
    }
ONE_EXIT("room/mount_top", "west",
	 "Plateau",
	 "You are on a large, open plateau on top of the mountain.\n"
	 "The view is fantastic in all directions and the clouds\n"
	 "that rush past above feel so close you could almost\n"
	 "touch them. Strong winds whip across the exposed rock.\n", 1)
