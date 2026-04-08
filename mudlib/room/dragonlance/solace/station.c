/*
 * Solace Station - Underground rail stop beneath the vallenwood gate.
 */

inherit "room/room";

void reset(int arg) {
    if (arg) return;
    set_light(1);
    short_desc = "Solace Station";
    dest_dir = ({ "room/dragonlance/solace/gate", "up" });
    items = ({
	"tracks", "Steel rails run along a channel in the floor, disappearing into dark tunnels",
	"rails", "Steel rails run along a channel in the floor, disappearing into dark tunnels",
	"platform", "A stone platform runs alongside the tracks",
	"roots", "Massive vallenwood roots arch overhead, forming a natural ceiling"
    });
    if (!present("sign")) {
	move_object(clone_object("obj/sign"), this_object());
    }
}

void init() {
    ::init();
    add_action("do_board", "board");
    add_action("do_board", "enter");
}

int do_board(string str) {
    object train;
    string docked;

    train = find_object("obj/train");
    if (!train) {
	write("There is no mine cart here. Wait for it to arrive.\n");
	return 1;
    }

    docked = train->query_docked_at();
    if (docked != object_name(this_object())) {
	write("There is no mine cart here right now. Wait for it to arrive.\n");
	return 1;
    }

    say(this_player()->query_name() + " climbs into the mine cart.\n");
    this_player()->move_player("board#obj/train");
    return 1;
}

void long(string str) {
    object train;
    string docked;

    if (str) {
	::long(str);
	return;
    }

    write("You are in an underground station carved from rock beneath the great\n" +
	  "vallenwood trees. Massive roots arch overhead like natural pillars.\n" +
	  "Steel rails run along a channel in the floor, disappearing into dark\n" +
	  "tunnels. A wooden sign shows the rail schedule. Stone stairs lead up\n" +
	  "to the gate of Solace.\n");

    train = find_object("obj/train");
    if (train) {
	docked = train->query_docked_at();
	if (docked == object_name(this_object()))
	    write("A heavy iron mine cart sits on the rails. You can 'board' it.\n");
    }

    write("    There is one obvious exit: up\n");
}
