/*
 * Pax Tharkas Barracks - Dragonarmy captain guards here.
 */

inherit "room/room";

object captain;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "Dragonarmy barracks";
	long_desc =
"A large stone hall converted to barracks for the Dragonarmy\n" +
"garrison. Rows of crude bunks line the walls, and the air reeks\n" +
"of sweat and weapon oil. Maps and battle plans are pinned to a\n" +
"large board on one wall. A weapons rack holds spare swords and\n" +
"shields. The courtyard is to the east.\n";
	dest_dir = ({
	    "room/dragonlance/pax_tharkas/court", "east"
	});
	items = ({
	    "bunks", "Rows of crude wooden bunks with threadbare blankets",
	    "maps", "Battle maps showing troop movements across Ansalon",
	    "plans", "Plans for the Dragonarmy's conquest of the region",
	    "rack", "A weapons rack with spare swords and shields"
	});
    }
    if (!captain || !living(captain)) {
	captain = clone_object("obj/monster");
	captain->set_name("captain");
	captain->set_alias("dragonarmy captain");
	captain->set_short("A dragonarmy captain");
	captain->set_long(
"A battle-scarred human officer in blackened plate armor decorated\n" +
"with dragon motifs. He wears a crimson cloak and carries a\n" +
"masterwork longsword. His face bears a jagged scar from temple\n" +
"to jaw, and his eyes are cold with casual cruelty.\n");
	captain->set_level(12);
	captain->set_hp(138);
	captain->set_al(-200);
	captain->set_wc(14);
	captain->set_ac(3);
	captain->set_aggressive(1);
	captain->load_a_chat(20, ({
	    "The captain bellows: Sound the alarm! We have an intruder!\n",
	    "The captain strikes with deadly precision!\n",
	    "The captain says: Lord Verminaard will have your head!\n"
	}));
	move_object(captain, this_object());
    }
}
