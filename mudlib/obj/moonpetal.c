/*
 * Moonpetal flower - quest item for the Fairy Plague quest.
 * When the quest player picks it up at stage 2, advances to stage 3.
 */

inherit "obj/treasure";

void reset(int arg) {
    if (arg) return;
    set_id("moonpetal");
    set_alias("moonpetal flower");
    set_short("A moonpetal flower");
    set_long(
"A delicate flower with pale, luminescent petals that seem to glow\n" +
"with an inner moonlight. It is said to have purifying properties.\n");
    set_weight(1);
    set_value(15);
}

int id(string str) {
    return str == "moonpetal" || str == "flower" || str == "moonpetal flower";
}

int get() {
    object controller;

    controller = find_object("obj/fairy_quest");
    if (controller && controller->is_quest_player(this_player()) &&
	controller->query_stage() == 2) {
	controller->set_stage(3);
	write("You should bring this back to the fairy elder in the glade.\n");
	say(this_player()->query_name() +
	    " carefully picks up a glowing moonpetal flower.\n");
    }
    return 1;
}
