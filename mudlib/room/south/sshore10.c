/*
 * Shore of Crescent Lake room 10 - converted to inherit style for grotto exit.
 */

inherit "room/room";

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "The shore of Crescent Lake";
	long_desc =
"You are standing on the shore of Crescent Lake, a beautiful and\n" +
"clear lake. Out in the centre of the lake stands the Isle\n" +
"of the Magi.\n" +
"A dark opening in the rocky bank leads into a cave beneath the shore.\n" +
"The shore of Crescent Lake continues east and west.\n";
	dest_dir = ({
	    "room/south/sshore9", "east",
	    "room/south/sshore11", "west",
	    "room/south/grotto", "cave"
	});
	items = ({
	    "cave", "A dark opening in the rocky bank, barely large enough to enter.\nA foul smell wafts from within",
	    "opening", "A dark opening in the rocky bank, barely large enough to enter.\nA foul smell wafts from within"
	});
    }
}
