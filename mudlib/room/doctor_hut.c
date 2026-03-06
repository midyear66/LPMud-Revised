/*
 * Doctor's hut - off narr_alley to the west.
 * Doctor Willem helps with the Forest Remedy quest.
 */

inherit "room/room";

object doctor;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "Doctor's hut";
	long_desc =
"A small but tidy hut that serves as a doctor's office. Shelves line\n" +
"the walls, filled with medical instruments and bottles of tinctures.\n" +
"The air smells faintly of antiseptic herbs.\n";
	dest_dir = ({ "room/narr_alley", "east" });
	items = ({
	    "shelves", "Rows of neatly organized medical instruments and bottles",
	    "instruments", "Various surgical and medical tools, well maintained",
	    "bottles", "Bottles of tinctures and medicines in various colors"
	});
    }
    if (!doctor || !living(doctor)) {
	doctor = clone_object("obj/monster");
	doctor->set_name("willem");
	doctor->set_alias("doctor");
	doctor->set_alt_name("doctor willem");
	doctor->set_short("Doctor Willem");
	doctor->set_long(
"A middle-aged man with kind eyes and steady hands. His shelves are\n" +
"lined with medical instruments.\n");
	doctor->set_level(5);
	doctor->set_al(200);
	doctor->set_aggressive(0);
	move_object(doctor, this_object());
	doctor->set_match(this_object(),
	    ({ "quest_talk" }),
	    ({ "says:" }),
	    ({ 0 })
	);
	doctor->load_chat(3, ({
	    "Doctor Willem examines a bottle of tincture carefully.\n",
	    "Doctor Willem says: Medicine is as much art as science.\n",
	    "Doctor Willem polishes a surgical instrument.\n"
	}));
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
    if (sscanf(rest, "%shelp%s", a, b) != 2 &&
	sscanf(rest, "%straveler%s", a, b) != 2 &&
	sscanf(rest, "%swound%s", a, b) != 2 &&
	sscanf(rest, "%sherb%s", a, b) != 2 &&
	sscanf(rest, "%smedicine%s", a, b) != 2 &&
	sscanf(rest, "%shurt%s", a, b) != 2 &&
	sscanf(rest, "%sinjur%s", a, b) != 2 &&
	sscanf(rest, "%shello%s", a, b) != 2 &&
	sscanf(rest, "%shi%s", a, b) != 2)
	return;

    controller = find_object("obj/forest_quest");
    if (!controller) {
	call_other("obj/forest_quest", "reset", 0);
	controller = find_object("obj/forest_quest");
    }

    if (controller && controller->is_quest_player(player) &&
	controller->query_stage() == 1) {
	tell_object(player,
"Doctor Willem says: A wounded traveler? I can help, but I'll need a\n" +
"silverleaf herb to make the proper remedy. It's a rare plant -- I've\n" +
"heard there's an old herbalist who lives deep in the forest. She might\n" +
"know where it grows. Find her and ask about the silverleaf.\n");
	say("Doctor Willem nods gravely and rummages through his supplies.\n",
	    player);
	controller->set_stage(2);
	controller->spawn_herb();
	return;
    }

    tell_object(player,
"Doctor Willem says: Good day. I am always here if you need medical\n" +
"attention.\n");
}

void init() {
    ::init();
    add_action("do_give", "give");
}

int do_give(string str) {
    string item, target;
    object controller;
    object player;
    object herb;
    object medicine;

    if (!str)
	return 0;

    if (sscanf(str, "%s to willem", item) != 1 &&
	sscanf(str, "%s to doctor", item) != 1 &&
	sscanf(str, "%s to doctor willem", item) != 1)
	return 0;

    if (item != "silverleaf" && item != "herb")
	return 0;

    player = this_player();
    controller = find_object("obj/forest_quest");

    if (!controller || !controller->is_quest_player(player) ||
	controller->query_stage() != 3) {
	write("Doctor Willem says: I have no use for that at the moment.\n");
	return 1;
    }

    herb = present("silverleaf", player);
    if (!herb) {
	write("You don't have a silverleaf herb.\n");
	return 1;
    }

    /* Take the herb and prepare the medicine */
    destruct(herb);
    write(
"Doctor Willem takes the silverleaf and examines it carefully.\n" +
"Doctor Willem says: Ah, excellent! This is just what I need.\n" +
"He grinds the leaves with a mortar and pestle, mixing in a clear liquid.\n" +
"After a few moments, he pours the mixture into a small vial.\n" +
"Doctor Willem says: Here -- this healing medicine should do the trick.\n" +
"Give it to the traveler quickly.\n");
    say(player->query_name() +
	" gives a silverleaf herb to Doctor Willem, who prepares a medicine.\n",
	player);

    medicine = clone_object("obj/treasure");
    medicine->set_id("medicine");
    medicine->set_alias("healing medicine");
    medicine->set_short("A healing medicine");
    medicine->set_long("A small vial filled with a softly glowing liquid. It was prepared by Doctor Willem from a rare silverleaf herb.\n");
    medicine->set_weight(1);
    medicine->set_value(50);
    move_object(medicine, player);

    controller->set_stage(4);
    write("You should bring the medicine back to the wounded traveler.\n");
    return 1;
}
