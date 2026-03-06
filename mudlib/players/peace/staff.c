/*
 * Peace's wizard staff - a formidable weapon with teleport commands.
 */

inherit "obj/weapon";

object my_familiar;

void reset(int arg) {
    if (arg) return;
    ::reset(0);
    set_name("staff");
    set_alias("wizard staff");
    set_alt_name("peace staff");
    set_short("Peace's wizard staff");
    set_long(
"A tall staff of dark ironwood, topped with a faintly glowing crystal.\n" +
"It hums with arcane energy. It is a formidable weapon.\n" +
"Try 'staff help' to learn its powers.\n");
    set_class(15);
    set_weight(3);
    set_value(500);
}

void init() {
    ::init();
    add_action("do_church", "church");
    add_action("do_workshop", "workshop");
    add_action("do_familiar", "familiar");
    add_action("do_familiar", "fm");
    add_action("do_staff", "staff");
}

int do_staff(string str) {
    string cmd, arg;

    if (environment(this_object()) != this_player()) {
	write("You need to be carrying the staff.\n");
	return 1;
    }

    if (!str) {
	write("Usage: staff <help> [args]\n");
	return 1;
    }

    if (sscanf(str, "%s %s", cmd, arg) != 2)
	cmd = str;

    if (cmd == "help") {
	write(
"=== Peace's Wizard Staff ===\n" +
"Teleport commands:\n" +
"  church          - Teleport to the church\n" +
"  workshop        - Teleport to your workshop\n" +
"\n" +
"Staff commands:\n" +
"  staff help      - Show this help\n" +
"\n" +
"Familiar commands (also available as 'fm'):\n" +
"  familiar create <desc>  - Conjure a familiar with given description\n" +
"  familiar look           - See through your familiar's eyes\n" +
"  familiar move <dir>     - Move the familiar in a direction\n" +
"  familiar say <msg>      - Speak through the familiar\n" +
"  familiar emote <msg>    - Emote through the familiar\n" +
"  familiar send <room>    - Teleport familiar to a room path\n" +
"  familiar recall         - Return familiar to your side\n" +
"  familiar where          - Show familiar's location\n" +
"  familiar mute           - Toggle message relay on/off\n" +
"  familiar destroy        - Dismiss the familiar\n");
	return 1;
    }

    write("Unknown staff command: " + cmd + "\n");
    write("Commands: help\n");
    return 1;
}

int do_church(string str) {
    if (environment(this_object()) != this_player()) {
	write("You need to be carrying the staff.\n");
	return 1;
    }
    write("The crystal atop your staff flashes and you are transported!\n");
    say(this_player()->query_name() + " vanishes in a flash of light.\n");
    this_player()->move_player("X#room/church");
    return 1;
}

int do_workshop(string str) {
    if (environment(this_object()) != this_player()) {
	write("You need to be carrying the staff.\n");
	return 1;
    }
    write("The crystal atop your staff flashes and you are transported!\n");
    say(this_player()->query_name() + " vanishes in a flash of light.\n");
    this_player()->move_player("X#players/peace/workshop");
    return 1;
}

int do_familiar(string str) {
    string cmd, arg;

    if (environment(this_object()) != this_player()) {
	write("You need to be carrying the staff.\n");
	return 1;
    }

    if (!str) {
	write("Usage: familiar <create|move|look|say|emote|send|recall|where|mute|destroy> [args]\n");
	return 1;
    }

    if (sscanf(str, "%s %s", cmd, arg) != 2)
	cmd = str;

    if (cmd == "create") {
	if (my_familiar && living(my_familiar)) {
	    write("You already have a familiar. Destroy it first.\n");
	    return 1;
	}
	if (!arg) {
	    write("Usage: familiar create <description>\n");
	    return 1;
	}
	my_familiar = clone_object("players/peace/familiar");
	my_familiar->set_wizard(this_player());
	my_familiar->set_familiar_desc(arg);
	move_object(my_familiar, environment(this_player()));
	write("You conjure " + arg + " from thin air.\n");
	say("A " + arg + " appears out of nowhere.\n");
	return 1;
    }

    if (!my_familiar || !living(my_familiar)) {
	write("You don't have a familiar. Use 'familiar create <desc>' first.\n");
	my_familiar = 0;
	return 1;
    }

    if (cmd == "move") {
	if (!arg) {
	    write("Move which direction?\n");
	    return 1;
	}
	my_familiar->familiar_move(arg);
	tell_object(this_player(), "[Familiar] " + my_familiar->familiar_look());
	return 1;
    }

    if (cmd == "look") {
	tell_object(this_player(), "[Familiar] " + my_familiar->familiar_look());
	return 1;
    }

    if (cmd == "say") {
	if (!arg) {
	    write("Say what?\n");
	    return 1;
	}
	my_familiar->familiar_say(arg);
	write("Your familiar says: " + arg + "\n");
	return 1;
    }

    if (cmd == "emote") {
	if (!arg) {
	    write("Emote what?\n");
	    return 1;
	}
	my_familiar->familiar_emote(arg);
	write("Your familiar " + arg + "\n");
	return 1;
    }

    if (cmd == "send") {
	if (!arg) {
	    write("Send where? Provide a room path.\n");
	    return 1;
	}
	move_object(my_familiar, arg);
	write("You send your familiar to " + arg + ".\n");
	tell_object(this_player(), "[Familiar] " + my_familiar->familiar_look());
	return 1;
    }

    if (cmd == "recall") {
	move_object(my_familiar, environment(this_player()));
	write("Your familiar returns to your side.\n");
	return 1;
    }

    if (cmd == "where") {
	string room_path;
	object env;
	env = environment(my_familiar);
	if (env) {
	    room_path = object_name(env);
	    write("Your familiar is in: " + room_path + " (" + env->short() + ")\n");
	} else {
	    write("Your familiar is nowhere.\n");
	}
	return 1;
    }

    if (cmd == "mute") {
	my_familiar->toggle_relay();
	if (my_familiar->query_relay())
	    write("Familiar relay is now ON.\n");
	else
	    write("Familiar relay is now OFF.\n");
	return 1;
    }

    if (cmd == "destroy") {
	write("Your familiar vanishes in a puff of smoke.\n");
	say(my_familiar->short() + " vanishes in a puff of smoke.\n",
	    my_familiar);
	destruct(my_familiar);
	my_familiar = 0;
	return 1;
    }

    write("Unknown familiar command: " + cmd + "\n");
    write("Commands: create, move, look, say, emote, send, recall, where, mute, destroy\n");
    return 1;
}
