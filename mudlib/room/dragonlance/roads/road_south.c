/*
 * Dragonlance road south - Road between Solace and Xak Tsaroth.
 */

inherit "room/room";

object hobgoblin;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "A desolate road";
	long_desc =
"The road deteriorates as it winds south through increasingly barren\n" +
"terrain. The healthy vallenwoods give way to twisted, blackened trees.\n" +
"A foul smell hangs in the air, and the ground is scarred as if by\n" +
"ancient fire. The cheerful lights of Solace are visible to the north,\n" +
"while the road continues south toward ruins glimpsed through the haze.\n";
	dest_dir = ({
	    "room/dragonlance/roads/road2", "north",
	    "room/dragonlance/xak_tsaroth/road1", "south"
	});
	items = ({
	    "trees", "Twisted, blackened trees -- something terrible happened here long ago",
	    "ground", "The earth is scarred and scorched, as if by dragonfire",
	    "ruins", "Crumbling stone structures are barely visible through the haze to the south"
	});
    }
    if (!hobgoblin || !living(hobgoblin)) {
	hobgoblin = clone_object("obj/monster");
	hobgoblin->set_name("hobgoblin");
	hobgoblin->set_alias("hobgoblin raider");
	hobgoblin->set_short("A hobgoblin raider");
	hobgoblin->set_long(
"A hulking hobgoblin standing over six feet tall, with dark orange\n" +
"skin and a muscular build. It wears a crude breastplate marked with\n" +
"the emblem of a dragon and carries a heavy mace.\n");
	hobgoblin->set_level(4);
	hobgoblin->set_hp(60);
	hobgoblin->set_al(-100);
	hobgoblin->set_wc(6);
	hobgoblin->set_ac(1);
	hobgoblin->set_aggressive(1);
	hobgoblin->load_a_chat(20, ({
	    "The hobgoblin swings its mace in a wide arc!\n",
	    "The hobgoblin bellows: For the Dragon Highlords!\n"
	}));
	move_object(hobgoblin, this_object());
    }
}
