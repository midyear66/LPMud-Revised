/*
 * Default wizard workroom template - generated into players/<name>/workroom.c
 * by master_create_wizard().
 *
 * Predefined strings (prepended by master_create_wizard):
 *   NAME - wizard name
 *   DEST - room outside the castle (where to go on "out")
 */

#ifndef NAME
#  define NAME "Nobody"
#endif

#ifndef DEST
#  define DEST "/room/church"
#endif

inherit "room/room";

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "Workroom of " + capitalize(NAME);
	long_desc =
"This is the workroom of " + capitalize(NAME) + ". It is sparsely\n" +
"furnished with a rough wooden desk and a few shelves. A doorway\n" +
"leads back outside.\n";
	dest_dir = ({ DEST, "out" });
    }
}
