/*
 * Xak Tsaroth Collapsed Passage - Side tunnel with minor treasure.
 */

inherit "room/room";

void reset(int arg) {
    if (!arg) {
	set_light(0);
	short_desc = "A collapsed passage";
	long_desc =
"This side tunnel has partially collapsed, filling the far end with\n" +
"rubble. Among the fallen stones, you can see fragments of ancient\n" +
"pottery and the glint of scattered coins. The bones of some\n" +
"unfortunate soul lie half-buried in the debris. The main passage\n" +
"lies to the west.\n";
	dest_dir = ({
	    "room/dragonlance/xak_tsaroth/under1", "west"
	});
	items = ({
	    "rubble", "Fallen stones blocking the passage further east",
	    "pottery", "Fragments of ancient pottery, once finely crafted",
	    "coins", "A few coins glinting among the rubble",
	    "bones", "The bones of some unfortunate explorer, long dead"
	});
    }
    /* Scatter some coins here */
    if (!present("coins")) {
	object coins;
	coins = clone_object("obj/money");
	coins->set_money(50);
	move_object(coins, this_object());
    }
}
