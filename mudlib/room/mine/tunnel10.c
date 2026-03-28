#include "../room.h"

object monster;

#undef EXTRA_RESET
#define EXTRA_RESET\
    if (!monster || !living(monster)) {\
	object money;\
	monster = clone_object("obj/monster");\
	monster->set_name("rat");\
	monster->set_alias("giant rat");\
	monster->set_level(7);\
	monster->set_hp(80);\
	monster->set_wc(10);\
	monster->set_ac(1);\
	monster->set_al(-30);\
	monster->set_aggressive(1);\
	monster->set_short("A giant rat");\
	monster->set_long(\
"An enormous rat the size of a large dog, with matted fur and\n"\
"yellow teeth. Its eyes gleam with feral hunger.\n");\
	move_object(monster, this_object());\
	money = clone_object("obj/money");\
	money->set_money(random(30));\
	move_object(money, monster);\
    }
TWO_EXIT("room/mine/tunnel9", "east",
	 "room/mine/tunnel11", "west",
	 "Tunnel",
	 "In the tunnel into the mines. Gnaw marks cover the wooden\nsupport beams and small bones litter the floor.\n", 0)
