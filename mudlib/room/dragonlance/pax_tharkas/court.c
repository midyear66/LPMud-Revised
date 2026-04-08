/*
 * Pax Tharkas Courtyard - Central courtyard of the fortress.
 */

inherit "room/room";

object soldier;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "Fortress courtyard";
	long_desc =
"A vast stone courtyard surrounded by the towering walls of Pax\n" +
"Tharkas. Originally built as a monument to peace between elves\n" +
"and dwarves, the fortress has been corrupted by the Dragonarmy.\n" +
"Weapon racks and training dummies line the walls. Cages holding\n" +
"prisoners are stacked against the east wall. The great hall lies\n" +
"to the north, with barracks to the west and an armory to the\n" +
"east. A heavy iron grate leads down to the prison below.\n";
	dest_dir = ({
	    "room/dragonlance/pax_tharkas/gate", "south",
	    "room/dragonlance/pax_tharkas/hall", "north",
	    "room/dragonlance/pax_tharkas/barracks", "west",
	    "room/dragonlance/pax_tharkas/armory", "east",
	    "room/dragonlance/pax_tharkas/prison", "down"
	});
	items = ({
	    "cages", "Iron cages holding wretched prisoners -- elves and humans",
	    "racks", "Weapon racks lining the courtyard walls",
	    "dummies", "Training dummies scarred by sword and axe practice",
	    "grate", "A heavy iron grate set into the ground, leading to a prison below"
	});
    }
    if (!soldier || !living(soldier)) {
	soldier = clone_object("obj/monster");
	soldier->set_name("soldier");
	soldier->set_alias("dragonarmy soldier");
	soldier->set_short("A dragonarmy soldier");
	soldier->set_long(
"A dragonarmy soldier patrolling the courtyard, keeping watch\n" +
"over the prisoners with a cruel sneer.\n");
	soldier->set_level(10);
	soldier->set_hp(122);
	soldier->set_al(-150);
	soldier->set_wc(12);
	soldier->set_ac(2);
	soldier->set_aggressive(1);
	move_object(soldier, this_object());
    }
}
