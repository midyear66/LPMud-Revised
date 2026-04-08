/*
 * Pax Tharkas Summit - Dragon Highlord Verminaard boss room.
 * Quest 3 boss fight. Verminaard spawns for quest players.
 */

inherit "room/room";

object verminaard;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "Tower summit";
	long_desc =
"The summit of the east tower, open to the cold mountain sky.\n" +
"A throne of black iron sits upon a raised dais, and dark banners\n" +
"snap in the fierce wind. From here, you can see the mountain\n" +
"passes stretching in all directions. This is where the Dragon\n" +
"Highlord Verminaard commands his army. The stairs lead down.\n";
	dest_dir = ({
	    "room/dragonlance/pax_tharkas/tower_e", "down"
	});
	items = ({
	    "throne", "A throne of black iron upon a raised dais",
	    "banners", "Dark banners bearing the Dragonarmy emblem",
	    "dais", "A raised stone platform holding the iron throne",
	    "passes", "Mountain passes stretching to the horizon"
	});
    }
    /* Load the quest controller */
    if (!find_object("obj/dl_pax_quest"))
	call_other("obj/dl_pax_quest", "reset", 0);
}

void init() {
    ::init();
    call_out("check_quest_spawn", 1);
}

void check_quest_spawn() {
    object controller;
    object player;

    if (verminaard && living(verminaard))
	return;

    controller = find_object("obj/dl_pax_quest");
    if (!controller)
	return;

    player = controller->query_player();
    if (!player || environment(player) != this_object())
	return;

    if (controller->query_stage() == 3) {
	controller->set_stage(4);

	tell_room(this_object(),
"A thunderous voice booms across the summit:\n" +
"'You DARE enter my domain?!'\n" +
"From behind the iron throne, a massive figure rises -- Dragon\n" +
"Highlord Verminaard, clad in black dragonscale armor, wielding\n" +
"a nightblack mace called Nightbringer. His dragon helm conceals\n" +
"his face, but his eyes burn with fanatical rage!\n");

	verminaard = clone_object("obj/monster");
	verminaard->set_name("verminaard");
	verminaard->set_alias("dragon highlord");
	verminaard->set_alt_name("highlord");
	verminaard->set_race("human");
	verminaard->set_level(15);
	verminaard->set_hp(200);
	verminaard->set_al(-500);
	verminaard->set_wc(18);
	verminaard->set_ac(4);
	verminaard->set_short("Dragon Highlord Verminaard");
	verminaard->set_long(
"Dragon Highlord Verminaard, one of the most feared commanders of\n" +
"the Dragonarmy. He is a massive man in black dragonscale armor,\n" +
"wielding the legendary mace Nightbringer. His horned dragon helm\n" +
"conceals his face, and dark divine power crackles around him --\n" +
"he is both a warrior and a cleric of Takhisis.\n");
	verminaard->set_aggressive(1);
	verminaard->set_chance(25);
	verminaard->set_spell_dam(25);
	verminaard->set_spell_mess1(
"Verminaard calls down a pillar of dark fire!\n");
	verminaard->set_spell_mess2(
"Dark fire engulfs you as Verminaard invokes Takhisis!\n");
	verminaard->load_a_chat(30, ({
	    "Verminaard roars: Takhisis, grant me strength to crush this insect!\n",
	    "Verminaard swings Nightbringer in a devastating arc!\n",
	    "Verminaard says: You cannot defeat a servant of the Dark Queen!\n",
	    "Dark energy crackles around Verminaard's mace!\n"
	}));
	verminaard->set_dead_ob(this_object());
	move_object(verminaard, this_object());
    }
}

void monster_died(object killed) {
    object controller;

    controller = find_object("obj/dl_pax_quest");
    if (!controller)
	return;

    if (controller->query_stage() == 4) {
	controller->set_stage(5);
	tell_room(this_object(),
"Verminaard crashes to the ground, Nightbringer clattering from\n" +
"his lifeless hand. As his body falls, a great cheer rises from\n" +
"the courtyard below -- the prisoners can sense their oppressor\n" +
"is defeated! The dark banners on the summit tear loose in the\n" +
"wind and blow away. Pax Tharkas is liberated!\n" +
"Return to Elistan in the prison to complete the liberation.\n");
    }
    verminaard = 0;
}
