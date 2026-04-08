/*
 * Xak Tsaroth Temple - Ruined temple, Riverwind gives Quest 2.
 */

inherit "room/room";

object riverwind;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "A ruined temple";
	long_desc =
"The shell of what was once a magnificent temple. The roof has\n" +
"collapsed, leaving the interior open to the sky. Broken columns\n" +
"ring a shattered altar, and faded murals on the remaining walls\n" +
"depict the goddess Mishakal healing the sick. Despite the\n" +
"destruction, there is a faint sense of holiness that lingers here.\n" +
"The plaza lies to the east.\n";
	dest_dir = ({
	    "room/dragonlance/xak_tsaroth/plaza", "east"
	});
	items = ({
	    "columns", "Broken columns that once supported a grand ceiling",
	    "altar", "A shattered stone altar, still bearing faded holy symbols",
	    "murals", "Faded murals depicting Mishakal, goddess of healing",
	    "walls", "The remaining walls are scorched but still bear traces of beauty"
	});
    }
    if (!riverwind || !living(riverwind)) {
	riverwind = clone_object("obj/monster");
	riverwind->set_name("riverwind");
	riverwind->set_alias("riverwind");
	riverwind->set_alt_name("ranger");
	riverwind->set_short("Riverwind, Plainsman Ranger");
	riverwind->set_long(
"A tall, powerfully built man with copper skin and long black hair.\n" +
"He wears the leathers of a Que-Shu plainsman and carries a longbow\n" +
"across his back. His dark eyes are fierce but honorable. He bears\n" +
"several wounds -- he has clearly been fighting the draconians.\n");
	riverwind->set_level(8);
	riverwind->set_hp(106);
	riverwind->set_al(200);
	riverwind->set_aggressive(0);
	move_object(riverwind, this_object());
	riverwind->set_match(this_object(),
	    ({ "quest_talk" }),
	    ({ "says:" }),
	    ({ 0 })
	);
	riverwind->load_chat(5, ({
	    "Riverwind scans the ruins with wary eyes.\n",
	    "Riverwind says: The draconians grow bolder every day.\n",
	    "Riverwind touches a wound on his arm and winces.\n"
	}));
    }
    /* Load the quest controller */
    if (!find_object("obj/dl_disks_quest"))
	call_other("obj/dl_disks_quest", "reset", 0);

    /* Register quest in quest_room */
    if (find_object("room/quest_room")) {
	object ob;
	int found;
	ob = first_inventory(find_object("room/quest_room"));
	while (ob) {
	    if (ob->id("disks_of_mishakal")) {
		found = 1;
		break;
	    }
	    ob = next_inventory(ob);
	}
	if (!found) {
	    ob = clone_object("obj/dl_disks_quest_obj");
	    move_object(ob, find_object("room/quest_room"));
	}
    }
}

void give_quest_reward(object player) {
    object shield;

    if (!player)
	return;
    player->add_exp(1500);
    player->set_quest("disks_of_mishakal");

    shield = clone_object("obj/armour");
    shield->set_id("shield");
    shield->set_alias("dragonscale shield");
    shield->set_type("shield");
    shield->set_short("A dragonscale shield");
    shield->set_long(
"A round shield crafted from the scales of a black dragon. The\n" +
"scales are hard as steel and shimmer with a dark iridescence.\n" +
"Despite its origin, it has been blessed by Mishakal and now\n" +
"serves to protect the righteous.\n");
    shield->set_ac(2);
    shield->set_value(500);
    shield->set_weight(4);
    move_object(shield, player);

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

    if (sscanf(rest, "%shello%s", a, b) != 2 &&
	sscanf(rest, "%shelp%s", a, b) != 2 &&
	sscanf(rest, "%sdisk%s", a, b) != 2 &&
	sscanf(rest, "%smishakal%s", a, b) != 2 &&
	sscanf(rest, "%sdragon%s", a, b) != 2 &&
	sscanf(rest, "%swell%s", a, b) != 2 &&
	sscanf(rest, "%squest%s", a, b) != 2 &&
	sscanf(rest, "%shi%s", a, b) != 2)
	return;

    controller = find_object("obj/dl_disks_quest");
    if (!controller) {
	"obj/dl_disks_quest"->reset(0);
	controller = find_object("obj/dl_disks_quest");
    }

    if (player->query_quests("disks_of_mishakal")) {
	tell_object(player,
"Riverwind says: The Disks are safe, thanks to you. Mishakal's\n" +
"wisdom will guide us through the dark times ahead.\n");
	return;
    }

    if (controller->query_stage() > 0 &&
	!controller->is_quest_player(player)) {
	tell_object(player,
"Riverwind says: Another warrior has already gone below. We must\n" +
"wait for their return.\n");
	return;
    }

    if (controller->is_quest_player(player)) {
	int stage;
	stage = controller->query_stage();
	if (stage >= 1 && stage <= 3) {
	    tell_object(player,
"Riverwind says: The Disks of Mishakal lie deep below the city,\n" +
"in the lair of the black dragon Khisanth. Use the rope at the\n" +
"well to descend. Be brave!\n");
	    return;
	}
	if (stage == 4) {
	    if (present("disks", player)) {
		object disks;
		disks = present("disks", player);
		if (disks) destruct(disks);
		controller->set_stage(0);
		tell_object(player,
"Riverwind says: The Disks of Mishakal! You have done it!\n" +
"Riverwind takes the ancient platinum disks reverently. His eyes\n" +
"fill with tears of relief.\n" +
"Riverwind says: Goldmoon and I searched so long for these. They\n" +
"contain the wisdom of the true gods. You have earned this -- a\n" +
"shield forged from the scales of the dragon you defeated.\n" +
"Riverwind hands you a magnificent dragonscale shield.\n");
		say("Riverwind accepts the Disks of Mishakal from " +
		    player->query_name() + " with tears of gratitude.\n" +
		    "Riverwind presents a dragonscale shield to " +
		    player->query_name() + ".\n", player);
		give_quest_reward(player);
		return;
	    }
	    tell_object(player,
"Riverwind says: You defeated Khisanth? Then the Disks must be\n" +
"in the dragon's lair. Go back and find them!\n");
	    return;
	}
	return;
    }

    /* Start the quest */
    if (controller->start_quest(player)) {
	object rope;
	rope = clone_object("obj/treasure");
	rope->set_id("rope");
	rope->set_alias("climbing rope");
	rope->set_short("A sturdy climbing rope");
	rope->set_long(
"A long coil of strong rope, suitable for climbing. It looks\n" +
"long enough to descend a deep well.\n");
	rope->set_value(10);
	rope->set_weight(2);
	move_object(rope, player);
	tell_object(player,
"Riverwind says: Listen well. Deep beneath this city, in caverns\n" +
"once sacred to the old gods, lie the Disks of Mishakal -- platinum\n" +
"tablets inscribed with the wisdom of the goddess herself.\n" +
"But a black dragon named Khisanth has claimed the caverns as her\n" +
"lair. She guards the Disks jealously.\n" +
"There is a well in the plaza. Take this rope and use it to descend.\n" +
"Find the Disks, defeat the dragon if you must, and bring them back\n" +
"to me. The fate of Krynn may depend on it.\n" +
"Riverwind hands you a sturdy climbing rope.\n");
	say("Riverwind hands a coil of rope to " +
	    player->query_name() + ".\n", player);
    }
}
