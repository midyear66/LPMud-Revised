#include "../room.h"

object monster;

#undef EXTRA_RESET
#define EXTRA_RESET\
    if (!monster || !living(monster)) {\
	object money;\
	monster = clone_object("obj/monster");\
	monster->set_name("beetle");\
	monster->set_alias("mine beetle");\
	monster->set_level(8);\
	monster->set_hp(90);\
	monster->set_wc(12);\
	monster->set_ac(2);\
	monster->set_al(-20);\
	monster->set_aggressive(1);\
	monster->set_short("A mine beetle");\
	monster->set_long(\
"A beetle the size of a shield, with a thick armored carapace\n"\
"and mandibles strong enough to crack stone.\n");\
	move_object(monster, this_object());\
	money = clone_object("obj/money");\
	money->set_money(random(40));\
	move_object(money, monster);\
    }
TWO_EXIT("room/mine/tunnel11", "south",
	 "room/mine/tunnel13", "north",
	 "Tunnel",
	 "In the tunnel into the mines. The walls here are scored\nwith deep grooves, as if something has been burrowing.\n", 0)
