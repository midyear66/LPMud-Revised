/*
 * Xak Tsaroth Underground 1 - Cavern below the well.
 */

inherit "room/room";

object kapak;

void reset(int arg) {
    if (!arg) {
	set_light(0);
	short_desc = "Underground passage";
	long_desc =
"You are in a damp underground passage carved from living rock.\n" +
"The walls glisten with moisture and phosphorescent fungi cast\n" +
"an eerie blue-green glow. The passage was once a grand corridor\n" +
"-- remnants of tile flooring and carved reliefs are still visible.\n" +
"A rope hangs from a shaft above, leading back up. The passage\n" +
"continues south deeper underground, and a side tunnel branches\n" +
"east.\n";
	dest_dir = ({
	    "room/dragonlance/xak_tsaroth/well", "up",
	    "room/dragonlance/xak_tsaroth/under2", "south",
	    "room/dragonlance/xak_tsaroth/collapse", "east"
	});
	items = ({
	    "fungi", "Phosphorescent fungi growing on the damp walls",
	    "reliefs", "Carved reliefs depicting scenes of worship, now crumbling",
	    "rope", "A rope hanging from the shaft above",
	    "walls", "Damp rock walls glistening with moisture"
	});
    }
    if (!kapak || !living(kapak)) {
	kapak = clone_object("obj/monster");
	kapak->set_name("kapak");
	kapak->set_alias("draconian kapak");
	kapak->set_alt_name("draconian");
	kapak->set_short("A draconian kapak");
	kapak->set_long(
"A sleek, copper-scaled draconian with venomous fangs and a sinuous\n" +
"build. Unlike its larger baaz cousins, the kapak is fast and deadly,\n" +
"its claws dripping with a paralytic venom. It moves with serpentine\n" +
"grace through the underground passages.\n");
	kapak->set_level(8);
	kapak->set_hp(106);
	kapak->set_al(-250);
	kapak->set_wc(10);
	kapak->set_ac(2);
	kapak->set_aggressive(1);
	kapak->load_a_chat(25, ({
	    "The kapak's claws drip with venom!\n",
	    "The draconian kapak hisses and strikes!\n",
	    "The kapak lashes with its venomous tail!\n"
	}));
	move_object(kapak, this_object());
    }
}
