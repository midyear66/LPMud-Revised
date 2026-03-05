#include "room/room.h"
#undef EXTRA_RESET
#define EXTRA_RESET extra_reset();

void extra_reset() {
    object orc, weapon;
    int i;

    if (!present("orc")) {
        i = 0;
        while (i < 3) {
            i += 1;
            orc = clone_object("obj/monster");
            orc->set_name("orc");
            orc->set_alias("orc warrior");
            orc->set_alt_name("warrior");
            orc->set_race("orc");
            orc->set_level(random(2) + 2);
            orc->set_hp(35);
            orc->set_al(-60);
            orc->set_short("An orc warrior");
            orc->set_long("A scarred orc eating something unidentifiable.\n");
            orc->set_aggressive(1);
            orc->load_a_chat(30, ({
                "Orc warrior says: You interrupted my meal!\n",
                "Orc warrior throws a bone at you.\n",
                "Orc warrior belches loudly.\n",
                "Orc warrior says: More meat for the stew!\n"
            }));
            weapon = clone_object("obj/weapon");
            weapon->set_name("cleaver");
            weapon->set_class(7);
            weapon->set_value(12);
            weapon->set_weight(2);
            weapon->set_alt_name("orc cleaver");
            transfer(weapon, orc);
            command("wield cleaver", orc);
            move_object(orc, this_object());
        }
    }
}

THREE_EXIT("room/orc_fortress/gate", "east",
     "room/orc_fortress/armory", "west",
     "room/orc_fortress/pit", "south",
     "The mess hall",
     "A large open area serves as the orc mess hall. Long crude tables\n" +
     "are covered with scraps of rancid food and gnawed bones. Flies\n" +
     "buzz around piles of refuse in the corners. The smell is almost\n" +
     "unbearable.\n", 1)
