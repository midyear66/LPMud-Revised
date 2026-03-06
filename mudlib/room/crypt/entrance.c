/*
 * Crypt entrance - first underground room of the Restless Dead quest.
 * Dark room with skeleton warriors. Player needs a light source.
 */

inherit "room/room";

void reset(int arg) {
    object skel, weapon;
    int i;

    if (!arg) {
	set_light(0);
	short_desc = "Crypt entrance";
	long_desc =
"A narrow stone stairway opens into a dank underground chamber. The air\n" +
"is cold and stale, carrying the faint smell of decay. Ancient stone\n" +
"walls are lined with alcoves, some still holding crumbling bones.\n" +
"Faint scratching sounds echo from deeper in the crypt to the south.\n" +
"A stairway leads up to the cemetery above.\n";
	dest_dir = ({
	    "room/cemetery", "up",
	    "room/crypt/passage", "south"
	});
	items = ({
	    "alcoves", "Stone alcoves line the walls, holding ancient remains",
	    "bones", "Crumbling bones rest in the wall alcoves, undisturbed for centuries",
	    "walls", "Rough-hewn stone walls, slick with moisture",
	    "stairway", "A narrow stone stairway leads up to the cemetery"
	});
    }
    if (!present("skeleton")) {
	i = 0;
	while (i < 2) {
	    i += 1;
	    skel = clone_object("obj/monster");
	    skel->set_name("skeleton");
	    skel->set_alias("skeleton warrior");
	    skel->set_alt_name("warrior");
	    skel->set_race("undead");
	    skel->set_level(4);
	    skel->set_hp(40);
	    skel->set_al(-100);
	    skel->set_short("A skeleton warrior");
	    skel->set_long("An animated skeleton clad in rusted armor fragments, wielding a\ncorroded sword. Empty eye sockets glow with a faint green light.\n");
	    skel->set_aggressive(1);
	    skel->load_a_chat(25, ({
		"The skeleton warrior rattles its jaw menacingly.\n",
		"The skeleton warrior swings its rusty sword wildly.\n",
		"The skeleton warrior's eyes flare with sickly light.\n"
	    }));
	    weapon = clone_object("obj/weapon");
	    weapon->set_name("rusty sword");
	    weapon->set_class(10);
	    weapon->set_value(5);
	    weapon->set_weight(3);
	    transfer(weapon, skel);
	    command("wield rusty sword", skel);
	    move_object(skel, this_object());
	}
    }
}
