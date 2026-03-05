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
            orc->set_alias("orc elite");
            orc->set_alt_name("elite");
            orc->set_race("orc");
            orc->set_level(6);
            orc->set_hp(65);
            orc->set_al(-80);
            orc->set_short("An orc elite guard");
            orc->set_long("A heavily armored orc elite, hand-picked by the king himself.\n");
            orc->set_aggressive(1);
            orc->set_ac(2);
            orc->load_a_chat(30, ({
                "Orc elite says: None shall pass!\n",
                "Orc elite says: The king must not be disturbed!\n",
                "Orc elite guard stands firm.\n"
            }));
            weapon = clone_object("obj/weapon");
            weapon->set_name("broad sword");
            weapon->set_class(12);
            weapon->set_value(45);
            weapon->set_weight(2);
            weapon->set_alt_name("sword");
            transfer(weapon, orc);
            command("wield broad sword", orc);
            move_object(orc, this_object());
        }
    }
}

THREE_EXIT("room/orc_fortress/pit", "north",
     "room/orc_fortress/cells", "east",
     "room/orc_fortress/throne", "south",
     "The stairs",
     "Rough stone steps descend deeper into the fortress. The walls\n" +
     "here are damp stone rather than timber, and the air grows colder.\n" +
     "Torches gutter in iron brackets, barely holding back the darkness.\n" +
     "The stairs lead down to the south, toward the heart of the fortress.\n", 1)
