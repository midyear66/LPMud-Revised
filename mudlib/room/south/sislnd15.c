/*
 * Isle of the Magi - Crumbling stone monument.
 * Marvus the wizard is here, mourning his brother Aldric who was killed
 * by the Wyrm of Arcanarton. Quest hub for the Wyrm Spellbook quest.
 */

inherit "room/room";

object marvus;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "A crumbling stone monument";
	long_desc =
"You are halfway up the hill.\n" +
"A crumbling monument stands on the side of the hill here.\n" +
"On top of the hill, to the west, stands the ruins of the tower of\n" +
"Arcanarton.\n" +
"A path winds around the hill to the north, and heads down to the shore\n" +
"of the island to the south and east.\n";
	dest_dir = ({
	    "room/south/sislnd14", "north",
	    "room/south/sislnd6", "south",
	    "room/south/sislnd5", "east",
	    "room/south/sislnd18", "west"
	});
	items = ({
	    "monument", "The monument is very old and is crumbling.\n" +
"Affixed to the side of the monument is a corroded old plaque which reads:\n" +
"+-------------------------------------------------------------------------+\n" +
"|                   BEWARE ALL YE WHO READ THIS MESSAGE                   |\n" +
"|        Be it known, that on this day, the tower of the evil mage        |\n" +
"|       Arcanarton was destroyed in an attack by the combined forces      |\n" +
"|               of all of the mages of the land of Lustria.               |\n" +
"|     The body of the mage Arcanarton was not found, and it is feared     |\n" +
"|             that his evil work in this world is not yet over.           |\n" +
"+-------------------------------------------------------------------------+",
	    "plaque", "The corroded plaque is affixed to the monument. Try looking at the monument"
	});
    }
    if (!marvus || !living(marvus)) {
	marvus = clone_object("obj/monster");
	marvus->set_name("marvus");
	marvus->set_alias("wizard");
	marvus->set_alt_name("wizard marvus");
	marvus->set_short("Marvus the wizard");
	marvus->set_long(
"An elderly wizard in faded blue robes, his face lined with grief.\n" +
"He clutches a gnarled staff and stares toward the old well with\n" +
"hollow eyes. He seems to be searching for something.\n");
	marvus->set_level(10);
	marvus->set_hp(100);
	marvus->set_al(200);
	marvus->set_aggressive(0);
	move_object(marvus, this_object());
	marvus->set_match(this_object(),
	    ({ "quest_talk" }),
	    ({ "says:" }),
	    ({ 0 })
	);
	marvus->load_chat(5, ({
	    "Marvus stares sadly toward the old well.\n",
	    "Marvus says: Aldric, brother... I will find your book.\n",
	    "Marvus mutters an incantation under his breath.\n"
	}));
    }
    /* Load the quest controller so it exists */
    if (!find_object("obj/wyrm_quest"))
	call_other("obj/wyrm_quest", "reset", 0);

    /* Register quest object in quest_room if not already there */
    if (find_object("room/quest_room")) {
	object ob;
	int found;
	ob = first_inventory(find_object("room/quest_room"));
	while (ob) {
	    if (ob->id("wyrm_spellbook")) {
		found = 1;
		break;
	    }
	    ob = next_inventory(ob);
	}
	if (!found) {
	    ob = clone_object("obj/wyrm_quest_obj");
	    move_object(ob, find_object("room/quest_room"));
	}
    }
}

void give_quest_reward(object player) {
    if (!player)
	return;
    player->add_exp(1500);
    player->set_quest("wyrm_spellbook");
    tell_object(player,
"You feel more experienced from your brave deed.\n");
}

void quest_talk(string str) {
    object controller;
    object player;
    object book;
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
	sscanf(rest, "%sbrother%s", a, b) != 2 &&
	sscanf(rest, "%saldric%s", a, b) != 2 &&
	sscanf(rest, "%swyrm%s", a, b) != 2 &&
	sscanf(rest, "%sspellbook%s", a, b) != 2 &&
	sscanf(rest, "%sbook%s", a, b) != 2 &&
	sscanf(rest, "%squest%s", a, b) != 2)
	return;

    controller = find_object("obj/wyrm_quest");
    if (!controller) {
	"obj/wyrm_quest"->reset(0);
	controller = find_object("obj/wyrm_quest");
    }

    /* Check if player already completed this quest */
    if (player->query_quests("wyrm_spellbook")) {
	tell_object(player,
"Marvus says: Ah, my friend. Thanks to you, I have Aldric's spellbook\n" +
"back. His knowledge lives on.\n");
	return;
    }

    /* Check if another player is already doing the quest */
    if (controller->query_stage() > 0 &&
	!controller->is_quest_player(player)) {
	tell_object(player,
"Marvus says: I have already enlisted someone's aid. Perhaps come\n" +
"back later.\n");
	return;
    }

    /* If this player is already on the quest, check for spellbook */
    if (controller->is_quest_player(player)) {
	book = present("spellbook", player);
	if (book) {
	    /* Player has the spellbook - complete the quest.
	       Give reward directly here instead of via controller,
	       because quest_talk runs in the monster's heartbeat
	       context where this_player() is the monster, and
	       add_exp() calls query_ip_number(this_player()) which
	       crashes on non-interactive objects. */
	    object bracer;
	    bracer = clone_object("obj/armour");
	    bracer->set_id("bracer");
	    bracer->set_alias("arcane bracer");
	    bracer->set_type("armour");
	    bracer->set_short("An arcane bracer");
	    bracer->set_long(
"A leather bracer inscribed with glowing protective runes. It was\n" +
"crafted by Marvus in memory of his brother Aldric.\n");
	    bracer->set_ac(1);
	    bracer->set_value(300);
	    bracer->set_weight(2);
	    move_object(bracer, player);
	    destruct(book);
	    controller->set_stage(0);
	    tell_object(player,
"Marvus says: Aldric's spellbook! You found it! I cannot thank you\n" +
"enough, brave one. My brother's knowledge will not be lost.\n" +
"Please, take this -- Aldric would have wanted his savior to have it.\n" +
"Marvus traces a glowing sigil in the air and a bracer materializes.\n" +
"Marvus places an arcane bracer on your arm.\n");
	    say("Marvus clutches the old spellbook to his chest and weeps with\n" +
		"relief, then presses a glowing bracer into " +
		player->query_name() + "'s hand.\n", player);
	    give_quest_reward(player);
	    return;
	}
	/* Player is on quest but does not have the spellbook yet */
	tell_object(player,
"Marvus says: The wyrm lurks at the bottom of the old well to the\n" +
"northwest. Descend the ladder and slay the beast. My brother's\n" +
"spellbook should be among its hoard.\n");
	return;
    }

    /* Start the quest */
    if (controller->start_quest(player)) {
	tell_object(player,
"Marvus says: Please, traveler, I need your help. My brother Aldric\n" +
"came to this cursed island to study Arcanarton's magic. He descended\n" +
"into the old well to the northwest, seeking the wyrm's lair, hoping\n" +
"to recover artifacts of power. He never returned.\n" +
"Marvus says: The Wyrm of Arcanarton guards the bottom of that well.\n" +
"It is a fearsome undead beast. I am too old and frail to face it\n" +
"myself, but a warrior of your skill might prevail.\n" +
"Marvus says: My brother's spellbook -- please, if you can slay the\n" +
"wyrm, bring the book back to me. It is all I have left of him.\n");
	say("Marvus clasps " + player->query_name() +
	    "'s hands and looks at them with desperate hope.\n", player);
    }
}
