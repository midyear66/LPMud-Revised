/*
 * Cemetery - above-ground area for the Restless Dead quest.
 * Brother Cedric tends the churchyard here. Connected from the church (north).
 */

inherit "room/room";

object cedric;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "Old cemetery";
	long_desc =
"An old churchyard cemetery surrounded by a rusting iron fence. Weathered\n" +
"gravestones lean at odd angles among patches of dead grass. Several graves\n" +
"near the back have been disturbed -- freshly turned earth spills across\n" +
"the paths. A heavy stone trapdoor leads down into an ancient crypt.\n" +
"The village church lies to the south.\n";
	dest_dir = ({
	    "room/church", "south",
	    "room/crypt/entrance", "down"
	});
	items = ({
	    "gravestones", "Worn stone markers, most too weathered to read",
	    "graves", "Several graves near the back show signs of disturbance",
	    "fence", "A rusting iron fence surrounds the cemetery",
	    "trapdoor", "A heavy stone trapdoor set into the ground, leading down into darkness",
	    "earth", "Freshly turned earth -- something has been digging here"
	});
    }
    if (!cedric || !living(cedric)) {
	cedric = clone_object("obj/monster");
	cedric->set_name("cedric");
	cedric->set_alias("brother cedric");
	cedric->set_alt_name("monk");
	cedric->set_short("Brother Cedric");
	cedric->set_long(
"A weary-looking monk in brown robes, clutching a string of prayer beads.\n" +
"Dark circles under his eyes suggest many sleepless nights. He tends the\n" +
"cemetery grounds with quiet devotion.\n");
	cedric->set_level(5);
	cedric->set_hp(60);
	cedric->set_al(200);
	cedric->set_aggressive(0);
	move_object(cedric, this_object());
	cedric->set_match(this_object(),
	    ({ "quest_talk" }),
	    ({ "says:" }),
	    ({ 0 })
	);
	cedric->load_chat(5, ({
	    "Brother Cedric murmurs a quiet prayer over a gravestone.\n",
	    "Brother Cedric glances nervously toward the crypt entrance.\n",
	    "Brother Cedric says: The dead should rest in peace...\n"
	}));
    }
    /* Load the quest controller so it exists */
    if (!find_object("obj/crypt_quest"))
	call_other("obj/crypt_quest", "reset", 0);

    /* Register quest object in quest_room if not already there */
    if (find_object("room/quest_room")) {
	object ob;
	int found;
	ob = first_inventory(find_object("room/quest_room"));
	while (ob) {
	    if (ob->id("restless_dead")) {
		found = 1;
		break;
	    }
	    ob = next_inventory(ob);
	}
	if (!found) {
	    ob = clone_object("obj/crypt_quest_obj");
	    move_object(ob, find_object("room/quest_room"));
	}
    }
}

void give_quest_reward(object player) {
    if (!player)
	return;
    player->add_exp(750);
    player->set_quest("restless_dead");
    tell_object(player,
"You feel more experienced from your brave deed.\n");
}

void quest_talk(string str) {
    object controller;
    object player;
    object water;
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
	sscanf(rest, "%sdead%s", a, b) != 2 &&
	sscanf(rest, "%sgrave%s", a, b) != 2 &&
	sscanf(rest, "%scrypt%s", a, b) != 2 &&
	sscanf(rest, "%sundead%s", a, b) != 2 &&
	sscanf(rest, "%srestless%s", a, b) != 2 &&
	sscanf(rest, "%sdisturb%s", a, b) != 2 &&
	sscanf(rest, "%shi%s", a, b) != 2)
	return;

    controller = find_object("obj/crypt_quest");
    if (!controller) {
	"obj/crypt_quest"->reset(0);
	controller = find_object("obj/crypt_quest");
    }

    /* Check if player already completed this quest */
    if (player->query_quests("restless_dead")) {
	tell_object(player,
"Brother Cedric says: Bless you, friend. Thanks to you, the dead rest\n" +
"peacefully once more.\n");
	return;
    }

    /* Check if another player is already doing the quest */
    if (controller->query_stage() > 0 &&
	!controller->is_quest_player(player)) {
	tell_object(player,
"Brother Cedric says: Someone else is already looking into the disturbances.\n" +
"Perhaps check back later.\n");
	return;
    }

    /* If this player is already on the quest, give stage-appropriate response */
    if (controller->is_quest_player(player)) {
	int stage;
	stage = controller->query_stage();
	if (stage == 1 || stage == 2) {
	    tell_object(player,
"Brother Cedric says: Please, you must descend into the crypt and put an\n" +
"end to whatever evil stirs below. Use the holy water on the source of\n" +
"the dark magic!\n");
	    return;
	}
	if (stage == 3) {
	    /* Player defeated the wraith - complete the quest.
	       Give reward directly here instead of via controller,
	       because quest_talk runs in the monster's heartbeat
	       context where this_player() is the monster, and
	       add_exp() calls query_ip_number(this_player()) which
	       crashes on non-interactive objects. */
	    object ring;
	    ring = clone_object("obj/armour");
	    ring->set_id("ring");
	    ring->set_alias("silver ring");
	    ring->set_type("ring");
	    ring->set_short("An enchanted silver ring");
	    ring->set_long("A silver ring etched with holy symbols, glowing faintly with\nprotective enchantment. You could sell this at the shop.\n");
	    ring->set_ac(1);
	    ring->set_value(150);
	    ring->set_weight(1);
	    move_object(ring, player);
	    controller->set_stage(0);
	    tell_object(player,
"Brother Cedric says: You did it! I can feel it -- the restless spirits\n" +
"have been calmed. The cemetery is at peace for the first time in weeks.\n" +
"You have done a great service. Please, take this ring as a token of\n" +
"my gratitude. It was blessed by the abbot himself.\n" +
"Brother Cedric places a silver ring in your hand.\n");
	    say("Brother Cedric beams with relief and presses something into " +
		player->query_name() + "'s hand.\n", player);
	    give_quest_reward(player);
	    return;
	}
	return;
    }

    /* Start the quest */
    if (controller->start_quest(player)) {
	water = clone_object("obj/holy_water");
	move_object(water, player);
	tell_object(player,
"Brother Cedric says: Thank the heavens someone has come! The graves are\n" +
"being disturbed every night and unearthly moans echo from the old crypt.\n" +
"I believe the spirit of Valdris, a sorcerer buried here centuries ago,\n" +
"has awakened and is raising the dead.\n" +
"Take this vial of holy water -- it should break the dark ward on his\n" +
"tomb. Descend into the crypt, find Valdris's resting place, and put\n" +
"his spirit to rest. Be careful, the dead walk down there!\n" +
"Brother Cedric hands you a vial of holy water.\n");
	say("Brother Cedric hands a vial of shimmering water to " +
	    player->query_name() + ".\n", player);
    }
}
