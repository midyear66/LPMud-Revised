/*
 * Dragonlance road 2 - Forest road approaching Solace.
 */

inherit "room/room";

object goblin;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "A forest road near Solace";
	long_desc =
"The road widens as it approaches a remarkable sight -- an entire\n" +
"village built high among the branches of enormous vallenwood trees.\n" +
"Rope bridges and wooden walkways connect the tree-top buildings.\n" +
"Warm light glows from windows far above. The village gate lies\n" +
"to the west, while the road continues east. A muddy track leads\n" +
"south toward darker, more desolate lands.\n";
	dest_dir = ({
	    "room/dragonlance/roads/road1", "east",
	    "room/dragonlance/solace/gate", "west",
	    "room/dragonlance/roads/road_south", "south"
	});
	items = ({
	    "village", "A village built among the treetops -- this must be Solace",
	    "bridges", "Rope bridges sway gently between the massive trees",
	    "windows", "Warm light spills from windows high above"
	});
    }
    if (!goblin || !living(goblin)) {
	goblin = clone_object("obj/monster");
	goblin->set_name("goblin");
	goblin->set_alias("goblin scout");
	goblin->set_short("A goblin scout");
	goblin->set_long(
"A wiry goblin with mottled green skin and a mean look in its eyes.\n" +
"It carries a crude spear and wears a necklace of animal teeth.\n");
	goblin->set_level(2);
	goblin->set_hp(40);
	goblin->set_al(-50);
	goblin->set_wc(4);
	goblin->set_ac(0);
	goblin->set_aggressive(1);
	goblin->load_a_chat(20, ({
	    "The goblin jabs with its spear!\n",
	    "The goblin hisses through pointed teeth!\n"
	}));
	move_object(goblin, this_object());
    }
}
