#include "../room.h"

object monster;

#undef EXTRA_RESET
#define EXTRA_RESET\
    if (!monster || !living(monster)) {\
	object money;\
	monster = clone_object("obj/monster");\
	monster->set_name("troll");\
	monster->set_alias("cave troll");\
	monster->set_level(11);\
	monster->set_hp(130);\
	monster->set_wc(14);\
	monster->set_ac(2);\
	monster->set_al(-200);\
	monster->set_aggressive(1);\
	monster->set_short("A cave troll");\
	monster->set_long(\
"A hulking cave troll with grey, stone-like skin and long arms\n"\
"that drag along the ground. It smells terrible.\n");\
	move_object(monster, this_object());\
	money = clone_object("obj/money");\
	money->set_money(random(80));\
	move_object(money, monster);\
    }
ONE_EXIT("room/mine/tunnel21", "east",
	 "Dead end",
	 "A foul-smelling dead end littered with bones and refuse.\n"
	 "Something large has been using this as a lair.\n", 0)
