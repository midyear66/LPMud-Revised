/*
 * Temple Sanctum - Inner sanctum, dark guardian guards.
 */

inherit "room/room";

object guardian;

void reset(int arg) {
    if (!arg) {
	set_light(0);
	short_desc = "The inner sanctum";
	long_desc =
"Beyond the shattered barrier lies the inner sanctum of the\n" +
"temple. The walls pulse with dark energy, and reality itself\n" +
"seems to warp and twist. The floor is a mosaic depicting\n" +
"Takhisis in her five-headed dragon form, and her eyes seem\n" +
"to glow with malevolent awareness. A corridor leads north\n" +
"toward the throne room. The altar chamber lies to the south.\n";
	dest_dir = ({
	    "room/dragonlance/temple/altar", "south",
	    "room/dragonlance/temple/corridor", "north"
	});
	items = ({
	    "walls", "The walls pulse with dark energy, warping your vision",
	    "mosaic", "A floor mosaic of Takhisis in her five-headed dragon form",
	    "eyes", "The mosaic eyes glow with terrible awareness -- she is watching"
	});
    }
    if (!guardian || !living(guardian)) {
	guardian = clone_object("obj/monster");
	guardian->set_name("guardian");
	guardian->set_alias("dark guardian");
	guardian->set_short("A dark guardian");
	guardian->set_long(
"A towering figure of living shadow and dark fire, bound in\n" +
"enchanted armor of black dragonscale. It has no face -- only\n" +
"two burning points of crimson light where its eyes should be.\n" +
"It exists only to guard the path to the Dark Queen's throne,\n" +
"and it will destroy any who dare approach.\n");
	guardian->set_level(18);
	guardian->set_hp(186);
	guardian->set_al(-500);
	guardian->set_wc(18);
	guardian->set_ac(4);
	guardian->set_aggressive(1);
	guardian->set_chance(25);
	guardian->set_spell_dam(22);
	guardian->set_spell_mess1(
"The dark guardian unleashes a wave of dark flame!\n");
	guardian->set_spell_mess2(
"Dark flame engulfs you, burning body and soul!\n");
	guardian->load_a_chat(25, ({
	    "The dark guardian's eyes blaze crimson!\n",
	    "Shadow and flame swirl around the dark guardian!\n",
	    "The guardian strikes with inhuman speed!\n"
	}));
	move_object(guardian, this_object());
    }
}
