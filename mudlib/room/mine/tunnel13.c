#include "../room.h"

object monster;

#undef EXTRA_RESET
#define EXTRA_RESET\
    if (!monster || !living(monster)) {\
	object money;\
	monster = clone_object("obj/monster");\
	monster->set_name("spider");\
	monster->set_alias("tunnel spider");\
	monster->set_level(9);\
	monster->set_hp(100);\
	monster->set_wc(11);\
	monster->set_ac(2);\
	monster->set_al(-30);\
	monster->set_aggressive(1);\
	monster->set_spell_chance(10);\
	monster->set_spell_dam(10);\
	monster->set_spell_mesg(\
"The tunnel spider sprays venom at you!\n");\
	monster->set_short("A tunnel spider");\
	monster->set_long(\
"A spider as large as a man, crouching in a web of thick silken\n"\
"strands that stretch across the tunnel. Its fangs drip venom.\n");\
	move_object(monster, this_object());\
	money = clone_object("obj/money");\
	money->set_money(random(50));\
	move_object(money, monster);\
    }
ONE_EXIT("room/mine/tunnel12", "south",
	 "Tunnel",
	 "A dead-end tunnel choked with thick cobwebs. The webs are\nfreshly spun and sticky to the touch.\n", 0)
