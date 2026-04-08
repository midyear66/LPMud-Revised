/*
 * Solace Square - Central hub of Solace village, built among the treetops.
 */

inherit "room/room";

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "Solace village square";
	long_desc =
"You stand on a broad wooden platform built among the interlocking\n" +
"branches of several great vallenwood trees. This is the heart of\n" +
"Solace. Rope bridges and walkways branch off in every direction,\n" +
"connecting shops, homes, and the famous Inn of the Last Home.\n" +
"Lanterns hang from the branches, casting a warm golden glow. The\n" +
"sounds of laughter and clinking mugs drift from the inn to the\n" +
"north. You can 'enter temple' to visit a small shrine nearby.\n";
	dest_dir = ({
	    "room/dragonlance/solace/bridge", "south",
	    "room/dragonlance/solace/inn", "north",
	    "room/dragonlance/solace/shop", "west",
	    "room/dragonlance/solace/house", "east",
	    "room/dragonlance/solace/tree1", "southwest"
	});
	items = ({
	    "platform", "A broad wooden platform supported by massive branches",
	    "lanterns", "Oil lanterns hanging from the branches, casting warm light",
	    "inn", "The Inn of the Last Home, built in the branches of a great vallenwood",
	    "temple", "A small temple dedicated to the old gods, to the southeast"
	});
    }
}

void init() {
    ::init();
    add_action("enter_temple", "enter");
}

int enter_temple(string str) {
    if (!str || str != "temple")
	return 0;
    this_player()->move_player("into the temple#room/dragonlance/solace/temple");
    return 1;
}
