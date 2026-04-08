/*
 * Xak Tsaroth Plaza - Central plaza of the ruined city.
 */

inherit "room/room";

object draconian;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "A devastated plaza";
	long_desc =
"You stand in what was once the grand central plaza of Xak Tsaroth.\n" +
"A broken fountain dominates the center, its basin cracked and dry.\n" +
"Rubble from collapsed buildings is piled everywhere. To the west,\n" +
"the shell of a great temple still stands. To the south, a stone\n" +
"well is visible among the ruins. To the east, a half-collapsed\n" +
"great hall leans dangerously. The city gates lie to the north.\n";
	dest_dir = ({
	    "room/dragonlance/xak_tsaroth/gate", "north",
	    "room/dragonlance/xak_tsaroth/temple", "west",
	    "room/dragonlance/xak_tsaroth/well", "south",
	    "room/dragonlance/xak_tsaroth/hall", "east"
	});
	items = ({
	    "fountain", "A broken fountain -- its stone basin depicts scenes of the old gods",
	    "rubble", "Mountains of rubble from collapsed buildings",
	    "temple", "The shell of a great temple to the west, still partially standing",
	    "well", "A stone well to the south, its rim worn smooth by centuries of use"
	});
    }
    if (!draconian || !living(draconian)) {
	draconian = clone_object("obj/monster");
	draconian->set_name("draconian");
	draconian->set_alias("draconian soldier");
	draconian->set_short("A draconian soldier");
	draconian->set_long(
"A draconian patrol leader with darker scales and a cruel whip\n" +
"coiled at its belt alongside a heavy mace.\n");
	draconian->set_level(6);
	draconian->set_hp(88);
	draconian->set_al(-200);
	draconian->set_wc(8);
	draconian->set_ac(1);
	draconian->set_aggressive(1);
	move_object(draconian, this_object());
    }
}
