/*
 * Shore of Crescent Lake - quest start room for the Fairy Plague quest.
 * Home of the sick fairy NPC.
 */

inherit "room/room";

object fairy;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "The shore of Crescent Lake";
	long_desc =
"You are standing on the shore of Crescent Lake, a beautiful and\n" +
"clear lake. Out in the centre of the lake stands the Isle\n" +
"of the Magi.\n" +
"Among the reeds at the water's edge, a faint, flickering light\n" +
"catches your eye.\n" +
"A trail leads into the forest to the east.\n" +
"The shore of Crescent Lake continues north and south.\n";
	dest_dir = ({
	    "room/south/sshore4", "north",
	    "room/south/sshore6", "south",
	    "room/south/sforst13", "east"
	});
	items = ({
	    "reeds", "Tall reeds grow at the water's edge. Among them, a faint\nflickering light pulses weakly",
	    "lake", "The beautiful, clear waters of Crescent Lake stretch before you.\nThe Isle of the Magi rises from its center",
	    "isle", "The mysterious Isle of the Magi stands in the center of the lake"
	});
    }
    if (!fairy || !living(fairy)) {
	fairy = clone_object("obj/monster");
	fairy->set_name("fairy");
	fairy->set_alias("sick fairy");
	fairy->set_alt_name("forest fairy");
	fairy->set_short("A sickly forest fairy");
	fairy->set_long(
"A tiny forest fairy, barely a hand's span tall, lies among the reeds.\n" +
"Her wings are dull and tattered, and her skin has a sickly grey pallor.\n" +
"She shivers and coughs weakly, clearly suffering from some terrible\n" +
"affliction.\n");
	fairy->set_level(1);
	fairy->set_hp(5);
	fairy->set_al(300);
	fairy->set_aggressive(0);
	move_object(fairy, this_object());
	fairy->set_match(this_object(),
	    ({ "quest_talk" }),
	    ({ "says:" }),
	    ({ 0 })
	);
	fairy->load_chat(3, ({
	    "The sick fairy coughs weakly.\n",
	    "The sick fairy shivers and curls up tighter among the reeds.\n",
	    "The sick fairy's light flickers and nearly goes out.\n"
	}));
    }
    /* Load the quest controller so it exists */
    if (!find_object("obj/fairy_quest"))
	call_other("obj/fairy_quest", "reset", 0);

    /* Register quest object in quest_room if not already there */
    if (find_object("room/quest_room")) {
	object ob;
	int found;
	ob = first_inventory(find_object("room/quest_room"));
	while (ob) {
	    if (ob->id("fairy_plague")) {
		found = 1;
		break;
	    }
	    ob = next_inventory(ob);
	}
	if (!found) {
	    ob = clone_object("obj/fairy_quest_obj");
	    move_object(ob, find_object("room/quest_room"));
	}
    }
}

void quest_talk(string str) {
    object controller;
    object player;
    string who, rest;
    string a, b;

    if (sscanf(str, "%s says: %s\n", who, rest) != 2)
	return;

    player = find_living(lower_case(who));
    if (!player)
	return;

    /* Only respond to quest-related keywords */
    if (sscanf(rest, "%shello%s", a, b) != 2 &&
	sscanf(rest, "%shi%s", a, b) != 2 &&
	sscanf(rest, "%shelp%s", a, b) != 2 &&
	sscanf(rest, "%ssick%s", a, b) != 2 &&
	sscanf(rest, "%splague%s", a, b) != 2 &&
	sscanf(rest, "%sfairy%s", a, b) != 2 &&
	sscanf(rest, "%swrong%s", a, b) != 2 &&
	sscanf(rest, "%smatter%s", a, b) != 2 &&
	sscanf(rest, "%sokay%s", a, b) != 2 &&
	sscanf(rest, "%sdying%s", a, b) != 2)
	return;

    controller = find_object("obj/fairy_quest");
    if (!controller) {
	"obj/fairy_quest"->reset(0);
	controller = find_object("obj/fairy_quest");
    }

    /* Check if player already completed this quest */
    if (player->query_quests("fairy_plague")) {
	tell_object(player,
"The sick fairy says: Thank you again, kind one. The colony heals\n" +
"because of you.\n");
	return;
    }

    /* Check if another player is already doing the quest */
    if (controller->query_stage() > 0 &&
	!controller->is_quest_player(player)) {
	tell_object(player,
"The fairy whispers: Another kind soul is already helping us...\n");
	return;
    }

    /* If this player is already on the quest, give reminders */
    if (controller->is_quest_player(player)) {
	int stage;
	stage = controller->query_stage();
	if (stage == 1)
	    tell_object(player,
"The sick fairy says: Please... find the fairy elder. She dwells in\n" +
"a hidden glade deep in the southern forest. She will know what to do.\n");
	else if (stage == 2)
	    tell_object(player,
"The sick fairy says: Did you find the elder? She needs a moonpetal\n" +
"flower... please hurry...\n");
	else if (stage == 3)
	    tell_object(player,
"The sick fairy says: You found the flower? Bring it to the elder!\n");
	else if (stage >= 4)
	    tell_object(player,
"The sick fairy says: Please... stop the corruption at its source...\n");
	return;
    }

    /* Start the quest */
    if (controller->start_quest(player)) {
	tell_object(player,
"The sick fairy looks up at you with desperate eyes.\n" +
"The sick fairy says: Please... a terrible plague is killing our\n" +
"colony. We forest fairies are dying, one by one. The sickness\n" +
"comes from the lake... something dark beneath the shore.\n\n" +
"The sick fairy says: Our elder... she may know how to stop it.\n" +
"She lives in a hidden glade deep in the southern forest. Please,\n" +
"find her before it is too late for us all.\n");
	say("The sick fairy reaches out a tiny, trembling hand toward " +
	    player->query_name() + ".\n", player);
    }
}
