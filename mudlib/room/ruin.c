#include "std.h"

object monster;

int id(string str)
{
    if (str == "ruin")
	return 1;
    else
	return 0;
}

#undef EXTRA_RESET
#define EXTRA_RESET\
    if (!monster || !living(monster)) {\
	object money, gem;\
	monster = clone_object("obj/monster");\
	monster->set_name("golem");\
	monster->set_alias("stone golem");\
	monster->set_level(11);\
	monster->set_hp(140);\
	monster->set_wc(14);\
	monster->set_ac(4);\
	monster->set_al(-50);\
	monster->set_aggressive(1);\
	monster->set_short("A stone golem");\
	monster->set_long(\
"An ancient guardian made of rough-hewn stone, animated by\n"\
"some long-forgotten enchantment. Cracks run through its body\n"\
"but it still moves with terrible purpose.\n");\
	move_object(monster, this_object());\
	money = clone_object("obj/money");\
	money->set_money(random(70));\
	move_object(money, monster);\
	gem = clone_object("obj/treasure");\
	gem->set_id("gemstone");\
	gem->set_alias("cracked gemstone");\
	gem->set_short("A cracked gemstone");\
	gem->set_long(\
"A gemstone with a deep crack running through it, still faintly\n"\
"glowing with residual magic.\n");\
	gem->set_value(100);\
	gem->set_weight(1);\
	move_object(gem, monster);\
    }
FOUR_EXIT("room/plane4", "south",
	  "room/plane8", "north",
	  "room/plane9", "east",
	  "room/plane3", "west",
	  "Ruin",
	  "A very old looking ruin. There is no roof, and no door.\n"
	  "Crumbling stone walls surround an ancient courtyard where\n"
	  "a weathered statue stands guard.\n",
	  1)
