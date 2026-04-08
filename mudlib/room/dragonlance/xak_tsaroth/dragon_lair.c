/*
 * Xak Tsaroth Dragon Lair - Lair of Khisanth the black dragon.
 * Quest 2 boss room. Dragon spawns when quest player enters.
 */

inherit "room/room";

object dragon;

void reset(int arg) {
    if (!arg) {
	set_light(0);
	short_desc = "The dragon's lair";
	long_desc =
"An enormous cavern opens before you, its ceiling lost in darkness\n" +
"high above. The floor is covered with a thick layer of coins,\n" +
"gems, and corroded treasures -- the hoard of a dragon. The walls\n" +
"are scarred with acid burns and deep claw marks. Bones of the\n" +
"dragon's victims are scattered among the treasure. At the far\n" +
"end of the cavern, a raised stone platform holds something that\n" +
"gleams with a pale, holy light -- the Disks of Mishakal.\n" +
"The passage leads north.\n";
	dest_dir = ({
	    "room/dragonlance/xak_tsaroth/under2", "north"
	});
	items = ({
	    "hoard", "A vast dragon's hoard of coins, gems, and corroded treasures",
	    "bones", "The scattered bones of many victims",
	    "platform", "A raised stone platform at the far end of the cavern",
	    "disks", "Platinum tablets that gleam with a pale, holy light",
	    "walls", "Walls scarred with acid burns and enormous claw marks"
	});
    }
    /* Load the quest controller */
    if (!find_object("obj/dl_disks_quest"))
	call_other("obj/dl_disks_quest", "reset", 0);
}

void init() {
    ::init();
    /* Check if we should spawn the dragon for a quest player */
    call_out("check_quest_spawn", 1);
}

void check_quest_spawn() {
    object controller;
    object player;

    if (dragon && living(dragon))
	return;

    controller = find_object("obj/dl_disks_quest");
    if (!controller)
	return;

    player = controller->query_player();
    if (!player || environment(player) != this_object())
	return;

    if (controller->query_stage() == 2) {
	controller->set_stage(3);

	tell_room(this_object(),
"A thunderous roar shakes the cavern! From the deepest shadows,\n" +
"an enormous black dragon surges forward, acid dripping from\n" +
"her massive jaws. Khisanth, the black dragon of Xak Tsaroth,\n" +
"has awakened!\n");

	dragon = clone_object("obj/monster");
	dragon->set_name("khisanth");
	dragon->set_alias("black dragon");
	dragon->set_alt_name("dragon");
	dragon->set_race("dragon");
	dragon->set_level(10);
	dragon->set_hp(150);
	dragon->set_al(-500);
	dragon->set_wc(14);
	dragon->set_ac(3);
	dragon->set_short("Khisanth the black dragon");
	dragon->set_long(
"An enormous black dragon, her scales glistening like polished\n" +
"obsidian. Acid drips from her massive jaws, hissing where it\n" +
"strikes the stone floor. Her eyes burn with ancient malice and\n" +
"cruel intelligence. She is Khisanth, guardian of the Disks of\n" +
"Mishakal, and she does not suffer intruders.\n");
	dragon->set_aggressive(1);
	dragon->set_chance(20);
	dragon->set_spell_dam(15);
	dragon->set_spell_mess1(
"Khisanth breathes a stream of burning acid!\n");
	dragon->set_spell_mess2(
"Burning acid splashes across your body!\n");
	dragon->load_a_chat(30, ({
	    "Khisanth roars: You dare enter my domain, mortal?!\n",
	    "Khisanth lashes her massive tail across the cavern!\n",
	    "Acid drips from Khisanth's jaws, hissing on the stone!\n",
	    "Khisanth says: The Disks are MINE! You will die here!\n"
	}));
	dragon->set_dead_ob(this_object());
	move_object(dragon, this_object());
    }
}

void monster_died(object killed) {
    object controller;
    object disks;

    controller = find_object("obj/dl_disks_quest");
    if (!controller)
	return;

    if (controller->query_stage() == 3) {
	controller->set_stage(4);

	disks = clone_object("obj/treasure");
	disks->set_id("disks");
	disks->set_alias("disks of mishakal");
	disks->set_short("The Disks of Mishakal");
	disks->set_long(
"A set of platinum tablets inscribed with the holy words of\n" +
"Mishakal, goddess of healing. The tablets glow with a warm,\n" +
"steady light and feel surprisingly light in your hands.\n" +
"These are the legendary Disks of Mishakal, containing the\n" +
"wisdom of the true gods.\n");
	disks->set_value(1000);
	disks->set_weight(3);
	move_object(disks, this_object());

	tell_room(this_object(),
"As Khisanth crashes to the ground, her massive body dissolves\n" +
"into a pool of hissing acid. The Disks of Mishakal on the stone\n" +
"platform blaze with brilliant white light, free at last from\n" +
"the dragon's dark influence. The platinum tablets lie gleaming\n" +
"on the ground, ready to be taken.\n");
    }
    dragon = 0;
}
