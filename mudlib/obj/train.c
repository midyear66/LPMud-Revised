/*
 * obj/train.c - Underground mine cart transport
 *
 * Cycles between 4 underground stations on a 60-second loop.
 * 10 seconds dwell at each station, 5 seconds travel between.
 *
 * Route: Village -> Eastroad Inn -> Plains Crossing -> Orc Valley -> repeat
 */

inherit "room/room";

string *stations;
string *station_names;
int current_station;
int is_docked;

void arrive();
void depart();

string query_docked_at() {
    if (is_docked)
	return stations[current_station];
    return 0;
}

string query_next_station() {
    int next;
    next = (current_station + 1) % sizeof(stations);
    return station_names[next];
}

void reset(int arg) {
    if (arg) return;

    stations = ({
	"room/station",
	"room/station_east",
	"room/station_plains",
	"room/station_orc"
    });

    station_names = ({
	"Village",
	"Eastroad Inn",
	"Plains Crossing",
	"Orc Valley"
    });

    set_light(1);
    short_desc = "Mine cart";
    current_station = 0;
    is_docked = 0;

    items = ({
	"cart", "A heavy iron mine cart with wooden bench seats, riding on steel rails",
	"rails", "Steel rails stretch into the dark tunnels ahead and behind",
	"tracks", "Steel rails stretch into the dark tunnels ahead and behind"
    });

    if (!present("sign")) {
	move_object(clone_object("obj/sign"), this_object());
    }

    arrive();
}

void init() {
    ::init();
    add_action("do_exit", "out");
    add_action("do_exit", "exit");
    add_action("do_exit", "disembark");
}

int do_exit(string str) {
    if (!is_docked) {
	write("The cart is moving! You'll have to wait for the next stop.\n");
	return 1;
    }
    this_player()->move_player("out#" + stations[current_station]);
    return 1;
}

void long(string str) {
    int next;

    if (str) {
	::long(str);
	return;
    }

    if (is_docked) {
	write("You are riding in a heavy iron mine cart on steel rails. The cart\n" +
	      "is stopped at " + station_names[current_station] + " Station.\n" +
	      "A wooden sign shows the schedule. You can go 'out' to the platform.\n");
	write("    There is one obvious exit: out\n");
    } else {
	next = (current_station + 1) % sizeof(stations);
	write("You are riding in a heavy iron mine cart rattling through dark\n" +
	      "tunnels. Sparks fly from the wheels on the iron rails.\n" +
	      "A wooden sign shows the schedule.\n" +
	      "Next stop: " + station_names[next] + " Station.\n");
	write("    No obvious exits.\n");
    }
}

void arrive() {
    object station_room;

    is_docked = 1;
    dest_dir = ({ stations[current_station], "out" });

    tell_room(this_object(),
	"The mine cart grinds to a halt at " +
	station_names[current_station] + " Station.\n" +
	"You may go 'out' to the platform. Departing in 10 seconds.\n");

    station_room = find_object(stations[current_station]);
    if (station_room)
	tell_room(station_room,
	    "A mine cart arrives with a screech of metal on stone.\n");

    call_out("depart", 10);
}

void depart() {
    object station_room;
    int next;

    station_room = find_object(stations[current_station]);

    is_docked = 0;
    dest_dir = 0;

    next = (current_station + 1) % sizeof(stations);

    tell_room(this_object(),
	"With a jolt, the mine cart begins to move.\n" +
	"Next stop: " + station_names[next] + " Station.\n");

    if (station_room)
	tell_room(station_room,
	    "The mine cart departs into the dark tunnel.\n");

    current_station = next;
    call_out("arrive", 5);
}
