/*
 * obj/sign.c - Wooden sign displaying the underground rail schedule.
 * Cloned into each station room and the mine cart.
 */

string schedule;

void reset(int arg) {
    schedule = "UNDERGROUND RAIL: Village - Eastroad Inn - Plains Crossing - Orc Valley\n" +
	"Full circuit: approximately one minute. Cart pauses 10 seconds at each stop.\n" +
	"Commands: 'board' to enter the cart, 'exit' to leave at a station.\n";
}

string short() {
    return "A wooden sign";
}

int id(string str) {
    return str == "sign" || str == "wooden sign" || str == "schedule";
}

void long(string str) {
    write(schedule);
}

int get() {
    write("The sign is bolted to the wall.\n");
    return 0;
}

void init() {
    add_action("do_read", "read");
}

int do_read(string str) {
    if (!str || !id(str)) return 0;
    write(schedule);
    return 1;
}
