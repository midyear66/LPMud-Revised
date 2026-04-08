/*
 * Xak Tsaroth Gate - Ruined city gates.
 */

inherit "room/room";

object draconian;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "Ruined city gates";
	long_desc =
"Massive stone gates, cracked and leaning, mark the entrance to the\n" +
"ruined city of Xak Tsaroth. One gate has collapsed entirely, while\n" +
"the other hangs at a precarious angle. Beyond them, a devastated\n" +
"plaza opens up, surrounded by the shells of once-grand buildings.\n" +
"The destruction here is ancient but thorough -- this city was\n" +
"destroyed by dragonfire long ago.\n";
	dest_dir = ({
	    "room/dragonlance/xak_tsaroth/road2", "north",
	    "room/dragonlance/xak_tsaroth/plaza", "south"
	});
	items = ({
	    "gates", "Massive stone gates, cracked and leaning dangerously",
	    "buildings", "Hollow shells of once-grand buildings, gutted by fire",
	    "plaza", "A devastated plaza beyond the gates, choked with rubble"
	});
    }
    if (!draconian || !living(draconian)) {
	draconian = clone_object("obj/monster");
	draconian->set_name("draconian");
	draconian->set_alias("draconian soldier");
	draconian->set_short("A draconian soldier");
	draconian->set_long(
"A scaly draconian guard with dull bronze scales and a wicked\n" +
"halberd. It stands watch at the ruined gates with reptilian\n" +
"alertness.\n");
	draconian->set_level(6);
	draconian->set_hp(88);
	draconian->set_al(-200);
	draconian->set_wc(8);
	draconian->set_ac(1);
	draconian->set_aggressive(1);
	draconian->load_a_chat(20, ({
	    "The draconian guard snarls: No one passes!\n",
	    "The draconian swings its halberd in a wide arc!\n"
	}));
	move_object(draconian, this_object());
    }
}
