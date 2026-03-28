/*
 * Familiar Orb - wizard tool for conjuring and controlling a familiar.
 *
 * Any wizard (level 20+) can clone this orb. Non-wizards cannot pick
 * it up. Dropping the orb destroys it and any active familiar.
 *
 * Commands:
 *   orb help               - Show help
 *   familiar create <desc> - Conjure a familiar
 *   familiar look          - See through the familiar's eyes
 *   familiar move <dir>    - Move the familiar
 *   familiar say <msg>     - Speak through the familiar
 *   familiar emote <msg>   - Emote through the familiar
 *   familiar send <room>   - Teleport familiar to a room path
 *   familiar recall        - Return familiar to your side
 *   familiar where         - Show familiar's location
 *   familiar mute          - Toggle message relay
 *   familiar destroy       - Dismiss the familiar
 */

inherit "obj/weapon";

object my_familiar;

void reset(int arg) {
    if (arg) return;
    ::reset(0);
    set_name("orb");
    set_alias("familiar orb");
    set_alt_name("familiar");
    set_short("a swirling familiar orb");
    set_long(
"A sphere of dark glass in which faint shapes swirl and shift.\n" +
"It pulses gently with arcane energy, waiting for a wizard's command.\n" +
"Try 'orb help' to learn its powers.\n");
    set_class(2);
    set_weight(1);
    set_value(100);
}

int get() {
    if (this_player() && this_player()->query_level() < 20) {
	write("The orb burns your hand and you drop it!\n");
	call_out("remove_object", 0);
	return 0;
    }
    return 1;
}

void remove_object() {
    destruct(this_object());
}

int drop(int silently) {
    if (!silently) {
	write("The familiar orb shatters on the ground!\n");
	say("A familiar orb shatters into glittering fragments.\n");
    }
    if (my_familiar && living(my_familiar)) {
	say(my_familiar->short() + " vanishes in a puff of smoke.\n");
	destruct(my_familiar);
	my_familiar = 0;
    }
    call_out("remove_object", 0);
    return 0;
}

void init() {
    ::init();
    add_action("do_orb", "orb");
    add_action("do_familiar", "familiar");
    add_action("do_familiar", "fm");
}

int do_orb(string str) {
    string cmd, arg;

    if (environment(this_object()) != this_player()) {
	write("You need to be carrying the orb.\n");
	return 1;
    }

    if (!str) {
	write("Usage: orb help\n");
	return 1;
    }

    if (sscanf(str, "%s %s", cmd, arg) != 2)
	cmd = str;

    if (cmd == "help") {
	write(
"=== Familiar Orb ===\n" +
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
"  familiar destroy        - Dismiss the familiar\n" +
"\n" +
"Warning: dropping the orb will destroy it and your familiar!\n");
	return 1;
    }

    write("Unknown orb command: " + cmd + "\n");
    write("Try: orb help\n");
    return 1;
}

int do_familiar(string str) {
    string cmd, arg;

    if (environment(this_object()) != this_player()) {
	write("You need to be carrying the orb.\n");
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
	my_familiar = clone_object("obj/familiar");
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
