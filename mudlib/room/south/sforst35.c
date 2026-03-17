/*
 * South forest room 35 - converted to inherit style for fairy glade exit.
 */

inherit "room/room";

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "A dimly lit forest";
	long_desc =
"You are in part of a dimly lit forest.\n" +
"Through the trees to the northeast, you glimpse a soft, ethereal glow.\n" +
"Trails lead north, south, east and west.\n";
	dest_dir = ({
	    "room/south/sforst33", "north",
	    "room/south/sforst38", "south",
	    "room/south/sforst34", "east",
	    "room/south/sforst36", "west",
	    "room/south/fairy_glade", "northeast"
	});
    }
}
