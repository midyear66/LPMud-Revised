/*
 * Temple Crypt - Catacombs beneath the temple.
 */

inherit "room/room";

object champion;

void reset(int arg) {
    if (!arg) {
	set_light(0);
	short_desc = "Temple catacombs";
	long_desc =
"Dank catacombs stretch beneath the temple, lined with ancient\n" +
"stone sarcophagi. The lids of several have been pushed aside,\n" +
"their occupants risen to serve the Dark Queen. The air is\n" +
"freezing cold and carries the stench of ages-old death. Faint\n" +
"moaning echoes from deeper in the tunnels. Stairs lead up to\n" +
"the nave.\n";
	dest_dir = ({
	    "room/dragonlance/temple/nave", "up"
	});
	items = ({
	    "sarcophagi", "Ancient stone sarcophagi, some open and empty",
	    "tunnels", "Dark tunnels stretching deeper into the earth",
	    "lids", "Heavy stone lids pushed aside by the risen dead"
	});
    }
    if (!champion || !living(champion)) {
	champion = clone_object("obj/monster");
	champion->set_name("champion");
	champion->set_alias("skeletal champion");
	champion->set_short("A skeletal champion");
	champion->set_long(
"An enormous animated skeleton clad in ornate black armor. Unlike\n" +
"common undead, this creature moves with frightening intelligence\n" +
"and skill. Blue flames burn in its eye sockets and it wields a\n" +
"massive battleaxe with practiced ease. It was once a great\n" +
"warrior, now bound to eternal service in undeath.\n");
	champion->set_level(16);
	champion->set_hp(170);
	champion->set_al(-400);
	champion->set_wc(16);
	champion->set_ac(4);
	champion->set_aggressive(1);
	champion->set_race("undead");
	champion->load_a_chat(20, ({
	    "The skeletal champion's eyes blaze with cold blue fire!\n",
	    "The skeleton swings its battleaxe with bone-crushing force!\n",
	    "Bones rattle as the skeletal champion advances!\n"
	}));
	move_object(champion, this_object());
    }
}
