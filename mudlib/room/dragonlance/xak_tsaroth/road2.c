/*
 * Xak Tsaroth road 2 - Closer to the ruined city, draconians patrol.
 */

inherit "room/room";

object draconian;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "A crumbling road";
	long_desc =
"The ancient road crumbles further as it approaches the ruins of what\n" +
"was once a great city. Toppled columns and shattered statues litter\n" +
"the roadside. Claw marks score the remaining stonework, and the\n" +
"ground is scorched black in places. Something large and reptilian\n" +
"has been through here recently. The ruined city gates lie to the\n" +
"south.\n";
	dest_dir = ({
	    "room/dragonlance/xak_tsaroth/road1", "north",
	    "room/dragonlance/xak_tsaroth/gate", "south"
	});
	items = ({
	    "columns", "Toppled marble columns, once part of a grand colonnade",
	    "statues", "Shattered statues -- they may have depicted the old gods",
	    "marks", "Deep claw marks scorched into the stone -- dragonmarks",
	    "ground", "Scorched black earth, as if by dragonfire"
	});
    }
    if (!draconian || !living(draconian)) {
	draconian = clone_object("obj/monster");
	draconian->set_name("draconian");
	draconian->set_alias("draconian soldier");
	draconian->set_short("A draconian soldier");
	draconian->set_long(
"A hulking reptilian humanoid standing over seven feet tall, with\n" +
"bronze-scaled skin, leathery wings folded against its back, and a\n" +
"cruel, intelligent face. It wears dark armor emblazoned with a\n" +
"dragon emblem and carries a heavy curved sword.\n");
	draconian->set_level(6);
	draconian->set_hp(88);
	draconian->set_al(-200);
	draconian->set_wc(8);
	draconian->set_ac(1);
	draconian->set_aggressive(1);
	draconian->load_a_chat(25, ({
	    "The draconian hisses: Intruder! For the Dark Queen!\n",
	    "The draconian slashes with its curved sword!\n",
	    "The draconian's wings flare as it lunges forward!\n"
	}));
	move_object(draconian, this_object());
    }
}
