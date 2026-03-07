inherit "room/room";

void reset(int arg) {
    if (arg) return;
    set_light(1);
    short_desc = "A large open plain";
    long_desc = "A large open plain. There is a mountain to the north.\n" +
	"Stone stairs lead down to an underground station.\n";
    dest_dir = ({
	"room/plane6", "south",
	"room/mount_pass", "north",
	"room/plane13", "east",
	"room/plane12", "west",
	"room/station_plains", "down"
    });
}
