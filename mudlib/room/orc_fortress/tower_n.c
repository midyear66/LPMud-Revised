#include "room/room.h"
#undef EXTRA_RESET
#define EXTRA_RESET extra_reset();

void extra_reset() {
    object orc, weapon;

    if (!present("orc")) {
        orc = clone_object("obj/monster");
        orc->set_name("orc");
        orc->set_alias("orc archer");
        orc->set_alt_name("archer");
        orc->set_race("orc");
        orc->set_level(4);
        orc->set_hp(40);
        orc->set_al(-60);
        orc->set_short("An orc archer");
        orc->set_long("An orc perched in the watchtower, scanning the horizon.\n");
        orc->set_aggressive(1);
        orc->load_a_chat(30, ({
            "Orc archer says: I see you, maggot!\n",
            "Orc archer nocks another arrow.\n",
            "Orc archer says: Nowhere to run!\n"
        }));
        weapon = clone_object("obj/weapon");
        weapon->set_name("short bow");
        weapon->set_class(9);
        weapon->set_value(20);
        weapon->set_weight(1);
        weapon->set_alt_name("bow");
        transfer(weapon, orc);
        command("wield short bow", orc);
        move_object(orc, this_object());
    }
}

TWO_EXIT("room/orc_fortress/gate", "south",
     "room/orc_fortress/wall_n", "west",
     "The north watchtower",
     "You have climbed a rickety ladder to the top of a watchtower.\n" +
     "From here you can see over the palisade walls across the\n" +
     "surrounding valley. A narrow walkway leads along the wall\n" +
     "to the west.\n", 1)
