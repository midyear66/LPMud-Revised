/*
 * Tomb room - boss room of the Restless Dead quest.
 * Dark room with Valdris's sarcophagus. Player uses holy water to
 * break the ward, spawning the Wraith of Valdris.
 */

inherit "room/room";

object wraith;

void reset(int arg) {
    if (!arg) {
	set_light(0);
	short_desc = "Valdris's tomb";
	long_desc =
"A large circular chamber deep beneath the cemetery. In the center stands\n" +
"a massive stone sarcophagus covered in glowing runes that pulse with\n" +
"a sickly violet light. The air is bitterly cold and carries the stench\n" +
"of ancient death. Dark energy seems to radiate from the sarcophagus,\n" +
"making the shadows writhe and twist.\n" +
"The only exit leads north back to the passage.\n";
	dest_dir = ({
	    "room/crypt/passage", "north"
	});
	items = ({
	    "sarcophagus", "A massive stone sarcophagus etched with glowing runes.\nThe runes pulse with violet light -- a magical ward seals the lid shut",
	    "runes", "Arcane symbols that glow with a sickly violet light.\nThey form a protective ward over the sarcophagus",
	    "shadows", "The shadows seem alive, writhing and twisting unnaturally"
	});
    }
    /* Load the quest controller so it exists */
    if (!find_object("obj/crypt_quest"))
	call_other("obj/crypt_quest", "reset", 0);
}

void init() {
    ::init();
    add_action("use_water", "pour");
    add_action("use_water", "use");
}

int use_water(string str) {
    object controller;
    object player;
    object water;
    string a, b;

    if (!str)
	return 0;

    /* Match "pour holy water", "use holy water", "pour water on tomb", etc */
    if (sscanf(str, "%sholy water%s", a, b) != 2 &&
	sscanf(str, "%svial%s", a, b) != 2 &&
	str != "water")
	return 0;

    player = this_player();
    water = present("holy_water", player);
    if (!water)
	water = present("holy water", player);
    if (!water)
	water = present("vial", player);

    if (!water) {
	write("You don't have any holy water.\n");
	return 1;
    }

    controller = find_object("obj/crypt_quest");
    if (!controller) {
	write("Nothing happens.\n");
	return 1;
    }

    if (!controller->is_quest_player(player) ||
	controller->query_stage() != 1) {
	write("Nothing happens.\n");
	return 1;
    }

    /* Break the ward */
    write(
"You pour the holy water over the glowing runes on the sarcophagus.\n" +
"The runes flare brilliant white and then shatter with a thunderous crack!\n" +
"The lid of the sarcophagus slides open and a terrible darkness pours forth.\n" +
"A spectral figure rises from within -- the Wraith of Valdris!\n");
    say(player->query_name() +
	" pours holy water on the sarcophagus. The runes shatter!\n" +
	"A terrible wraith rises from the tomb!\n", player);

    destruct(water);
    controller->set_stage(2);

    /* Spawn the wraith */
    wraith = clone_object("obj/monster");
    wraith->set_name("wraith");
    wraith->set_alias("wraith of valdris");
    wraith->set_alt_name("valdris");
    wraith->set_race("undead");
    wraith->set_level(7);
    wraith->set_hp(100);
    wraith->set_al(-400);
    wraith->set_ac(2);
    wraith->set_short("The Wraith of Valdris");
    wraith->set_long(
"A towering spectral figure cloaked in tattered black robes. Its face is a\n" +
"skull wreathed in violet flame, and its bony hands crackle with dark energy.\n" +
"This is the spirit of Valdris, the sorcerer who was buried here centuries\n" +
"ago, now risen in terrible undeath.\n");
    wraith->set_aggressive(1);
    wraith->set_wc(15);
    wraith->set_chance(25);
    wraith->set_spell_dam(10);
    wraith->set_spell_mess1("The wraith hurls a bolt of dark energy at you!\n");
    wraith->set_spell_mess2("The wraith hurls a bolt of dark energy at ");
    wraith->load_a_chat(40, ({
	"The Wraith of Valdris shrieks: You dare disturb my slumber?!\n",
	"The Wraith of Valdris hisses: I will add your bones to my army!\n",
	"The Wraith of Valdris cackles: No mortal can destroy me!\n",
	"Dark energy swirls around the Wraith of Valdris.\n"
    }));
    wraith->set_dead_ob(this_object());
    move_object(wraith, this_object());

    return 1;
}

void monster_died(object killed) {
    object controller;

    controller = find_object("obj/crypt_quest");
    if (!controller)
	return;

    if (controller->query_stage() == 2) {
	controller->set_stage(3);
	tell_room(this_object(),
"As the wraith dissolves into nothing, the sarcophagus lid grinds shut.\n" +
"The glowing runes fade and the oppressive darkness lifts. A warm\n" +
"breeze flows through the chamber and the crypt grows peaceful.\n" +
"Valdris's spirit has been laid to rest at last.\n");
    }
    wraith = 0;
}
