/*
 * Temple Throne Room - Avatar of Takhisis boss room.
 * Quest 4 final boss. Avatar spawns when quest player enters.
 */

inherit "room/room";

object avatar;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "Throne of the Dark Queen";
	long_desc =
"The throne room of Takhisis, the Queen of Darkness. A colossal\n" +
"throne of black iron and dragon bone sits upon a dais of dark\n" +
"crystal. Behind it, a swirling portal of shadow and fire tears\n" +
"a hole between the mortal world and the Abyss itself. The five\n" +
"chromatic dragon heads that form the throne's back seem alive,\n" +
"their jeweled eyes blazing with malevolent intelligence.\n" +
"Dark power saturates every stone. The corridor lies to the south.\n";
	dest_dir = ({
	    "room/dragonlance/temple/corridor", "south"
	});
	items = ({
	    "throne", "A colossal throne of black iron and dragon bone",
	    "portal", "A swirling vortex of shadow and fire -- a gateway to the Abyss",
	    "dais", "A raised platform of dark crystal that pulses with power",
	    "heads", "Five dragon heads forming the throne's back, each a different color",
	    "eyes", "Jeweled eyes that blaze with terrible intelligence"
	});
    }
    /* Load the quest controller */
    if (!find_object("obj/dl_takhisis_quest"))
	call_other("obj/dl_takhisis_quest", "reset", 0);
}

void init() {
    ::init();
    call_out("check_quest_spawn", 1);
}

void check_quest_spawn() {
    object controller;
    object player;

    if (avatar && living(avatar))
	return;

    controller = find_object("obj/dl_takhisis_quest");
    if (!controller)
	return;

    player = controller->query_player();
    if (!player || environment(player) != this_object())
	return;

    if (controller->query_stage() >= 2 && controller->query_stage() < 4) {
	controller->set_stage(3);

	tell_room(this_object(),
"The portal behind the throne blazes with terrible light!\n" +
"A massive shape surges through -- five dragon heads, each a\n" +
"different color, attached to an enormous scaled body. It is\n" +
"the Avatar of Takhisis, the Dark Queen herself, manifesting\n" +
"in the mortal world! Her five heads roar in unison, shaking\n" +
"the very foundations of the temple!\n");

	avatar = clone_object("obj/monster");
	avatar->set_name("takhisis");
	avatar->set_alias("avatar of takhisis");
	avatar->set_alt_name("avatar");
	avatar->set_race("dragon");
	avatar->set_level(20);
	avatar->set_hp(250);
	avatar->set_al(-1000);
	avatar->set_wc(22);
	avatar->set_ac(5);
	avatar->set_short("The Avatar of Takhisis");
	avatar->set_long(
"The Avatar of Takhisis, Queen of Darkness, in her true form:\n" +
"an enormous five-headed chromatic dragon. Each head is a\n" +
"different color -- red, blue, green, black, and white -- and\n" +
"each breathes a different element of destruction. Her massive\n" +
"body fills the throne room, scales shimmering with dark\n" +
"rainbow light. Her presence is overwhelming, a weight of\n" +
"pure evil that threatens to crush your very will.\n");
	avatar->set_aggressive(1);
	avatar->set_chance(35);
	avatar->set_spell_dam(30);
	avatar->set_spell_mess1(
"All five of Takhisis's heads breathe destruction at once!\n");
	avatar->set_spell_mess2(
"Fire, lightning, acid, frost, and poison engulf you!\n");
	avatar->load_a_chat(40, ({
	    "The red head breathes a torrent of fire!\n",
	    "The blue head crackles with devastating lightning!\n",
	    "The black head spews a stream of burning acid!\n",
	    "The white head exhales a blast of killing frost!\n",
	    "Takhisis roars: I AM THE QUEEN OF DARKNESS! KNEEL OR PERISH!\n",
	    "The green head exhales a cloud of poisonous gas!\n",
	    "All five heads snap and tear at you simultaneously!\n"
	}));
	avatar->set_dead_ob(this_object());
	move_object(avatar, this_object());
    }
}

void monster_died(object killed) {
    object controller;

    controller = find_object("obj/dl_takhisis_quest");
    if (!controller)
	return;

    if (controller->query_stage() == 3) {
	controller->set_stage(4);
	tell_room(this_object(),
"With a final, earth-shaking roar, the Avatar of Takhisis\n" +
"dissolves into shadow and flame! The portal behind the throne\n" +
"screams and implodes, collapsing in on itself with a\n" +
"thunderous boom! The dark crystal dais cracks and shatters!\n\n" +
"For a moment, absolute silence fills the chamber. Then warm,\n" +
"golden light begins to seep through the cracks in the walls.\n" +
"The dark enchantments are broken. The temple of the Dark Queen\n" +
"is no more.\n\n" +
"Return to Fizban at the temple entrance.\n");
    }
    avatar = 0;
}
