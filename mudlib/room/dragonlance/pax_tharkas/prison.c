/*
 * Pax Tharkas Prison - Elistan gives Quest 3.
 */

inherit "room/room";

object elistan;

void reset(int arg) {
    if (!arg) {
	set_light(0);
	short_desc = "The prison";
	long_desc =
"A dank underground prison beneath the fortress courtyard. Iron\n" +
"bars line the walls, forming rows of cramped cells. The smell of\n" +
"misery hangs in the air. Huddled prisoners -- elves, humans, and\n" +
"dwarves -- watch you with hollow, hopeless eyes. One prisoner\n" +
"stands out: an elderly man in white robes who radiates a calm\n" +
"authority despite his imprisonment. An iron grate overhead leads\n" +
"up to the courtyard.\n";
	dest_dir = ({
	    "room/dragonlance/pax_tharkas/court", "up"
	});
	items = ({
	    "bars", "Rows of iron-barred cells lining the walls",
	    "cells", "Cramped cells holding wretched prisoners",
	    "prisoners", "Elves, humans, and dwarves, all looking thin and exhausted",
	    "grate", "An iron grate overhead leading up to the courtyard"
	});
    }
    if (!elistan || !living(elistan)) {
	elistan = clone_object("obj/monster");
	elistan->set_name("elistan");
	elistan->set_alias("elistan");
	elistan->set_alt_name("priest");
	elistan->set_short("Elistan, Priest of Paladine");
	elistan->set_long(
"An elderly man with a dignified bearing, dressed in once-white\n" +
"robes now stained and torn. His silver hair frames a face that\n" +
"radiates wisdom and compassion. Despite his imprisonment, his\n" +
"eyes shine with unwavering faith. He is Elistan, a priest of\n" +
"Paladine, leader of the imprisoned people.\n");
	elistan->set_level(10);
	elistan->set_hp(122);
	elistan->set_al(400);
	elistan->set_aggressive(0);
	move_object(elistan, this_object());
	elistan->set_match(this_object(),
	    ({ "quest_talk" }),
	    ({ "says:" }),
	    ({ 0 })
	);
	elistan->load_chat(5, ({
	    "Elistan prays quietly for the prisoners.\n",
	    "Elistan says: Paladine has not forsaken us.\n",
	    "Elistan tends to a sick prisoner with gentle hands.\n"
	}));
    }
    /* Load the quest controller */
    if (!find_object("obj/dl_pax_quest"))
	call_other("obj/dl_pax_quest", "reset", 0);

    /* Register quest in quest_room */
    if (find_object("room/quest_room")) {
	object ob;
	int found;
	ob = first_inventory(find_object("room/quest_room"));
	while (ob) {
	    if (ob->id("liberation_pax_tharkas")) {
		found = 1;
		break;
	    }
	    ob = next_inventory(ob);
	}
	if (!found) {
	    ob = clone_object("obj/dl_pax_quest_obj");
	    move_object(ob, find_object("room/quest_room"));
	}
    }
}

void give_quest_reward(object player) {
    object sword;

    if (!player)
	return;
    player->add_exp(3000);
    player->set_quest("liberation_pax_tharkas");

    sword = clone_object("obj/weapon");
    sword->set_name("wyrmslayer");
    sword->set_alias("sword");
    sword->set_short("Wyrmslayer, the Dragonbane");
    sword->set_long(
"A magnificent longsword forged from an alloy of steel and\n" +
"dragonlance silver. The blade gleams with an inner fire and\n" +
"is inscribed with ancient runes of power. It was crafted by\n" +
"the finest dwarven smiths of Thorbardin to slay dragons.\n");
    sword->set_class(14);
    sword->set_value(1200);
    sword->set_weight(5);
    move_object(sword, player);

    tell_object(player, "You feel much more experienced from your heroic deed.\n");
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

    if (sscanf(rest, "%shello%s", a, b) != 2 &&
	sscanf(rest, "%shelp%s", a, b) != 2 &&
	sscanf(rest, "%sfree%s", a, b) != 2 &&
	sscanf(rest, "%sprison%s", a, b) != 2 &&
	sscanf(rest, "%sverminaard%s", a, b) != 2 &&
	sscanf(rest, "%sliberat%s", a, b) != 2 &&
	sscanf(rest, "%squest%s", a, b) != 2 &&
	sscanf(rest, "%shi%s", a, b) != 2)
	return;

    controller = find_object("obj/dl_pax_quest");
    if (!controller) {
	"obj/dl_pax_quest"->reset(0);
	controller = find_object("obj/dl_pax_quest");
    }

    if (player->query_quests("liberation_pax_tharkas")) {
	tell_object(player,
"Elistan says: You are the liberator of Pax Tharkas! Your name\n" +
"will be remembered for generations. Paladine bless you.\n");
	return;
    }

    if (controller->query_stage() > 0 &&
	!controller->is_quest_player(player)) {
	tell_object(player,
"Elistan says: Another champion is already working to free us.\n" +
"Please return later.\n");
	return;
    }

    if (controller->is_quest_player(player)) {
	int stage;
	stage = controller->query_stage();
	if (stage >= 1 && stage <= 4) {
	    tell_object(player,
"Elistan says: Find the key in the armory, free Lady Laurana\n" +
"from the west tower, and defeat Verminaard at the summit!\n");
	    return;
	}
	if (stage == 5) {
	    controller->set_stage(0);
	    tell_object(player,
"Elistan says: You have done it! Verminaard is defeated and the\n" +
"prisoners are free! The people of Pax Tharkas owe you their\n" +
"lives. Take this blade -- Wyrmslayer, forged in Thorbardin to\n" +
"be the bane of dragons. May it serve you well in battles to come.\n" +
"Elistan places a magnificent sword in your hands.\n");
	    say("Elistan presents a gleaming sword to " +
		player->query_name() + " in gratitude.\n", player);
	    give_quest_reward(player);
	    return;
	}
	return;
    }

    /* Start the quest */
    if (controller->start_quest(player)) {
	tell_object(player,
"Elistan says: Paladine has sent you! Listen well, friend.\n" +
"We are prisoners of Dragon Highlord Verminaard. He rules this\n" +
"fortress with an iron fist and the power of Takhisis.\n" +
"To free us, you must do three things:\n" +
"First, find the key to the cells -- it is kept in the armory,\n" +
"east of the courtyard above.\n" +
"Second, free Lady Laurana -- she is held in the west tower.\n" +
"Third, defeat Verminaard himself at the summit of the east tower.\n" +
"Only then will the prisoners truly be free.\n" +
"Go with the blessing of Paladine!\n");
	say("Elistan blesses " + player->query_name() +
	    " and sends them on a mission.\n", player);
    }
}
