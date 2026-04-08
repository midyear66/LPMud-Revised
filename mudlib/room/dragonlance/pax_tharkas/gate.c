/*
 * Pax Tharkas Gate - Fortress entrance, guarded by dragonarmy soldiers.
 */

inherit "room/room";

object soldier1, soldier2;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "Fortress gates";
	long_desc =
"Massive iron-bound gates stand before you, set into walls of dark\n" +
"granite thirty feet thick. Murder holes and arrow slits line the\n" +
"gateway overhead. The gates have been forced open and show signs\n" +
"of recent battle -- the Dragonarmy has taken this ancient dwarven\n" +
"fortress by force. Beyond the gates, a large courtyard is visible.\n" +
"The mountain path leads south.\n";
	dest_dir = ({
	    "room/dragonlance/pax_tharkas/road3", "south",
	    "room/dragonlance/pax_tharkas/court", "north"
	});
	items = ({
	    "gates", "Massive iron-bound gates, forced open by the Dragonarmy",
	    "walls", "Dark granite walls, thirty feet thick",
	    "holes", "Murder holes overhead -- defenders could pour oil or arrows through them",
	    "slits", "Arrow slits staring down from the walls"
	});
    }
    if (!soldier1 || !living(soldier1)) {
	soldier1 = clone_object("obj/monster");
	soldier1->set_name("soldier");
	soldier1->set_alias("dragonarmy soldier");
	soldier1->set_short("A dragonarmy soldier");
	soldier1->set_long(
"A human soldier in dark plate armor bearing the five-headed dragon\n" +
"emblem of the Dragonarmy. He carries a heavy longsword and a\n" +
"battered shield. His eyes are hard and merciless.\n");
	soldier1->set_level(10);
	soldier1->set_hp(122);
	soldier1->set_al(-150);
	soldier1->set_wc(12);
	soldier1->set_ac(2);
	soldier1->set_aggressive(1);
	soldier1->load_a_chat(20, ({
	    "The soldier shouts: Intruder at the gates!\n",
	    "The dragonarmy soldier strikes with practiced efficiency!\n"
	}));
	move_object(soldier1, this_object());
    }
    if (!soldier2 || !living(soldier2)) {
	soldier2 = clone_object("obj/monster");
	soldier2->set_name("soldier");
	soldier2->set_alias("dragonarmy soldier");
	soldier2->set_short("A dragonarmy soldier");
	soldier2->set_long(
"A grim dragonarmy soldier in dark plate armor, standing guard\n" +
"at the fortress gates.\n");
	soldier2->set_level(10);
	soldier2->set_hp(122);
	soldier2->set_al(-150);
	soldier2->set_wc(12);
	soldier2->set_ac(2);
	soldier2->set_aggressive(1);
	move_object(soldier2, this_object());
    }
}
