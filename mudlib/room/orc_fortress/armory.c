#include "room/room.h"
#undef EXTRA_RESET
#define EXTRA_RESET extra_reset();

void extra_reset() {
    object orc, weapon;

    if (!present("orc")) {
        orc = clone_object("obj/monster");
        orc->set_name("orc");
        orc->set_alias("orc smith");
        orc->set_alt_name("smith");
        orc->set_race("orc");
        orc->set_level(5);
        orc->set_hp(60);
        orc->set_al(-70);
        orc->set_short("An orc smith");
        orc->set_long("A massive orc covered in soot, hammering away at crude weapons.\n");
        orc->set_aggressive(1);
        orc->load_a_chat(30, ({
            "Orc smith says: I'll hammer your skull flat!\n",
            "Orc smith swings a red-hot piece of metal at you.\n",
            "Orc smith says: You'll make a fine anvil!\n"
        }));
        weapon = clone_object("obj/weapon");
        weapon->set_name("war hammer");
        weapon->set_class(12);
        weapon->set_value(40);
        weapon->set_weight(3);
        weapon->set_alt_name("hammer");
        transfer(weapon, orc);
        command("wield war hammer", orc);
        move_object(orc, this_object());
    }
}

TWO_EXIT("room/orc_fortress/mess", "east",
     "room/orc_fortress/barracks", "west",
     "The armory",
     "Racks of crude weapons line the walls -- jagged swords, notched\n" +
     "axes, and bent spears. A forge glows in the corner, its heat\n" +
     "adding to the oppressive atmosphere. The orc smith works here\n" +
     "day and night, forging instruments of war.\n", 1)
