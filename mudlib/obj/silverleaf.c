/*
 * Silverleaf herb - quest item for the Forest Remedy quest.
 * When the quest player picks it up at stage 2, advances to stage 3.
 */

inherit "obj/treasure";

void reset(int arg) {
    if (arg) return;
    set_id("silverleaf");
    set_alias("herb");
    set_short("A silverleaf herb");
    set_long("A rare herb with delicate silver-veined leaves. It shimmers faintly in the light.\n");
    set_weight(1);
    set_value(10);
}

int get() {
    object controller;

    controller = find_object("obj/forest_quest");
    if (controller && controller->is_quest_player(this_player()) &&
	controller->query_stage() == 2) {
	controller->set_stage(3);
	write("You should bring this back to Doctor Willem.\n");
	say(this_player()->query_name() + " carefully picks up a silverleaf herb.\n");
    }
    return 1;
}
