#include "room.h"

object monster;

#undef EXTRA_RESET
#define EXTRA_RESET\
    if (!monster || !living(monster)) {\
	object money;\
	monster = clone_object("obj/monster");\
	monster->set_name("lion");\
	monster->set_alias("mountain lion");\
	monster->set_level(14);\
	monster->set_hp(170);\
	monster->set_wc(18);\
	monster->set_ac(3);\
	monster->set_al(-50);\
	monster->set_aggressive(1);\
	monster->set_short("A mountain lion");\
	monster->set_long(\
"A powerful mountain lion with tawny fur and rippling muscle.\n"\
"It crouches on a rocky ledge, watching you with cold yellow eyes.\n");\
	move_object(monster, this_object());\
	money = clone_object("obj/money");\
	money->set_money(random(80));\
	move_object(money, monster);\
    }
TWO_EXIT("room/mount_pass", "down",
	 "room/mount_top", "up",
	 "Ravine",
	 "You are in a narrow ravine between mountains. Loose rocks\n"
	 "and gravel make the footing treacherous. A trail leads up\n"
	 "toward the summit and down toward the mountain pass.\n", 1)
