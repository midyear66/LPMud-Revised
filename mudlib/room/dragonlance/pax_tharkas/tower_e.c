/*
 * Pax Tharkas East Tower - Contains portal to Temple (level 16+ gate).
 */

inherit "room/room";

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "East tower";
	long_desc =
"The east tower of Pax Tharkas. A spiral staircase leads up to\n" +
"the summit where the Dragon Highlord holds court. In a shadowed\n" +
"alcove, a shimmering portal of darkness pulses with otherworldly\n" +
"energy -- a gateway to the Temple of the Dark Queen herself.\n" +
"The upper corridor lies to the west.\n";
	dest_dir = ({
	    "room/dragonlance/pax_tharkas/upper", "west",
	    "room/dragonlance/pax_tharkas/summit", "up"
	});
	items = ({
	    "staircase", "A spiral stone staircase leading up to the summit",
	    "portal", "A shimmering portal of darkness, pulsing with dark energy",
	    "alcove", "A shadowed alcove housing the dark portal"
	});
    }
}

void init() {
    ::init();
    add_action("enter_portal", "enter");
}

int enter_portal(string str) {
    if (!str || str != "portal")
	return 0;

    if (this_player()->query_level() < 16) {
	write(
"You step toward the dark portal, but a wave of crushing energy\n" +
"slams into you, hurling you backward! The portal's dark power\n" +
"is too great for you to withstand. You must grow stronger before\n" +
"you can pass through.\n");
	return 1;
    }

    write(
"You steel yourself and step into the shimmering darkness. The\n" +
"world dissolves around you in a whirl of shadow and flame...\n");
    say(this_player()->query_name() +
	" steps into the dark portal and vanishes!\n", this_player());
    this_player()->move_player(
	"through the portal#room/dragonlance/temple/entry");
    return 1;
}
