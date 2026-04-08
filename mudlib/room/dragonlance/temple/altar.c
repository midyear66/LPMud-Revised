/*
 * Temple Altar - Blood altar. Gem activation point for Quest 4.
 * North exit to sanctum blocked until gem is used.
 */

inherit "room/room";

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "The blood altar";
	long_desc =
"A massive altar of black obsidian dominates this chamber, its\n" +
"surface stained dark with centuries of blood sacrifice. Five\n" +
"dragon heads are carved into the altar's base, each with ruby\n" +
"eyes that seem to follow your movements. Behind the altar, a\n" +
"passage leads north to the inner sanctum, but a shimmering wall\n" +
"of dark energy blocks the way. The nave lies to the south.\n";
	dest_dir = ({
	    "room/dragonlance/temple/nave", "south"
	});
	items = ({
	    "altar", "A massive obsidian altar stained with the blood of sacrifices",
	    "heads", "Five carved dragon heads with ruby eyes that seem to watch you",
	    "eyes", "Ruby eyes set into the dragon carvings -- they glow faintly",
	    "barrier", "A shimmering wall of dark energy blocking the north passage",
	    "energy", "Dark energy crackles and writhes, blocking your way north"
	});
    }
    /* Load the quest controller */
    if (!find_object("obj/dl_takhisis_quest"))
	call_other("obj/dl_takhisis_quest", "reset", 0);
}

void init() {
    ::init();
    add_action("use_gem", "use");
    add_action("use_gem", "place");
    add_action("go_north", "north");
}

int go_north() {
    object controller;

    controller = find_object("obj/dl_takhisis_quest");
    if (controller && controller->is_quest_player(this_player()) &&
	controller->query_stage() >= 2) {
	this_player()->move_player(
	    "north#room/dragonlance/temple/sanctum");
	return 1;
    }

    write("A shimmering wall of dark energy blocks your passage north.\n" +
	  "It crackles menacingly as you approach.\n");
    return 1;
}

int use_gem(string str) {
    object controller;
    object player;
    object gem;
    string a, b;

    if (!str)
	return 0;

    if (sscanf(str, "%sgem%s", a, b) != 2 &&
	sscanf(str, "%sdiamond%s", a, b) != 2)
	return 0;

    player = this_player();
    gem = present("gem", player);
    if (!gem)
	gem = present("gem of paladine", player);

    if (!gem) {
	write("You don't have a suitable gem.\n");
	return 1;
    }

    controller = find_object("obj/dl_takhisis_quest");
    if (!controller || !controller->is_quest_player(player) ||
	controller->query_stage() != 1) {
	write("Nothing happens.\n");
	return 1;
    }

    /* Activate the gem */
    destruct(gem);
    controller->set_stage(2);

    write(
"You place the Gem of Paladine upon the blood altar. The diamond\n" +
"blazes with brilliant platinum light that sears away the dark\n" +
"stains on the obsidian! The ruby eyes of the dragon carvings\n" +
"shatter one by one! The wall of dark energy blocking the north\n" +
"passage screams like a living thing and dissolves into nothing!\n" +
"The way to the inner sanctum is open!\n");
    say(player->query_name() +
	" places a blazing gem on the altar! The dark barrier shatters!\n",
	player);

    return 1;
}
