/*
 * Pax Tharkas Great Hall - Dark cleric lurks here.
 */

inherit "room/room";

object cleric;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "The great hall";
	long_desc =
"A massive hall with vaulted ceilings supported by pillars carved\n" +
"to resemble intertwined elven and dwarven warriors -- a reminder\n" +
"of the fortress's original purpose as a symbol of unity. The\n" +
"Dragonarmy has defaced the carvings and hung dark banners from\n" +
"the pillars. A dark altar has been erected at one end, dedicated\n" +
"to Takhisis. Stairs lead up to the upper levels. The courtyard\n" +
"lies to the south.\n";
	dest_dir = ({
	    "room/dragonlance/pax_tharkas/court", "south",
	    "room/dragonlance/pax_tharkas/upper", "up"
	});
	items = ({
	    "pillars", "Carved pillars depicting elven and dwarven warriors, now defaced",
	    "banners", "Dark banners bearing the five-headed dragon emblem",
	    "altar", "A dark altar dedicated to Takhisis, stained with blood",
	    "carvings", "The original carvings show elves and dwarves working together"
	});
    }
    if (!cleric || !living(cleric)) {
	cleric = clone_object("obj/monster");
	cleric->set_name("cleric");
	cleric->set_alias("dark cleric");
	cleric->set_short("A dark cleric");
	cleric->set_long(
"A robed figure kneeling before the dark altar. The cleric wears\n" +
"black robes emblazoned with the symbol of Takhisis and carries\n" +
"a mace of black iron. Dark energy crackles around his fingertips\n" +
"as he chants unholy prayers.\n");
	cleric->set_level(11);
	cleric->set_hp(130);
	cleric->set_al(-300);
	cleric->set_wc(10);
	cleric->set_ac(2);
	cleric->set_aggressive(1);
	cleric->set_chance(30);
	cleric->set_spell_dam(18);
	cleric->set_spell_mess1(
"The dark cleric hurls a bolt of dark energy!\n");
	cleric->set_spell_mess2(
"Dark energy sears through your body!\n");
	cleric->load_a_chat(25, ({
	    "The dark cleric chants: By Takhisis, destroyer of souls!\n",
	    "The dark cleric's eyes glow with unholy power!\n",
	    "The dark cleric swings his iron mace!\n"
	}));
	move_object(cleric, this_object());
    }
}
