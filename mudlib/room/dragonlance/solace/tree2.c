/*
 * Solace Tree Walkway 2 - Secluded overlook and Quest 1 boss location.
 * The corrupted treant spawns here when the player uses the prayer token.
 */

inherit "room/room";

object treant;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "A secluded overlook";
	long_desc =
"The walkway ends at a broad platform built around the trunk of an\n" +
"ancient vallenwood. From here you can see for miles across the\n" +
"forest canopy. But something is wrong -- the bark of this great\n" +
"tree is blackened and cracked, and its leaves are withered and grey.\n" +
"A dark corruption seems to pulse within the wood itself. The\n" +
"walkway leads back east.\n";
	dest_dir = ({
	    "room/dragonlance/solace/tree1", "east"
	});
	items = ({
	    "tree", "The ancient vallenwood is sick -- dark veins of corruption pulse through its bark",
	    "bark", "Blackened and cracked, oozing a dark sap",
	    "leaves", "Grey and withered, rustling with an unnatural sound",
	    "canopy", "The forest canopy stretches to the horizon in every direction"
	});
    }
    /* Load the quest controller */
    if (!find_object("obj/dl_staff_quest"))
	call_other("obj/dl_staff_quest", "reset", 0);
}

void init() {
    ::init();
    add_action("use_token", "use");
    add_action("use_token", "pray");
}

int use_token(string str) {
    object controller;
    object player;
    object token;
    string a, b;

    if (!str)
	return 0;

    if (sscanf(str, "%sprayer%s", a, b) != 2 &&
	sscanf(str, "%stoken%s", a, b) != 2 &&
	str != "pray")
	return 0;

    player = this_player();
    token = present("prayer_token", player);
    if (!token)
	token = present("token", player);

    if (!token) {
	write("You don't have a prayer token.\n");
	return 1;
    }

    controller = find_object("obj/dl_staff_quest");
    if (!controller) {
	write("Nothing happens.\n");
	return 1;
    }

    if (!controller->is_quest_player(player) ||
	controller->query_stage() != 1) {
	write("Nothing happens.\n");
	return 1;
    }

    /* Activate the token */
    write(
"You hold the prayer token aloft and whisper a prayer to Mishakal.\n" +
"The token blazes with golden light! The corrupted tree shudders and\n" +
"groans, and a massive shape tears itself free from the blackened\n" +
"trunk -- a corrupted treant, its wooden body writhing with dark\n" +
"energy! It turns toward you with hollow, burning eyes!\n");
    say(player->query_name() +
	" holds a prayer token aloft. The corrupted tree comes alive!\n" +
	"A terrible treant tears itself free from the trunk!\n", player);

    destruct(token);
    controller->set_stage(2);

    /* Spawn the corrupted treant */
    treant = clone_object("obj/monster");
    treant->set_name("treant");
    treant->set_alias("corrupted treant");
    treant->set_alt_name("corrupted");
    treant->set_race("plant");
    treant->set_level(5);
    treant->set_hp(82);
    treant->set_al(-200);
    treant->set_ac(1);
    treant->set_wc(8);
    treant->set_short("A corrupted treant");
    treant->set_long(
"A massive tree-creature twisted by dark magic. Its bark is blackened\n" +
"and cracked, oozing dark sap. Gnarled branches serve as arms, ending\n" +
"in clawed wooden fingers. Its hollow eyes burn with a sickly violet\n" +
"fire, and the air around it reeks of decay.\n");
    treant->set_aggressive(1);
    treant->set_chance(20);
    treant->set_spell_dam(8);
    treant->set_spell_mess1("The treant's roots erupt from the ground, entangling you!\n");
    treant->set_spell_mess2("Grasping roots erupt from the ground around ");
    treant->load_a_chat(30, ({
	"The corrupted treant groans with a sound like splintering wood!\n",
	"Dark sap drips from the treant's clawed fingers!\n",
	"The treant swings a massive branch at you!\n"
    }));
    treant->set_dead_ob(this_object());
    move_object(treant, this_object());

    return 1;
}

void monster_died(object killed) {
    object controller;
    object fragment;

    controller = find_object("obj/dl_staff_quest");
    if (!controller)
	return;

    if (controller->query_stage() == 2) {
	controller->set_stage(3);

	/* Drop the crystal fragment */
	fragment = clone_object("obj/treasure");
	fragment->set_id("crystal");
	fragment->set_alias("crystal fragment");
	fragment->set_short("A glowing crystal fragment");
	fragment->set_long(
"A shard of blue crystal that pulses with a warm, healing light.\n" +
"It feels warm to the touch and hums with divine energy. This must\n" +
"be a fragment of the legendary Blue Crystal Staff.\n");
	fragment->set_value(100);
	fragment->set_weight(1);
	move_object(fragment, this_object());

	tell_room(this_object(),
"As the corrupted treant crumbles to splinters, a brilliant blue\n" +
"crystal fragment falls from its shattered core, pulsing with\n" +
"warm light. The ancient vallenwood tree begins to heal, fresh\n" +
"green shoots already pushing through the blackened bark.\n");
    }
    treant = 0;
}
