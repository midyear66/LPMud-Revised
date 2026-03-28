#include "../room.h"

object monster;

#undef EXTRA_RESET
#define EXTRA_RESET\
    if (!monster || !living(monster)) {\
	object money, pick;\
	monster = clone_object("obj/monster");\
	monster->set_name("foreman");\
	monster->set_alias("mine foreman");\
	monster->set_level(12);\
	monster->set_hp(150);\
	monster->set_wc(15);\
	monster->set_ac(3);\
	monster->set_al(-400);\
	monster->set_aggressive(1);\
	monster->set_short("The mine foreman");\
	monster->set_long(\
"A skeletal figure still wearing tattered work clothes and a\n"\
"cracked leather helmet. It grips a rusted pickaxe in bony\n"\
"hands and its hollow eye sockets glow with a faint red light.\n");\
	move_object(monster, this_object());\
	money = clone_object("obj/money");\
	money->set_money(random(100));\
	move_object(money, monster);\
	pick = clone_object("obj/treasure");\
	pick->set_id("pickaxe");\
	pick->set_alias("pick");\
	pick->set_short("An iron pickaxe");\
	pick->set_long(\
"A heavy iron pickaxe, rusted but still solid.\n");\
	pick->set_value(120);\
	pick->set_weight(3);\
	move_object(pick, monster);\
    }
THREE_EXIT("room/mine/tunnel19", "south",
	 "room/mine/tunnel21", "west",
	 "room/mine/tunnel23", "east",
	 "Tunnel",
	 "A tunnel fork. Old mining equipment lies scattered about,\ncovered in dust. The air here is cold and stale.\n", 0)
