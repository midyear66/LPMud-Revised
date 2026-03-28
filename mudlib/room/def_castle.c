/*
 * Castle facade template - generated into players/<name>/castle.c
 * by master_create_wizard().
 *
 * Predefined strings (prepended by master_create_wizard):
 *   NAME         - wizard name (e.g. "peace")
 *   DEST         - room where the castle was dropped
 *   DIRECTION    - direction to enter (e.g. "south")
 *   CASTLE_SHORT - custom description (e.g. "a long bridge into a marsh")
 *   OPEN         - 1 = gates open, 0 = gates closed
 *
 * To open your castle, edit the defines at the top of your castle.c:
 *   1. Set DIRECTION to an unused exit direction of the drop room
 *   2. Set CASTLE_SHORT to your custom description
 *   3. Set OPEN to 1
 *   4. Update/destruct the castle object to reload
 */

#ifndef NAME
#  define NAME "Nobody"
#endif

#ifndef DEST
#  define DEST "/room/church"
#endif

#ifndef DIRECTION
#  define DIRECTION "south"
#endif

#ifndef CASTLE_SHORT
#  define CASTLE_SHORT "the castle of " + capitalize(NAME)
#endif

#ifndef OPEN
#  define OPEN 0
#endif

string workroom_path;

int id(string str) { return str == "castle"; }

string short() {
    if (OPEN)
	return "To the " + DIRECTION + " is " + CASTLE_SHORT;
    return "The castle of " + capitalize(NAME) + " (gates closed)";
}

void long() {
    if (OPEN) {
	write("To the " + DIRECTION + " is " + CASTLE_SHORT + ".\n");
	write("You can go " + DIRECTION + " to enter.\n");
    } else {
	write("This is the castle of " + capitalize(NAME) + ".\n");
	write("The gates are closed.\n");
    }
}

void init() {
    string *exits;
    int i, conflict;
    add_action("enter_castle", "enter");
    /* Check if room already has this direction */
    if (environment()) {
	exits = environment()->query_dest_dir();
	if (exits) {
	    i = 1;
	    while (i < sizeof(exits)) {
		if (exits[i] == DIRECTION)
		    conflict = 1;
		i += 2;
	    }
	}
    }
    if (!conflict)
	add_action("enter_castle", DIRECTION);
}

int enter_castle(string str) {
    string verb;
    verb = query_verb();
    /* For "enter", require "enter castle" */
    if (verb == "enter" && str != "castle")
	return 0;
    if (!OPEN) {
	write("The gates of " + capitalize(NAME) +
	    "'s castle are closed.\n");
	return 1;
    }
    if (file_size("/" + workroom_path + ".c") < 1) {
	write("The castle gates are open but the interior is bare.\n");
	return 1;
    }
    this_player()->move_player(DIRECTION + "#" + workroom_path);
    return 1;
}

void reset(int arg) {
    if (arg)
	return;
    workroom_path = "players/" + NAME + "/workroom";
    move_object(this_object(), DEST);
}
