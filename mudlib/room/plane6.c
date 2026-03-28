#include "room.h"

object monster;

#undef EXTRA_RESET
#define EXTRA_RESET\
    if (!monster || !living(monster)) {\
	object money;\
	monster = clone_object("obj/monster");\
	monster->set_name("bandit");\
	monster->set_alias("plains bandit");\
	monster->set_level(9);\
	monster->set_hp(100);\
	monster->set_wc(12);\
	monster->set_ac(2);\
	monster->set_al(-200);\
	monster->set_aggressive(1);\
	monster->set_short("A plains bandit");\
	monster->set_long(\
"A rough-looking bandit in worn leather armor, scarred and\n"\
"sun-weathered from a life on the open plains. He eyes you\n"\
"with greedy intent.\n");\
	move_object(monster, this_object());\
	money = clone_object("obj/money");\
	money->set_money(random(60));\
	move_object(money, monster);\
    }
FOUR_EXIT("room/plane3", "south",
	  "room/plane11", "north",
	  "room/plane8", "east",
	  "room/plane10", "west",
	  "A large open plain",
	  "A large open plain. A well-worn track cuts across the grass\n"
	  "here, suggesting frequent travel.\n",
	  1)
