/*
 * Crypt passage - middle room of the Restless Dead quest.
 * Dark room with a skeleton archer and an inscription hint.
 */

inherit "room/room";

void reset(int arg) {
    object skel, weapon;

    if (!arg) {
	set_light(0);
	short_desc = "Crypt passage";
	long_desc =
"A long passage stretches through the crypt, its ceiling low and oppressive.\n" +
"The walls are carved with faded reliefs depicting robed figures performing\n" +
"some kind of ritual. An ancient inscription is chiseled into the east wall.\n" +
"The passage continues north toward the entrance and south toward a\n" +
"heavy stone doorway.\n";
	dest_dir = ({
	    "room/crypt/entrance", "north",
	    "room/crypt/tomb", "south"
	});
	items = ({
	    "inscription", "The inscription reads: 'Only water blessed by the faithful\ncan break the ward that binds the sorcerer's tomb. Pour it upon the\nrunes and face what rises within.'",
	    "reliefs", "Faded carvings of robed figures surrounding a glowing sarcophagus",
	    "walls", "Ancient stone walls covered in faded carvings and reliefs",
	    "doorway", "A heavy stone doorway to the south, partially ajar"
	});
    }
    if (!present("skeleton")) {
	skel = clone_object("obj/monster");
	skel->set_name("skeleton");
	skel->set_alias("skeleton archer");
	skel->set_alt_name("archer");
	skel->set_race("undead");
	skel->set_level(5);
	skel->set_hp(50);
	skel->set_al(-100);
	skel->set_short("A skeleton archer");
	skel->set_long("A skeletal figure crouches in the shadows, clutching a bow made of\nbleached bone. Its hollow eyes track your every movement.\n");
	skel->set_aggressive(1);
	skel->load_a_chat(25, ({
	    "The skeleton archer nocks a bone arrow with eerie precision.\n",
	    "The skeleton archer's jaw clacks in a ghastly grin.\n",
	    "The skeleton archer looses an arrow with a dry snap.\n"
	}));
	weapon = clone_object("obj/weapon");
	weapon->set_name("bone bow");
	weapon->set_class(12);
	weapon->set_value(10);
	weapon->set_weight(2);
	transfer(weapon, skel);
	command("wield bone bow", skel);
	move_object(skel, this_object());
    }
}
