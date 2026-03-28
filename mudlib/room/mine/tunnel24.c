#include "../room.h"

object monster;

#undef EXTRA_RESET
#define EXTRA_RESET\
    if (!monster || !living(monster)) {\
	object money;\
	monster = clone_object("obj/monster");\
	monster->set_name("worm");\
	monster->set_alias("rock worm");\
	monster->set_level(10);\
	monster->set_hp(110);\
	monster->set_wc(13);\
	monster->set_ac(3);\
	monster->set_al(-50);\
	monster->set_aggressive(1);\
	monster->set_spell_chance(15);\
	monster->set_spell_dam(15);\
	monster->set_spell_mesg(\
"The rock worm spits a jet of acid at you!\n");\
	monster->set_short("A rock worm");\
	monster->set_long(\
"A massive segmented worm with a mouth ringed by grinding\n"\
"teeth. It burrows through solid rock with ease.\n");\
	move_object(monster, this_object());\
	money = clone_object("obj/money");\
	money->set_money(random(60));\
	move_object(money, monster);\
    }
TWO_EXIT("room/mine/tunnel23", "west",
	 "room/mine/tunnel25", "down",
	 "Tunnel",
	 "The tunnel slopes steeply down a hole here. The walls are\nscored with circular bore marks from some burrowing creature.\n", 0)
