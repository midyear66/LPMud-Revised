#include "room/room.h"
#undef EXTRA_RESET
#define EXTRA_RESET extra_reset();

object chest;

void extra_reset() {
    object orc, weapon;

    if (!present("orc")) {
        orc = clone_object("obj/monster");
        orc->set_name("orc");
        orc->set_alias("orc captain");
        orc->set_alt_name("captain");
        orc->set_race("orc");
        orc->set_level(6);
        orc->set_hp(70);
        orc->set_al(-80);
        orc->set_short("An orc captain");
        orc->set_long("A grizzled orc wearing a crude iron helmet. Scars criss-cross\n" +
                       "his arms and face. He commands the wall garrison.\n");
        orc->set_aggressive(1);
        orc->set_chance(15);
        orc->set_spell_mess1("Orc captain bellows a war cry that shakes your resolve!\n");
        orc->set_spell_mess2("Orc captain bellows a war cry that shakes your resolve!\n");
        orc->set_spell_dam(8);
        orc->load_a_chat(30, ({
            "Orc captain says: I've killed dozens like you!\n",
            "Orc captain says: You dare challenge me?!\n",
            "Orc captain roars with fury.\n"
        }));
        weapon = clone_object("obj/weapon");
        weapon->set_name("battle axe");
        weapon->set_class(13);
        weapon->set_value(50);
        weapon->set_weight(3);
        weapon->set_alt_name("axe");
        transfer(weapon, orc);
        command("wield battle axe", orc);
        move_object(orc, this_object());
    }
}

ONE_EXIT("room/orc_fortress/wall_n", "east",
     "The northeast tower",
     "This tower overlooks the eastern approach to the fortress.\n" +
     "A crude table holds maps scratched onto animal hides.\n" +
     "This appears to be a command post for the wall garrison.\n" +
     "A battered iron chest sits in the corner.\n", 1)
