/*
 * Xak Tsaroth Well - Ancient well with rope descent for Quest 2.
 */

inherit "room/room";

int rope_tied;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "An ancient well";
	long_desc =
"A large stone well stands in a small clearing among the ruins.\n" +
"The well is ancient, its rim worn smooth by centuries of use.\n" +
"Peering over the edge, you see only impenetrable darkness far\n" +
"below. A cold draft rises from the depths, carrying the smell\n" +
"of damp stone and something reptilian. The plaza lies to the\n" +
"north.\n";
	dest_dir = ({
	    "room/dragonlance/xak_tsaroth/plaza", "north"
	});
	items = ({
	    "well", "A deep stone well. The darkness below seems bottomless",
	    "rim", "The stone rim is worn smooth by centuries of hands drawing water",
	    "darkness", "Impenetrable darkness far below -- you cannot see the bottom"
	});
    }
    rope_tied = 0;
    /* Load the quest controller */
    if (!find_object("obj/dl_disks_quest"))
	call_other("obj/dl_disks_quest", "reset", 0);
}

void init() {
    ::init();
    add_action("use_rope", "use");
    add_action("use_rope", "tie");
    add_action("descend_well", "descend");
    add_action("descend_well", "climb");
}

int use_rope(string str) {
    object controller;
    object player;
    object rope;
    string a, b;

    if (!str)
	return 0;

    if (sscanf(str, "%srope%s", a, b) != 2)
	return 0;

    player = this_player();
    rope = present("rope", player);

    if (!rope) {
	write("You don't have a rope.\n");
	return 1;
    }

    controller = find_object("obj/dl_disks_quest");
    if (!controller || !controller->is_quest_player(player)) {
	write("You tie the rope to the well rim and peer into the darkness.\n" +
	      "On second thought, there is no reason to go down there.\n");
	return 1;
    }

    if (controller->query_stage() < 1) {
	write("Nothing happens.\n");
	return 1;
    }

    /* Tie rope and descend */
    destruct(rope);
    rope_tied = 1;
    controller->set_stage(2);

    write(
"You tie the rope securely to the well rim and lower yourself into\n" +
"the darkness. Hand over hand, you descend into the depths of the\n" +
"earth. The light from above dwindles to a tiny point...\n");
    say(player->query_name() +
	" ties a rope to the well and descends into the darkness.\n", player);

    player->move_player("down into the well#room/dragonlance/xak_tsaroth/under1");
    return 1;
}

int descend_well(string str) {
    object player;

    if (!str)
	str = "";

    if (str != "" && str != "well" && str != "rope" && str != "down")
	return 0;

    player = this_player();

    if (!rope_tied) {
	write("The well is too deep to climb without a rope.\n");
	return 1;
    }

    write("You climb down the rope into the darkness below.\n");
    say(player->query_name() + " climbs down the rope into the well.\n", player);
    player->move_player("down into the well#room/dragonlance/xak_tsaroth/under1");
    return 1;
}
