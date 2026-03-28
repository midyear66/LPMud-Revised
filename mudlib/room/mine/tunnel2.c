/*
 * Mine tunnel - Grimjaw the old miner sits here.
 * Quest giver for the Lost Miners quest.
 */

inherit "room/room";

object grimjaw;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "Tunnel";
	long_desc =
"In the tunnel into the mines. An old lantern hangs from a beam,\n" +
"casting a flickering light. The tunnel continues north into\n" +
"darkness and south toward the mine entrance.\n";
	dest_dir = ({
	    "room/mine/tunnel", "south",
	    "room/mine/tunnel3", "north"
	});
    }
    if (!grimjaw || !living(grimjaw)) {
	grimjaw = clone_object("obj/monster");
	grimjaw->set_name("grimjaw");
	grimjaw->set_alias("miner");
	grimjaw->set_alt_name("old miner");
	grimjaw->set_short("Grimjaw the old miner");
	grimjaw->set_long(
"A grizzled old miner with a scarred face and calloused hands.\n" +
"His eyes are red-rimmed from grief and lack of sleep. He sits\n" +
"slumped against the tunnel wall, staring into the darkness.\n");
	grimjaw->set_level(8);
	grimjaw->set_hp(80);
	grimjaw->set_al(100);
	grimjaw->set_aggressive(0);
	move_object(grimjaw, this_object());
	grimjaw->set_match(this_object(),
	    ({ "quest_talk" }),
	    ({ "says:" }),
	    ({ 0 })
	);
	grimjaw->load_chat(5, ({
	    "Grimjaw stares into the darkness and sighs heavily.\n",
	    "Grimjaw mutters: They're still down there... I know it.\n",
	    "Grimjaw turns a battered mining helmet over in his hands.\n"
	}));
    }
    /* Load the quest controller so it exists */
    if (!find_object("obj/mine_quest"))
	call_other("obj/mine_quest", "reset", 0);

    /* Register quest object in quest_room if not already there */
    if (find_object("room/quest_room")) {
	object ob;
	int found;
	ob = first_inventory(find_object("room/quest_room"));
	while (ob) {
	    if (ob->id("lost_miners")) {
		found = 1;
		break;
	    }
	    ob = next_inventory(ob);
	}
	if (!found) {
	    ob = clone_object("obj/mine_quest_obj");
	    move_object(ob, find_object("room/quest_room"));
	}
    }
}

void give_quest_reward(object player) {
    if (!player)
	return;
    player->add_exp(2000);
    player->set_quest("lost_miners");
    tell_object(player,
"You feel more experienced from your brave deed.\n");
}

void quest_talk(string str) {
    object controller;
    object player;
    object token;
    string who, rest;
    string a, b;

    if (sscanf(str, "%s says: %s\n", who, rest) != 2)
	return;

    player = find_living(lower_case(who));
    if (!player || !interactive(player))
	return;

    /* Only respond to quest-related keywords */
    if (sscanf(rest, "%shello%s", a, b) != 2 &&
	sscanf(rest, "%shi%s", a, b) != 2 &&
	sscanf(rest, "%shelp%s", a, b) != 2 &&
	sscanf(rest, "%sminer%s", a, b) != 2 &&
	sscanf(rest, "%slost%s", a, b) != 2 &&
	sscanf(rest, "%scave%s", a, b) != 2 &&
	sscanf(rest, "%screw%s", a, b) != 2 &&
	sscanf(rest, "%stoken%s", a, b) != 2 &&
	sscanf(rest, "%squest%s", a, b) != 2)
	return;

    controller = find_object("obj/mine_quest");
    if (!controller) {
	"obj/mine_quest"->reset(0);
	controller = find_object("obj/mine_quest");
    }

    /* Check if player already completed this quest */
    if (player->query_quests("lost_miners")) {
	tell_object(player,
"Grimjaw says: Bless you, friend. Thanks to you, my crew is safe.\n");
	return;
    }

    /* Check if another player is already doing the quest */
    if (controller->query_stage() > 0 &&
	!controller->is_quest_player(player)) {
	tell_object(player,
"Grimjaw says: Someone else is already looking for my crew.\n" +
"Perhaps check back later.\n");
	return;
    }

    /* If this player is already on the quest, check for token */
    if (controller->is_quest_player(player)) {
	token = present("token", player);
	if (token) {
	    /* Player has the mining token - complete the quest */
	    object lamp;
	    lamp = clone_object("obj/treasure");
	    lamp->set_id("lamp");
	    lamp->set_alias("miner's lamp");
	    lamp->set_short("A miner's lamp");
	    lamp->set_long(
"A sturdy brass lamp that burns with a steady flame. It was\n" +
"a gift from Grimjaw for rescuing his crew.\n");
	    lamp->set_value(100);
	    lamp->set_weight(1);
	    move_object(lamp, player);
	    destruct(token);
	    controller->set_stage(0);
	    tell_object(player,
"Grimjaw says: The mining token! You found them! They're alive!\n" +
"I cannot thank you enough. Take this lamp -- it has served me\n" +
"well for thirty years in these tunnels. May it light your way.\n" +
"Grimjaw presses a brass lamp into your hands, tears of relief\n" +
"streaming down his weathered face.\n");
	    say("Grimjaw leaps to his feet and embraces " +
		player->query_name() + " with a crushing hug.\n", player);
	    give_quest_reward(player);
	    return;
	}
	/* Player is on quest but does not have the token yet */
	tell_object(player,
"Grimjaw says: Please, you must hurry! My crew is trapped deep\n" +
"below. Head north and down through the tunnels. Use the rope\n" +
"to help them escape. They will give you a token so I know\n" +
"they are safe.\n");
	return;
    }

    /* Start the quest */
    if (controller->start_quest(player)) {
	object rope;
	rope = clone_object("obj/treasure");
	rope->set_id("rope");
	rope->set_alias("coil of rope");
	rope->set_short("A coil of rope");
	rope->set_long(
"A sturdy coil of hemp rope, about thirty feet long.\n");
	rope->set_value(0);
	rope->set_weight(2);
	move_object(rope, player);
	tell_object(player,
"Grimjaw says: Thank the heavens someone has come! There was a\n" +
"cave-in deep in the tunnels. My crew -- three good miners --\n" +
"they're trapped down there. I can hear them tapping on the\n" +
"rocks but I'm too old and broken to dig them out.\n" +
"Grimjaw says: Take this rope. Head deep into the tunnels, all\n" +
"the way down to the lowest levels. Find my crew and use the\n" +
"rope to pull them free. They will give you a mining token so\n" +
"I know they are safe. Please, hurry!\n" +
"Grimjaw hands you a coil of rope.\n");
	say("Grimjaw hands a coil of rope to " +
	    player->query_name() + " with trembling hands.\n", player);
    }
}
