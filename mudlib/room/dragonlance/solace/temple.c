/*
 * Temple of Mishakal in Solace - Quest 1 giver room.
 * Goldmoon gives "The Blue Crystal Staff" quest.
 */

inherit "room/room";

object goldmoon;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "Temple of Mishakal";
	long_desc =
"A small but beautiful temple carved into the hollow of a great\n" +
"vallenwood tree. The walls are smooth living wood, polished to a\n" +
"warm sheen. A simple stone altar stands at the far end, adorned\n" +
"with fresh wildflowers. Soft light filters through crystal windows\n" +
"set into the bark. The air carries a faint scent of incense and\n" +
"the feeling of ancient peace. An archway leads out to the village\n" +
"square.\n";
	dest_dir = ({
	    "room/dragonlance/solace/square", "out"
	});
	items = ({
	    "altar", "A simple stone altar with fresh wildflowers placed upon it",
	    "windows", "Crystal windows set into the bark, filtering soft light",
	    "walls", "Smooth living wood, polished to a warm golden sheen",
	    "flowers", "Fresh wildflowers -- someone tends this altar with care"
	});
    }
    if (!goldmoon || !living(goldmoon)) {
	goldmoon = clone_object("obj/monster");
	goldmoon->set_name("goldmoon");
	goldmoon->set_alias("goldmoon");
	goldmoon->set_alt_name("priestess");
	goldmoon->set_short("Goldmoon, Priestess of Mishakal");
	goldmoon->set_long(
"A tall, strikingly beautiful woman with silver-gold hair that falls\n" +
"past her waist. She wears flowing white robes and carries herself\n" +
"with the quiet dignity of a barbarian chieftain's daughter. Her\n" +
"blue eyes shine with a serene inner light -- the mark of a true\n" +
"cleric of Mishakal, goddess of healing.\n");
	goldmoon->set_level(5);
	goldmoon->set_hp(82);
	goldmoon->set_al(300);
	goldmoon->set_aggressive(0);
	move_object(goldmoon, this_object());
	goldmoon->set_match(this_object(),
	    ({ "quest_talk" }),
	    ({ "says:" }),
	    ({ 0 })
	);
	goldmoon->load_chat(5, ({
	    "Goldmoon prays softly before the altar.\n",
	    "Goldmoon says: The old gods have not abandoned us.\n",
	    "A warm light briefly surrounds Goldmoon.\n"
	}));
    }
    /* Load the quest controller */
    if (!find_object("obj/dl_staff_quest"))
	call_other("obj/dl_staff_quest", "reset", 0);

    /* Register quest in quest_room */
    if (find_object("room/quest_room")) {
	object ob;
	int found;
	ob = first_inventory(find_object("room/quest_room"));
	while (ob) {
	    if (ob->id("blue_crystal_staff")) {
		found = 1;
		break;
	    }
	    ob = next_inventory(ob);
	}
	if (!found) {
	    ob = clone_object("obj/dl_staff_quest_obj");
	    move_object(ob, find_object("room/quest_room"));
	}
    }
}

void give_quest_reward(object player) {
    object staff;

    if (!player)
	return;
    player->add_exp(600);
    player->set_quest("blue_crystal_staff");

    staff = clone_object("obj/weapon");
    staff->set_name("staff");
    staff->set_alias("staff of healing");
    staff->set_short("The Staff of Healing");
    staff->set_long(
"A staff of gleaming blue crystal, warm to the touch. It hums\n" +
"softly with divine energy, a gift from Mishakal herself. The\n" +
"crystal radiates a soothing light that eases pain and lifts\n" +
"the spirit.\n");
    staff->set_class(5);
    staff->set_value(200);
    staff->set_weight(3);
    move_object(staff, player);

    tell_object(player, "You feel more experienced from your brave deed.\n");
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
	sscanf(rest, "%scrystal%s", a, b) != 2 &&
	sscanf(rest, "%sstaff%s", a, b) != 2 &&
	sscanf(rest, "%stree%s", a, b) != 2 &&
	sscanf(rest, "%scorrupt%s", a, b) != 2 &&
	sscanf(rest, "%squest%s", a, b) != 2 &&
	sscanf(rest, "%sfragment%s", a, b) != 2 &&
	sscanf(rest, "%shi%s", a, b) != 2)
	return;

    controller = find_object("obj/dl_staff_quest");
    if (!controller) {
	"obj/dl_staff_quest"->reset(0);
	controller = find_object("obj/dl_staff_quest");
    }

    /* Already completed */
    if (player->query_quests("blue_crystal_staff")) {
	tell_object(player,
"Goldmoon says: Bless you, friend. Thanks to your courage, the\n" +
"Blue Crystal Staff is being restored. Mishakal smiles upon you.\n");
	return;
    }

    /* Another player is doing the quest */
    if (controller->query_stage() > 0 &&
	!controller->is_quest_player(player)) {
	tell_object(player,
"Goldmoon says: Another brave soul is already seeking the crystal\n" +
"fragment. Please check back later.\n");
	return;
    }

    /* Player is already on the quest - give stage-appropriate response */
    if (controller->is_quest_player(player)) {
	int stage;
	stage = controller->query_stage();
	if (stage == 1) {
	    tell_object(player,
"Goldmoon says: Take the prayer token to the corrupted tree on the\n" +
"western walkway. Use it there to draw out the darkness.\n");
	    return;
	}
	if (stage == 2) {
	    tell_object(player,
"Goldmoon says: The corrupted treant has awakened! You must defeat\n" +
"it to recover the crystal fragment!\n");
	    return;
	}
	if (stage == 3) {
	    /* Check if player has the crystal fragment */
	    if (present("crystal", player)) {
		object frag;
		frag = present("crystal", player);
		if (frag) destruct(frag);
		controller->set_stage(0);
		tell_object(player,
"Goldmoon says: You found it! The crystal fragment!\n" +
"Goldmoon takes the fragment and holds it before the altar. It\n" +
"blazes with brilliant blue light, and for a moment the entire\n" +
"temple glows with healing radiance.\n" +
"Goldmoon says: Mishakal be praised! Take this staff as a token\n" +
"of her gratitude. You have done a great service to all of Krynn.\n" +
"Goldmoon places a staff of gleaming blue crystal in your hands.\n");
		say("Goldmoon takes a crystal fragment from " +
		    player->query_name() + " and the temple blazes with light!\n" +
		    "Goldmoon presents a staff of healing to " +
		    player->query_name() + ".\n", player);
		give_quest_reward(player);
		return;
	    }
	    tell_object(player,
"Goldmoon says: The treant has fallen! Seek the crystal fragment\n" +
"where the creature was destroyed, and bring it back to me.\n");
	    return;
	}
	return;
    }

    /* Start the quest */
    if (controller->start_quest(player)) {
	object token;
	token = clone_object("obj/treasure");
	token->set_id("prayer_token");
	token->set_alias("token");
	token->set_short("A prayer token of Mishakal");
	token->set_long(
"A small silver medallion inscribed with the symbol of Mishakal --\n" +
"an infinity sign. It is warm to the touch and glows faintly with\n" +
"divine light.\n");
	token->set_value(0);
	token->set_weight(1);
	move_object(token, player);
	tell_object(player,
"Goldmoon says: Thank the gods you have come! A dark corruption\n" +
"has taken root in one of our ancient vallenwoods. A fragment of\n" +
"the legendary Blue Crystal Staff was hidden within that tree for\n" +
"safekeeping, but the corruption has twisted it into something\n" +
"terrible -- a treant possessed by dark magic.\n" +
"Take this prayer token. Go to the corrupted tree on the western\n" +
"walkway and use it to draw out the darkness. Defeat the corrupted\n" +
"treant and recover the crystal fragment. Bring it back to me.\n" +
"Goldmoon hands you a prayer token of Mishakal.\n");
	say("Goldmoon hands a silver prayer token to " +
	    player->query_name() + ".\n", player);
    }
}
