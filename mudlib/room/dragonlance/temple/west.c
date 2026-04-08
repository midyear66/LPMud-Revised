/*
 * Temple West Wing - Shadow priests dwell here.
 */

inherit "room/room";

object priest;

void reset(int arg) {
    if (!arg) {
	set_light(0);
	short_desc = "West wing";
	long_desc =
"The west wing of the temple is shrouded in unnatural darkness.\n" +
"Rows of prayer alcoves line the walls, each containing a small\n" +
"shrine to Takhisis. The air is thick with incense smoke and the\n" +
"whispered prayers of the dark faithful. Shadows pool in the\n" +
"corners like liquid darkness. The nave lies to the east.\n";
	dest_dir = ({
	    "room/dragonlance/temple/nave", "east"
	});
	items = ({
	    "alcoves", "Prayer alcoves containing shrines to Takhisis",
	    "shrines", "Small shrines bearing the five-headed dragon symbol",
	    "incense", "Thick incense smoke that stings the eyes",
	    "shadows", "Shadows that seem unnaturally deep and alive"
	});
    }
    if (!priest || !living(priest)) {
	priest = clone_object("obj/monster");
	priest->set_name("priest");
	priest->set_alias("shadow priest");
	priest->set_short("A shadow priest");
	priest->set_long(
"A gaunt figure in robes of deepest black, their face hidden\n" +
"within a shadowed hood. Dark energy crackles between their\n" +
"fingers as they chant prayers to Takhisis. The shadows around\n" +
"them seem to obey their will.\n");
	priest->set_level(15);
	priest->set_hp(162);
	priest->set_al(-400);
	priest->set_wc(14);
	priest->set_ac(3);
	priest->set_aggressive(1);
	priest->set_chance(30);
	priest->set_spell_dam(20);
	priest->set_spell_mess1(
"The shadow priest hurls a bolt of pure darkness!\n");
	priest->set_spell_mess2(
"Pure shadow strikes you like a physical blow!\n");
	priest->load_a_chat(25, ({
	    "The shadow priest hisses: The Dark Queen devours all!\n",
	    "Shadows writhe and lash out at your command!\n",
	    "The shadow priest chants in a language of pure darkness!\n"
	}));
	move_object(priest, this_object());
    }
}
