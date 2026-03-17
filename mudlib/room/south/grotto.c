/*
 * Corrupted Grotto - boss room for the Fairy Plague quest.
 * Dark room beneath the lakeshore. Player pours purifying draught
 * on the cursed runestone, spawning the Corrupted Sprite.
 */

inherit "room/room";

object sprite;

void reset(int arg) {
    if (!arg) {
	set_light(0);
	short_desc = "A corrupted grotto";
	long_desc =
"A dank cavern beneath the lakeshore. Dark water pools on the uneven\n" +
"floor, giving off a sickly greenish glow. In the center of the grotto\n" +
"sits a large runestone covered in pulsing dark runes. Tendrils of\n" +
"black corruption spread outward from the stone like veins, creeping\n" +
"along the walls and into the rock above. The air is thick with the\n" +
"stench of decay and tainted magic.\n" +
"The way out leads back to the lakeshore.\n";
	dest_dir = ({
	    "room/south/sshore10", "out"
	});
	items = ({
	    "runestone", "A large stone covered in dark, pulsing runes. Black tendrils\nof corruption radiate outward from it, poisoning everything they touch.\nThis is the source of the fairy plague",
	    "runes", "Dark symbols that pulse with a sickly green light.\nThey seem to writhe and shift when you look at them directly",
	    "water", "Dark water pools on the floor, tainted with a greenish glow.\nIt smells foul and corrupted",
	    "pool", "Dark water pools on the floor, tainted with a greenish glow.\nIt smells foul and corrupted",
	    "tendrils", "Black tendrils of corruption spread from the runestone like\ndiseased veins, burrowing into the rock and soil above",
	    "walls", "The cavern walls are slick with moisture and streaked with\nblack corruption spreading from the runestone"
	});
    }
    /* Load the quest controller so it exists */
    if (!find_object("obj/fairy_quest"))
	call_other("obj/fairy_quest", "reset", 0);
}

void init() {
    ::init();
    add_action("use_draught", "pour");
    add_action("use_draught", "use");
}

int use_draught(string str) {
    object controller;
    object player;
    object draught;
    string a, b;

    if (!str)
	return 0;

    /* Match "pour draught", "pour purifying draught", "use vial", etc */
    if (sscanf(str, "%sdraught%s", a, b) != 2 &&
	sscanf(str, "%svial%s", a, b) != 2 &&
	sscanf(str, "%spurifying%s", a, b) != 2)
	return 0;

    player = this_player();
    draught = present("draught", player);
    if (!draught)
	draught = present("vial", player);
    if (!draught)
	draught = present("purifying draught", player);

    if (!draught) {
	write("You don't have the purifying draught.\n");
	return 1;
    }

    controller = find_object("obj/fairy_quest");
    if (!controller) {
	write("Nothing happens.\n");
	return 1;
    }

    if (!controller->is_quest_player(player) ||
	controller->query_stage() != 4) {
	write("Nothing happens.\n");
	return 1;
    }

    /* Pour the draught on the runestone */
    write(
"You pour the purifying draught over the cursed runestone.\n" +
"The iridescent liquid sizzles and flares with blinding white light!\n" +
"The dark runes shriek and crack, and the black tendrils writhe in agony.\n" +
"From the dark pool, a massive shape surges upward -- the Corrupted Sprite!\n");
    say(player->query_name() +
	" pours a glowing draught on the runestone. The cavern erupts in light!\n" +
	"A terrible creature rises from the dark water!\n", player);

    destruct(draught);
    controller->set_stage(5);

    /* Spawn the Corrupted Sprite */
    sprite = clone_object("obj/monster");
    sprite->set_name("sprite");
    sprite->set_alias("corrupted sprite");
    sprite->set_alt_name("water sprite");
    sprite->set_race("fairy");
    sprite->set_level(8);
    sprite->set_hp(120);
    sprite->set_al(-300);
    sprite->set_ac(2);
    sprite->set_wc(14);
    sprite->set_short("The Corrupted Sprite");
    sprite->set_long(
"Once a guardian spirit of Crescent Lake, this water sprite has been\n" +
"twisted beyond recognition by the cursed runestone. Its translucent\n" +
"body writhes with dark corruption, and its eyes burn with mindless\n" +
"malice. Tendrils of poisonous magic lash from its form.\n");
    sprite->set_aggressive(1);
    sprite->set_chance(20);
    sprite->set_spell_dam(8);
    sprite->set_spell_mess1(
	"The Corrupted Sprite sprays you with a jet of poisonous water!\n");
    sprite->set_spell_mess2(
	"The Corrupted Sprite sprays a jet of poisonous water at ");
    sprite->load_a_chat(40, ({
	"The Corrupted Sprite shrieks with an inhuman wail!\n",
	"The Corrupted Sprite hisses: This place is MINE now!\n",
	"Dark water swirls violently around the Corrupted Sprite.\n",
	"The Corrupted Sprite lashes out with tendrils of tainted magic!\n"
    }));
    sprite->set_dead_ob(this_object());
    move_object(sprite, this_object());

    return 1;
}

void monster_died(object killed) {
    object controller;

    controller = find_object("obj/fairy_quest");
    if (!controller)
	return;

    if (controller->query_stage() == 5) {
	controller->set_stage(6);
	tell_room(this_object(),
"As the corrupted sprite dissolves into clean water, the cursed\n" +
"runestone cracks and shatters with a thunderous boom!\n" +
"The dark tendrils wither and crumble to dust. The sickly glow fades\n" +
"from the pool, replaced by clear, pure water. A fresh breeze flows\n" +
"through the grotto, carrying the scent of wildflowers.\n" +
"The corruption has been cleansed. You should return to the fairy elder.\n");
    }
    sprite = 0;
}
