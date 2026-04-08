/*
 * Pax Tharkas West Tower - Laurana is imprisoned here. Quest 3 unlock.
 */

inherit "room/room";

object laurana;
int cell_open;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "West tower prison";
	long_desc =
"The west tower has been converted to a high-security prison.\n" +
"A single iron-barred cell dominates the room, its lock heavy\n" +
"and solid. Inside the cell, you can see a proud elven woman\n" +
"in tattered but once-fine clothing. She meets your gaze with\n" +
"fierce, intelligent eyes. The upper corridor lies to the east.\n";
	dest_dir = ({
	    "room/dragonlance/pax_tharkas/upper", "east"
	});
	items = ({
	    "cell", "A heavy iron-barred cell with a solid lock",
	    "lock", "A heavy lock on the cell door -- it needs a key",
	    "bars", "Thick iron bars, cold to the touch"
	});
    }
    cell_open = 0;
    if (!laurana || !living(laurana)) {
	laurana = clone_object("obj/monster");
	laurana->set_name("laurana");
	laurana->set_alias("laurana");
	laurana->set_alt_name("elf");
	laurana->set_short("Laurana, an elven prisoner");
	laurana->set_long(
"A strikingly beautiful elven woman with golden hair and green\n" +
"eyes that blaze with defiance. Despite her imprisonment, she\n" +
"carries herself with the dignity of elven royalty. Her clothes\n" +
"are tattered but were once the finest Qualinesti silk.\n");
	laurana->set_level(12);
	laurana->set_hp(138);
	laurana->set_al(300);
	laurana->set_aggressive(0);
	move_object(laurana, this_object());
	laurana->set_match(this_object(),
	    ({ "laurana_talk" }),
	    ({ "says:" }),
	    ({ 0 })
	);
	laurana->load_chat(10, ({
	    "Laurana says: Verminaard must be stopped.\n",
	    "Laurana paces her cell restlessly.\n",
	    "Laurana says: The Dragon Highlord is in the east tower summit.\n"
	}));
    }
    /* Load the quest controller */
    if (!find_object("obj/dl_pax_quest"))
	call_other("obj/dl_pax_quest", "reset", 0);
}

void init() {
    ::init();
    add_action("use_key", "use");
    add_action("use_key", "unlock");
}

int use_key(string str) {
    object controller;
    object player;
    object key;
    string a, b;

    if (!str)
	return 0;

    if (sscanf(str, "%skey%s", a, b) != 2 &&
	sscanf(str, "%scell%s", a, b) != 2 &&
	sscanf(str, "%slock%s", a, b) != 2)
	return 0;

    player = this_player();
    key = present("key", player);
    if (!key)
	key = present("gate key", player);

    if (!key) {
	write("You don't have a key.\n");
	return 1;
    }

    controller = find_object("obj/dl_pax_quest");
    if (!controller || !controller->is_quest_player(player)) {
	write("The key doesn't seem to fit this lock.\n");
	return 1;
    }

    if (controller->query_stage() < 1 || controller->query_stage() >= 3) {
	write("The key doesn't seem to fit this lock.\n");
	return 1;
    }

    /* Unlock the cell */
    destruct(key);
    cell_open = 1;
    controller->set_stage(3);

    write(
"You insert the heavy iron key into the lock and turn it with a\n" +
"satisfying click. The cell door swings open!\n");
    say(player->query_name() +
	" unlocks the prison cell! The door swings open!\n", player);

    tell_object(player,
"Laurana says: At last! Thank you, brave warrior. I am Laurana\n" +
"of Qualinesti. Verminaard, the Dragon Highlord, holds this\n" +
"fortress. He must be defeated or the prisoners will never be\n" +
"truly free. He dwells at the summit of the east tower.\n" +
"Go -- confront him. I will rally the other prisoners.\n");

    return 1;
}

void laurana_talk(string str) {
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
	sscanf(rest, "%skey%s", a, b) != 2 &&
	sscanf(rest, "%sfree%s", a, b) != 2 &&
	sscanf(rest, "%shi%s", a, b) != 2)
	return;

    controller = find_object("obj/dl_pax_quest");
    if (!controller)
	return;

    if (cell_open || (controller->is_quest_player(player) &&
		      controller->query_stage() >= 3)) {
	tell_object(player,
"Laurana says: Verminaard awaits at the summit of the east tower.\n" +
"Defeat him and this fortress will be liberated!\n");
	return;
    }

    tell_object(player,
"Laurana says: Please, find the key to this cell! Check the\n" +
"armory in the courtyard. Hurry!\n");
}
