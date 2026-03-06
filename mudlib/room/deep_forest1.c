/*
 * Deep forest 1 - home of the wounded traveler for the Forest Remedy quest.
 * Converted from ONE_EXIT macro to inherit style for NPC support.
 */

inherit "room/room";

object traveler;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "Deep forest";
	long_desc =
"In the deep forest. The wood lights up to the east.\n";
	dest_dir = ({ "room/plane12", "east" });
    }
    if (!traveler || !living(traveler)) {
	traveler = clone_object("obj/monster");
	traveler->set_name("traveler");
	traveler->set_alias("wounded traveler");
	traveler->set_short("A wounded traveler");
	traveler->set_long(
"A traveler lies against a tree, clutching a bloody wound. They look\n" +
"like they need help urgently.\n");
	traveler->set_level(1);
	traveler->set_hp(10);
	traveler->set_al(100);
	traveler->set_aggressive(0);
	move_object(traveler, this_object());
	traveler->set_match(this_object(),
	    ({ "quest_talk" }),
	    ({ "says:" }),
	    ({ 0 })
	);
	traveler->load_chat(3, ({
	    "The wounded traveler groans in pain.\n",
	    "The wounded traveler clutches their side and winces.\n",
	    "The wounded traveler whispers: Please... someone help me...\n"
	}));
    }
    /* Load the quest controller so it exists */
    if (!find_object("obj/forest_quest"))
	call_other("obj/forest_quest", "reset", 0);

    /* Register quest object in quest_room if not already there */
    if (find_object("room/quest_room")) {
	object ob;
	int found;
	ob = first_inventory(find_object("room/quest_room"));
	while (ob) {
	    if (ob->id("forest_remedy")) {
		found = 1;
		break;
	    }
	    ob = next_inventory(ob);
	}
	if (!found) {
	    ob = clone_object("obj/forest_quest_obj");
	    move_object(ob, find_object("room/quest_room"));
	}
    }
}

void init() {
    ::init();
    add_action("do_give", "give");
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
	sscanf(rest, "%shelp%s", a, b) != 2 &&
	sscanf(rest, "%swound%s", a, b) != 2 &&
	sscanf(rest, "%shurt%s", a, b) != 2 &&
	sscanf(rest, "%sokay%s", a, b) != 2 &&
	sscanf(rest, "%swrong%s", a, b) != 2 &&
	sscanf(rest, "%shappen%s", a, b) != 2 &&
	sscanf(rest, "%sinjur%s", a, b) != 2 &&
	sscanf(rest, "%sdoctor%s", a, b) != 2 &&
	sscanf(rest, "%smatter%s", a, b) != 2 &&
	sscanf(rest, "%shi%s", a, b) != 2)
	return;

    controller = find_object("obj/forest_quest");
    if (!controller) {
	"obj/forest_quest"->reset(0);
	controller = find_object("obj/forest_quest");
    }

    /* Check if player already completed this quest */
    if (player->query_quests("forest_remedy")) {
	tell_object(player,
"The wounded traveler says: Thank you again for saving my life, friend.\n");
	return;
    }

    /* Check if another player is already doing the quest */
    if (controller->query_stage() > 0 &&
	!controller->is_quest_player(player)) {
	tell_object(player,
"The traveler is already being helped by someone.\n");
	return;
    }

    /* If this player is already on the quest, give reminders */
    if (controller->is_quest_player(player)) {
	int stage;
	stage = controller->query_stage();
	if (stage == 1)
	    tell_object(player,
"The wounded traveler says: Please... find a doctor. There might be one\n" +
"in the village, perhaps down a narrow alley.\n");
	else if (stage == 2)
	    tell_object(player,
"The wounded traveler says: Did you find the doctor? Please hurry...\n");
	else if (stage == 3)
	    tell_object(player,
"The wounded traveler says: Did you find the herb? Bring it to the doctor\n" +
"so he can make the medicine!\n");
	else if (stage == 4)
	    tell_object(player,
"The wounded traveler says: Do you have the medicine? Please, give it to me!\n");
	return;
    }

    /* Start the quest */
    if (controller->start_quest(player)) {
	tell_object(player,
"The wounded traveler says: Please... I was attacked by bandits. I need\n" +
"a doctor. There might be one in the village... down a narrow alley.\n" +
"Please hurry, I don't have much time.\n");
	say("The wounded traveler reaches out weakly toward " +
	    player->query_name() + ".\n", player);
    }
}

int do_give(string str) {
    string item, target;
    object controller;
    object player;
    object potion;

    if (!str)
	return 0;

    if (sscanf(str, "%s to traveler", item) != 1 &&
	sscanf(str, "%s to wounded traveler", item) != 1)
	return 0;

    if (item != "potion" && item != "healing potion" &&
	item != "medicine" && item != "healing medicine")
	return 0;

    player = this_player();
    controller = find_object("obj/forest_quest");

    if (!controller || !controller->is_quest_player(player) ||
	controller->query_stage() != 4) {
	write("The wounded traveler says: I appreciate the thought, but that won't help me.\n");
	return 1;
    }

    potion = present("medicine", player);
    if (!potion)
	potion = present("potion", player);
    if (!potion) {
	write("You don't have the medicine.\n");
	return 1;
    }

    destruct(potion);
    write(
"You give the medicine to the wounded traveler.\n" +
"The traveler drinks the medicine and color returns to their face.\n" +
"The wound begins to close before your eyes.\n" +
"The wounded traveler says: Thank you! You saved my life! I will never\n" +
"forget your kindness.\n");
    say(player->query_name() +
	" gives a healing medicine to the wounded traveler, who recovers miraculously.\n",
	player);

    controller->complete_quest();

    write("You feel more experienced from your good deed.\n");

    /* Traveler gets up and leaves after being healed */
    say("The traveler stands up, stretches, and walks off into the forest.\n");
    write("The traveler stands up, stretches, and walks off into the forest.\n");
    destruct(traveler);
    traveler = 0;

    return 1;
}
