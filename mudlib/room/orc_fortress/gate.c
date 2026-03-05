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
            orc->set_alias("orc guard");
            orc->set_alt_name("guard");
            orc->set_race("orc");
            orc->set_level(3);
            orc->set_hp(40);
            orc->set_al(-60);
            orc->set_short("An orc guard");
            orc->set_long("A burly orc standing guard at the gate.\n");
            orc->set_aggressive(1);
            orc->load_a_chat(30, ({
                "Orc guard says: No humans allowed!\n",
                "Orc guard says: Intruder!\n",
                "Orc guard says: Sound the alarm!\n",
                "Orc guard grunts and swings wildly.\n"
            }));
            weapon = clone_object("obj/weapon");
            weapon->set_name("spear");
            weapon->set_class(8);
            weapon->set_value(20);
            weapon->set_weight(2);
            weapon->set_alt_name("orc spear");
            transfer(weapon, orc);
            command("wield spear", orc);
            move_object(orc, this_object());
        }
    }
}

THREE_EXIT("room/fortress", "east",
     "room/orc_fortress/mess", "west",
     "room/orc_fortress/hall", "south",
     "The gate of the orc fortress",
     "You stand at the gate of a crude orc fortress. Rough-hewn logs form\n" +
     "a palisade wall stretching in both directions. The stench of orcs\n" +
     "hangs heavy in the air. To the east lies the valley, and the fortress\n" +
     "sprawls westward and southward from here.\n", 1)
