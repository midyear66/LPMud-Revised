/*
 * Temple East Wing - Death knight guards here.
 */

inherit "room/room";

object knight;

void reset(int arg) {
    if (!arg) {
	set_light(0);
	short_desc = "East wing";
	long_desc =
"The east wing serves as a guardroom for the temple's most fearsome\n" +
"defender. Weapon racks line the walls, holding cursed blades that\n" +
"seem to drink the light. A suit of armor stands empty in the\n" +
"corner -- or is it empty? The eye slits of the helmet glow with\n" +
"a faint, unholy fire. The nave lies to the west.\n";
	dest_dir = ({
	    "room/dragonlance/temple/nave", "west"
	});
	items = ({
	    "racks", "Weapon racks holding cursed blades that seem to absorb light",
	    "blades", "Dark blades that hum with malevolent energy",
	    "armor", "A suit of ancient armor with glowing eye slits -- is it alive?",
	    "helmet", "The eye slits glow with unholy fire"
	});
    }
    if (!knight || !living(knight)) {
	knight = clone_object("obj/monster");
	knight->set_name("knight");
	knight->set_alias("death knight");
	knight->set_short("A death knight");
	knight->set_long(
"An undead knight in ancient, blackened armor that seethes with\n" +
"dark power. Unholy fire burns in the eye slits of its helm, and\n" +
"it wields a two-handed sword of black iron. Once a Knight of\n" +
"Solamnia, this warrior fell to darkness and now serves Takhisis\n" +
"in undeath for eternity.\n");
	knight->set_level(17);
	knight->set_hp(178);
	knight->set_al(-500);
	knight->set_wc(18);
	knight->set_ac(4);
	knight->set_aggressive(1);
	knight->set_race("undead");
	knight->load_a_chat(25, ({
	    "The death knight's sword blazes with dark fire!\n",
	    "The death knight says: I serve the Queen of Darkness... forever.\n",
	    "The death knight strikes with terrifying, mechanical precision!\n",
	    "Unholy fire flares from the death knight's helm!\n"
	}));
	move_object(knight, this_object());
    }
}
