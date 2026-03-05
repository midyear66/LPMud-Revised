#include "room/room.h"
#undef EXTRA_RESET
#define EXTRA_RESET extra_reset();

void extra_reset() {
    object orc, weapon;
    int i;

    if (!present("orc")) {
        i = 0;
        while (i < 4) {
            i += 1;
            orc = clone_object("obj/monster");
            orc->set_name("orc");
            orc->set_alias("orc soldier");
            orc->set_alt_name("soldier");
            orc->set_race("orc");
            orc->set_level(random(2) + 3);
            orc->set_hp(45);
            orc->set_al(-60);
            orc->set_short("An orc soldier");
            orc->set_long("A battle-hardened orc soldier in crude leather armor.\n");
            orc->set_aggressive(1);
            orc->load_a_chat(30, ({
                "Orc soldier says: For the king!\n",
                "Orc soldier says: Die, human filth!\n",
                "Orc soldier roars a battle cry.\n",
                "Orc soldier says: You won't leave here alive!\n"
            }));
            weapon = clone_object("obj/weapon");
            weapon->set_name("scimitar");
            weapon->set_class(10);
            weapon->set_value(30);
            weapon->set_weight(2);
            weapon->set_alt_name("sword");
            transfer(weapon, orc);
            command("wield scimitar", orc);
            move_object(orc, this_object());
        }
    }
}

ONE_EXIT("room/orc_fortress/armory", "east",
     "The barracks",
     "Rows of filthy straw pallets fill this long room. Weapons and bits\n" +
     "of armor are scattered everywhere. The orcs who sleep here clearly\n" +
     "care nothing for order or hygiene. A foul draft blows through\n" +
     "cracks in the timber walls.\n", 1)
