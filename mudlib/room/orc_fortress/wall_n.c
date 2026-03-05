#include "room/room.h"
#undef EXTRA_RESET
#define EXTRA_RESET extra_reset();

void extra_reset() {
    object orc, weapon;

    if (!present("orc")) {
        orc = clone_object("obj/monster");
        orc->set_name("orc");
        orc->set_alias("orc sentry");
        orc->set_alt_name("sentry");
        orc->set_race("orc");
        orc->set_level(3);
        orc->set_hp(35);
        orc->set_al(-60);
        orc->set_short("An orc sentry");
        orc->set_long("An orc pacing back and forth along the wall, grumbling.\n");
        orc->set_aggressive(1);
        orc->load_a_chat(30, ({
            "Orc sentry says: Nothing ever happens up here.\n",
            "Orc sentry says: Wait -- who goes there?!\n",
            "Orc sentry peers over the wall nervously.\n"
        }));
        weapon = clone_object("obj/weapon");
        weapon->set_name("short sword");
        weapon->set_class(8);
        weapon->set_value(15);
        weapon->set_weight(2);
        weapon->set_alt_name("sword");
        transfer(weapon, orc);
        command("wield short sword", orc);
        move_object(orc, this_object());
    }
}

TWO_EXIT("room/orc_fortress/tower_n", "east",
     "room/orc_fortress/tower_ne", "west",
     "The north wall",
     "A narrow wooden walkway runs along the top of the palisade wall.\n" +
     "The planks creak under your weight. Below you can see the\n" +
     "fortress compound. Watchtowers stand at each end of the walkway.\n", 1)
