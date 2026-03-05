#include "room/room.h"
#undef EXTRA_RESET
#define EXTRA_RESET extra_reset();

void extra_reset() {
    object orc, weapon;

    if (!present("orc")) {
        orc = clone_object("obj/monster");
        orc->set_name("orc");
        orc->set_alias("orc jailer");
        orc->set_alt_name("jailer");
        orc->set_race("orc");
        orc->set_level(5);
        orc->set_hp(55);
        orc->set_al(-70);
        orc->set_short("An orc jailer");
        orc->set_long("A squat orc jangling a ring of rusty keys. He takes\n" +
                       "obvious pleasure in tormenting his prisoners.\n");
        orc->set_aggressive(1);
        orc->load_a_chat(30, ({
            "Orc jailer says: Another one for the cells!\n",
            "Orc jailer cackles nastily.\n",
            "Orc jailer says: The king will want to see this one.\n",
            "Orc jailer rattles his keys menacingly.\n"
        }));
        weapon = clone_object("obj/weapon");
        weapon->set_name("whip");
        weapon->set_class(9);
        weapon->set_value(15);
        weapon->set_weight(1);
        weapon->set_alt_name("orc whip");
        transfer(weapon, orc);
        command("wield whip", orc);
        move_object(orc, this_object());
    }
}

TWO_EXIT("room/orc_fortress/hall", "north",
     "room/orc_fortress/stairs", "west",
     "The cells",
     "Iron-barred cells line both sides of a narrow corridor. Most are\n" +
     "empty, but a few hold the gnawed bones of former prisoners. Moans\n" +
     "echo from somewhere deeper in the fortress. The air is damp\n" +
     "and reeks of decay.\n", 1)
