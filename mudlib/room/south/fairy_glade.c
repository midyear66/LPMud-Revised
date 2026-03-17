/*
 * Fairy Glade - home of the fairy elder for the Fairy Plague quest.
 * Accessible from sforst35 (northeast).
 */

inherit "room/room";

object elder;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "A hidden fairy glade";
	long_desc =
"You stand in a small, enchanted glade hidden deep within the forest.\n" +
"Soft light filters through the canopy, illuminating a ring of pale\n" +
"mushrooms on the mossy ground. Tiny motes of light drift through the\n" +
"air like lazy fireflies, and the faint scent of flowers fills the\n" +
"clearing. The trees here seem ancient and watchful.\n" +
"A trail leads southwest back into the forest.\n";
	dest_dir = ({ "room/south/sforst35", "southwest" });
	items = ({
	    "mushrooms", "A ring of pale mushrooms forms a perfect circle on the\nmossy ground. Some call these fairy rings",
	    "mushroom ring", "A ring of pale mushrooms forms a perfect circle on the\nmossy ground. Some call these fairy rings",
	    "motes", "Tiny motes of golden light drift lazily through the air,\nleaving faint trails behind them",
	    "light", "Tiny motes of golden light drift lazily through the air,\nleaving faint trails behind them",
	    "trees", "The ancient trees lean inward protectively, their gnarled\nbranches forming a living canopy over the glade"
	});
    }
    if (!elder || !living(elder)) {
	elder = clone_object("obj/monster");
	elder->set_name("elder");
	elder->set_alias("fairy elder");
	elder->set_alt_name("fairy");
	elder->set_short("The fairy elder");
	elder->set_long(
"A tall, slender fairy with wings like stained glass and hair like\n" +
"spun silver. Despite an air of great age, her eyes burn with fierce\n" +
"determination. A faint golden aura surrounds her, though it flickers\n" +
"and dims -- the plague is taking its toll even on her.\n");
	elder->set_level(3);
	elder->set_al(300);
	elder->set_aggressive(0);
	move_object(elder, this_object());
	elder->set_match(this_object(),
	    ({ "quest_talk" }),
	    ({ "says:" }),
	    ({ 0 })
	);
	elder->load_chat(3, ({
	    "The fairy elder gazes sadly at the wilting flowers around the glade.\n",
	    "The fairy elder whispers an incantation, and the motes of light brighten briefly.\n",
	    "The fairy elder says: The forest grows darker each day...\n"
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
	sscanf(rest, "%shi%s", a, b) != 2 &&
	sscanf(rest, "%splague%s", a, b) != 2 &&
	sscanf(rest, "%ssick%s", a, b) != 2 &&
	sscanf(rest, "%shelp%s", a, b) != 2 &&
	sscanf(rest, "%sfairy%s", a, b) != 2 &&
	sscanf(rest, "%sfairies%s", a, b) != 2 &&
	sscanf(rest, "%scure%s", a, b) != 2 &&
	sscanf(rest, "%smoonpetal%s", a, b) != 2 &&
	sscanf(rest, "%sflower%s", a, b) != 2 &&
	sscanf(rest, "%sblight%s", a, b) != 2 &&
	sscanf(rest, "%ssprite%s", a, b) != 2 &&
	sscanf(rest, "%sgrotto%s", a, b) != 2)
	return;

    controller = find_object("obj/fairy_quest");
    if (!controller) {
	"obj/fairy_quest"->reset(0);
	controller = find_object("obj/fairy_quest");
    }

    /* Check if player already completed this quest */
    if (player->query_quests("fairy_plague")) {
	tell_object(player,
"The fairy elder says: Blessings upon you, hero. The colony thrives\n" +
"thanks to your courage.\n");
	return;
    }

    /* Check if another player is already doing the quest */
    if (controller->query_stage() > 0 &&
	!controller->is_quest_player(player)) {
	tell_object(player,
"The fairy elder says: Another brave soul is already aiding us.\n" +
"Please be patient.\n");
	return;
    }

    /* Not on quest yet -- elder won't start it, sick fairy does */
    if (!controller->is_quest_player(player)) {
	tell_object(player,
"The fairy elder says: Greetings, traveler. These are dark times for\n" +
"our kind. If you wish to help, seek the shores of Crescent Lake.\n" +
"You may find one of our kin in need there.\n");
	return;
    }

    /* Player is on the quest -- respond based on stage */
    if (controller->query_stage() == 1) {
	/* First visit to the elder */
	tell_object(player,
"The fairy elder says: So you have met one of our sick kin. The plague\n" +
"that ravages us comes from beneath the lakeshore, where a once-benign\n" +
"water sprite has been corrupted by a cursed runestone.\n" +
"The sprite's tainted magic seeps into the forest and poisons us.\n\n" +
"The fairy elder says: To cleanse the corruption, I can brew a\n" +
"purifying draught, but I need a rare moonpetal flower. They grow\n" +
"somewhere in the southern forest, glowing with pale light.\n" +
"Find one and bring it to me.\n");
	say("The fairy elder's wings droop with exhaustion as she speaks to " +
	    player->query_name() + ".\n", player);
	controller->set_stage(2);
	controller->spawn_moonpetal();
	return;
    }

    if (controller->query_stage() == 2) {
	tell_object(player,
"The fairy elder says: Have you found the moonpetal flower? Search the\n" +
"southern forest carefully. It glows with a pale light.\n");
	return;
    }

    if (controller->query_stage() == 3) {
	tell_object(player,
"The fairy elder says: You have the moonpetal! Please, give it to me\n" +
"so I can prepare the purifying draught.\n");
	return;
    }

    if (controller->query_stage() == 4) {
	tell_object(player,
"The fairy elder says: You have the draught. Go to the shore of\n" +
"Crescent Lake and find the cave that leads to the grotto beneath.\n" +
"Pour the draught upon the cursed runestone to break its hold.\n" +
"But beware -- the corrupted sprite will not yield easily.\n");
	return;
    }

    if (controller->query_stage() == 5) {
	/* Check if sprite is already dead (fallback if callback missed) */
	object grotto;
	grotto = find_object("room/south/grotto");
	if (!grotto || !present("sprite", grotto)) {
	    /* Sprite is gone, advance to stage 6 */
	    controller->set_stage(6);
	} else {
	    tell_object(player,
"The fairy elder says: The corruption still festers! You must defeat\n" +
"the corrupted sprite in the grotto!\n");
	    return;
	}
    }

    if (controller->query_stage() == 6) {
	object amulet;
	/* Quest complete! */
	tell_object(player,
"The fairy elder's eyes fill with tears of joy.\n" +
"The fairy elder says: You have done it! The corruption is cleansed!\n" +
"Already I can feel the forest healing, the blight receding.\n" +
"The fairy colony will survive, thanks to your bravery.\n\n" +
"The fairy elder says: Accept this gift, a token of our eternal\n" +
"gratitude. May it protect you on your journeys.\n");
	say("The fairy elder embraces " + player->query_name() +
	    " and the glade fills with golden light.\n", player);
	/* Give reward directly (avoid this_player context issues) */
	amulet = clone_object("obj/armour");
	amulet->set_id("amulet");
	amulet->set_alias("fairy amulet");
	amulet->set_short("A fairy amulet");
	amulet->set_long(
"A delicate amulet woven from silver threads and tiny moonpetals.\n" +
"It shimmers with a faint protective glow, a gift from the fairy\n" +
"elder in gratitude for saving the colony from the plague.\n");
	amulet->set_value(200);
	amulet->set_weight(1);
	amulet->set_type("amulet");
	amulet->set_ac(1);
	move_object(amulet, player);
	tell_object(player,
	    "The fairy elder places a shimmering amulet around your neck.\n");
	player->add_exp(1000);
	player->set_quest("fairy_plague");
	controller->set_stage(0);
	tell_object(player,
	    "You feel much more experienced from your heroic deed.\n");
	return;
    }
}

int do_give(string str) {
    string item, target;
    object controller;
    object player;
    object flower;
    object draught;

    if (!str)
	return 0;

    if (sscanf(str, "%s to elder", item) != 1 &&
	sscanf(str, "%s to fairy elder", item) != 1 &&
	sscanf(str, "%s to fairy", item) != 1)
	return 0;

    if (item != "moonpetal" && item != "flower" &&
	item != "moonpetal flower")
	return 0;

    player = this_player();
    controller = find_object("obj/fairy_quest");

    if (!controller || !controller->is_quest_player(player) ||
	controller->query_stage() != 3) {
	write("The fairy elder says: I appreciate the thought, but that is not what I need right now.\n");
	return 1;
    }

    flower = present("moonpetal", player);
    if (!flower)
	flower = present("flower", player);
    if (!flower) {
	write("You don't have a moonpetal flower.\n");
	return 1;
    }

    destruct(flower);

    write(
"You give the moonpetal flower to the fairy elder.\n" +
"She cradles it gently and begins to chant in a strange, melodic tongue.\n" +
"The flower dissolves into swirling light that she catches in a crystal\n" +
"vial. The liquid inside glows with an intense, purifying radiance.\n\n" +
"The fairy elder says: The purifying draught is ready. Take it to the\n" +
"grotto beneath the lakeshore. You will find a cave entrance along\n" +
"the shore of Crescent Lake. Pour the draught upon the cursed\n" +
"runestone to break its power. But be warned -- the corrupted sprite\n" +
"will fight to protect it.\n");
    say(player->query_name() +
	" gives a moonpetal flower to the fairy elder, who creates a " +
	"glowing draught.\n", player);

    draught = clone_object("obj/purifying_draught");
    move_object(draught, player);
    controller->set_stage(4);

    return 1;
}
