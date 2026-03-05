#include "room/room.h"
#undef EXTRA_RESET
#define EXTRA_RESET extra_reset();

void extra_reset() {
    object orc, weapon;
    int i;

    if (!present("orc")) {
        i = 0;
        while (i < 2) {
            i += 1;
            orc = clone_object("obj/monster");
            orc->set_name("orc");
            orc->set_alias("orc brute");
            orc->set_alt_name("brute");
            orc->set_race("orc");
            orc->set_level(4);
            orc->set_hp(50);
            orc->set_al(-70);
            orc->set_short("An orc brute");
            orc->set_long("A hulking orc with thick slabs of muscle and a mean look.\n");
            orc->set_aggressive(1);
            orc->load_a_chat(30, ({
                "Orc brute growls menacingly.\n",
                "Orc brute says: Crush your bones!\n",
                "Orc brute pounds its fists together.\n"
            }));
            weapon = clone_object("obj/weapon");
            weapon->set_name("mace");
            weapon->set_class(10);
            weapon->set_value(25);
            weapon->set_weight(3);
            weapon->set_alt_name("orc mace");
            transfer(weapon, orc);
            command("wield mace", orc);
            move_object(orc, this_object());
        }
    }
}

THREE_EXIT("room/orc_fortress/gate", "north",
     "room/orc_fortress/pit", "west",
     "room/orc_fortress/cells", "south",
     "The great hall",
     "A cavernous hall stretches before you, its ceiling supported by\n" +
     "rough timber pillars adorned with skulls and tattered banners.\n" +
     "Crude torches flicker on the walls, casting dancing shadows.\n" +
     "The floor is stained dark with old blood.\n", 1)
