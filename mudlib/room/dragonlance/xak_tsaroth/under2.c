/*
 * Xak Tsaroth Underground 2 - Deeper cavern approaching dragon lair.
 */

inherit "room/room";

object kapak;

void reset(int arg) {
    if (!arg) {
	set_light(0);
	short_desc = "A deep cavern";
	long_desc =
"The passage opens into a large natural cavern. Stalactites hang\n" +
"from the ceiling like stone fangs, and pools of stagnant water\n" +
"reflect the faint glow of underground fungi. The air is thick\n" +
"with a sharp, acrid smell -- like acid. Deep claw marks score\n" +
"the stone floor, each groove as wide as a man's hand. Something\n" +
"very large lives further south. The passage leads north.\n";
	dest_dir = ({
	    "room/dragonlance/xak_tsaroth/under1", "north",
	    "room/dragonlance/xak_tsaroth/dragon_lair", "south"
	});
	items = ({
	    "stalactites", "Massive stone formations hanging from the cavern ceiling",
	    "pools", "Pools of stagnant water with an oily sheen",
	    "marks", "Deep claw marks scoring the stone -- something enormous made these",
	    "smell", "A sharp, acrid smell like concentrated acid"
	});
    }
    if (!kapak || !living(kapak)) {
	kapak = clone_object("obj/monster");
	kapak->set_name("kapak");
	kapak->set_alias("draconian kapak");
	kapak->set_alt_name("draconian");
	kapak->set_short("A draconian kapak");
	kapak->set_long(
"A draconian kapak with dark copper scales, lurking in the shadows\n" +
"of the cavern. Its venomous fangs glint in the dim light.\n");
	kapak->set_level(8);
	kapak->set_hp(106);
	kapak->set_al(-250);
	kapak->set_wc(10);
	kapak->set_ac(2);
	kapak->set_aggressive(1);
	kapak->load_a_chat(20, ({
	    "The kapak spits venom!\n",
	    "The draconian slashes with poisoned claws!\n"
	}));
	move_object(kapak, this_object());
    }
}
