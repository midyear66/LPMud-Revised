/*
 * Temple Nave - Grand nave of the Temple of the Dark Queen.
 */

inherit "room/room";

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "The temple nave";
	long_desc =
"An enormous vaulted nave stretches before you, its ceiling lost\n" +
"in shadow hundreds of feet above. Massive pillars of black marble\n" +
"line the central aisle, each carved to resemble a different color\n" +
"of chromatic dragon -- red, blue, green, black, and white. Dark\n" +
"flames burn in iron braziers, casting flickering shadows that\n" +
"dance like living things. The nave branches in several directions:\n" +
"west and east wings, north to an altar, down to crypts below,\n" +
"and up to an observation gallery.\n";
	dest_dir = ({
	    "room/dragonlance/temple/entry", "south",
	    "room/dragonlance/temple/west", "west",
	    "room/dragonlance/temple/east", "east",
	    "room/dragonlance/temple/altar", "north",
	    "room/dragonlance/temple/crypt", "down",
	    "room/dragonlance/temple/gallery", "up"
	});
	items = ({
	    "pillars", "Five massive dragon pillars -- red, blue, green, black, and white",
	    "ceiling", "Lost in shadow, impossibly far above",
	    "braziers", "Iron braziers burning with dark, unnatural flames",
	    "shadows", "The shadows seem to move with a life of their own"
	});
    }
}
