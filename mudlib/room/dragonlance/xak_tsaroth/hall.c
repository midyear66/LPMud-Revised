/*
 * Xak Tsaroth Hall - Collapsed great hall with gully dwarf.
 */

inherit "room/room";

object gully;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "A collapsed great hall";
	long_desc =
"This was once a grand hall of assembly, but the roof has caved in\n" +
"and rubble fills most of the interior. A few pillars still stand,\n" +
"holding up nothing. Rats scurry among the debris, and a foul smell\n" +
"rises from somewhere below. A grubby-looking dwarf sits in a\n" +
"corner, gnawing on something unidentifiable. The plaza lies west.\n";
	dest_dir = ({
	    "room/dragonlance/xak_tsaroth/plaza", "west"
	});
	items = ({
	    "pillars", "A few stone pillars still standing, holding up nothing",
	    "rubble", "Heaps of broken stone and rotting timber",
	    "rats", "Fat rats scurrying among the debris"
	});
    }
    if (!gully || !living(gully)) {
	gully = clone_object("obj/monster");
	gully->set_name("bupu");
	gully->set_alias("gully dwarf");
	gully->set_alt_name("dwarf");
	gully->set_short("Bupu the gully dwarf");
	gully->set_long(
"A small, filthy dwarf with matted hair, a squashed nose, and\n" +
"clothes that appear to be stitched together from rags and dead\n" +
"lizard skins. Despite her bedraggled appearance, she has bright,\n" +
"curious eyes. She clutches a dead rat like a treasured possession.\n");
	gully->set_level(3);
	gully->set_hp(50);
	gully->set_al(0);
	gully->set_aggressive(0);
	move_object(gully, this_object());
	gully->load_chat(15, ({
	    "Bupu says: Me know two things. One and...one. That two!\n",
	    "Bupu gnaws on something unidentifiable.\n",
	    "Bupu says: Big lizard-mens scary. They live down in well.\n",
	    "Bupu says: Pretty lady dragon live way down below. Very mean.\n",
	    "Bupu hugs her dead rat protectively.\n"
	}));
    }
}
