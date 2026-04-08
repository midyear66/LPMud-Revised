/*
 * Pax Tharkas Armory - Contains the gate key for Quest 3.
 */

inherit "room/room";

object guard;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "The armory";
	long_desc =
"A well-stocked armory filled with weapons and armor for the\n" +
"Dragonarmy garrison. Racks of swords, spears, and crossbows\n" +
"line the walls. Suits of dark plate armor stand on wooden frames.\n" +
"A heavy iron key ring hangs from a peg on the far wall -- the\n" +
"keys to the fortress cells. The courtyard lies to the west.\n";
	dest_dir = ({
	    "room/dragonlance/pax_tharkas/court", "west"
	});
	items = ({
	    "weapons", "Racks of swords, spears, and crossbows",
	    "armor", "Suits of dark plate armor bearing the dragon emblem",
	    "keys", "A heavy iron key ring hanging from a peg -- these look important"
	});
    }
    if (!guard || !living(guard)) {
	guard = clone_object("obj/monster");
	guard->set_name("guard");
	guard->set_alias("armory guard");
	guard->set_alt_name("soldier");
	guard->set_short("An armory guard");
	guard->set_long(
"A burly dragonarmy soldier assigned to guard the armory. He sits\n" +
"on a stool near the key rack, picking his teeth with a dagger.\n");
	guard->set_level(10);
	guard->set_hp(122);
	guard->set_al(-150);
	guard->set_wc(12);
	guard->set_ac(2);
	guard->set_aggressive(1);
	guard->load_a_chat(20, ({
	    "The armory guard shouts: Get away from those weapons!\n",
	    "The guard swings his mace at your head!\n"
	}));
	move_object(guard, this_object());
    }
    /* Spawn the gate key if not present */
    if (!present("key")) {
	object key;
	key = clone_object("obj/treasure");
	key->set_id("key");
	key->set_alias("gate key");
	key->set_short("A heavy iron key");
	key->set_long(
"A heavy iron key on a thick ring. It is stamped with the mark\n" +
"of Pax Tharkas and looks like it would open the fortress's\n" +
"cell doors.\n");
	key->set_value(0);
	key->set_weight(1);
	move_object(key, this_object());
    }
}
