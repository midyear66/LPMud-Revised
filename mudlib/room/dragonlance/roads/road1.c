/*
 * Dragonlance road 1 - Forest road connecting the plains to Solace.
 */

inherit "room/room";

object goblin;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "A forest road";
	long_desc =
"A worn dirt road winds through towering vallenwood trees whose branches\n" +
"form a thick canopy overhead. Dappled sunlight filters through the\n" +
"leaves, casting shifting patterns on the path. The road continues\n" +
"west deeper into the forest, while the open plains lie to the east.\n" +
"A narrower trail branches north toward distant mountains.\n";
	dest_dir = ({
	    "room/plane10", "east",
	    "room/dragonlance/roads/road2", "west",
	    "room/dragonlance/roads/pax_road1", "north"
	});
	items = ({
	    "trees", "Massive vallenwood trees, their trunks wider than a man is tall",
	    "canopy", "A thick canopy of leaves blocks most of the sky",
	    "path", "A well-worn dirt road showing signs of recent travel"
	});
    }
    if (!goblin || !living(goblin)) {
	goblin = clone_object("obj/monster");
	goblin->set_name("goblin");
	goblin->set_alias("goblin scout");
	goblin->set_short("A goblin scout");
	goblin->set_long(
"A scrawny goblin with yellowish skin and beady red eyes. It wears\n" +
"crude leather armor and carries a rusty short sword. It watches the\n" +
"road with shifty eyes, looking for easy prey.\n");
	goblin->set_level(2);
	goblin->set_hp(40);
	goblin->set_al(-50);
	goblin->set_wc(4);
	goblin->set_ac(0);
	goblin->set_aggressive(1);
	goblin->load_a_chat(20, ({
	    "The goblin snarls and slashes wildly!\n",
	    "The goblin shrieks in a high-pitched voice!\n"
	}));
	move_object(goblin, this_object());
    }
}
