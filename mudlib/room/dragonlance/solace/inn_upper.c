/*
 * Inn of the Last Home - Upper floor with Tika Waylan NPC.
 */

inherit "room/room";

object tika;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "Inn upper floor";
	long_desc =
"The upper floor of the Inn of the Last Home. A narrow hallway runs\n" +
"between several small guest rooms. The wooden walls creak gently\n" +
"as the great tree sways in the wind. A window offers a breathtaking\n" +
"view of the forest canopy stretching to the horizon. The smell of\n" +
"spiced potatoes drifts up from below.\n";
	dest_dir = ({
	    "room/dragonlance/solace/inn", "down"
	});
	items = ({
	    "rooms", "Small but comfortable guest rooms with straw mattresses",
	    "window", "A window with a stunning view of the endless forest canopy",
	    "walls", "Curved wooden walls formed by the living tree itself"
	});
    }
    if (!tika || !living(tika)) {
	tika = clone_object("obj/monster");
	tika->set_name("tika");
	tika->set_alias("tika waylan");
	tika->set_alt_name("barmaid");
	tika->set_short("Tika Waylan");
	tika->set_long(
"A pretty young woman with curly red hair and a determined expression.\n" +
"She wears a barmaid's apron and carries a tray of empty mugs. Despite\n" +
"her cheerful demeanor, there is a toughness about her -- she looks\n" +
"like someone who can handle herself in a fight.\n");
	tika->set_level(3);
	tika->set_hp(50);
	tika->set_al(100);
	tika->set_aggressive(0);
	move_object(tika, this_object());
	tika->load_chat(10, ({
	    "Tika says: Otik's spiced potatoes are the best in all of Krynn!\n",
	    "Tika says: Strange times we live in. The old gods are returning.\n",
	    "Tika polishes a mug absentmindedly.\n",
	    "Tika says: If you seek the old gods, visit the temple below.\n"
	}));
    }
}
