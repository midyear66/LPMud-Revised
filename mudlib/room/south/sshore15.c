object monster;

void reset(int started)
{
    if (!started)
	set_light(1);
    if (!monster || !living(monster)) {
	object money, scale;
	monster = clone_object("obj/monster");
	monster->set_name("serpent");
	monster->set_alias("lake serpent");
	monster->set_level(14);
	monster->set_hp(180);
	monster->set_wc(18);
	monster->set_ac(3);
	monster->set_al(-200);
	monster->set_aggressive(1);
	monster->set_spell_chance(20);
	monster->set_spell_dam(25);
	monster->set_spell_mesg(
"The lake serpent coils around you and squeezes!\n");
	monster->set_short("A lake serpent");
	monster->set_long(
"A massive serpent with iridescent blue-green scales that has\n" +
"slithered up from the depths of Crescent Lake. Its body is as\n" +
"thick as a tree trunk and its fangs drip with venom.\n");
	move_object(monster, this_object());
	money = clone_object("obj/money");
	money->set_money(random(120));
	move_object(money, monster);
	scale = clone_object("obj/treasure");
	scale->set_id("scale");
	scale->set_alias("serpent scale");
	scale->set_short("A serpent scale");
	scale->set_long(
"A large iridescent scale from a lake serpent.\n");
	scale->set_value(200);
	scale->set_weight(1);
	move_object(scale, monster);
    }
}

void init()
{
    add_action("northwest", "northwest");
    add_action("south", "south");
    add_action("west", "west");
    add_action("southeast", "southeast");
}

string short()
{
    return "The shore of Crescent Lake";
}

void long()
{
    write("You are standing on the shore of Crescent Lake, a beautiful and\n" +
	  "clear lake. Out in the centre of the lake stands the Isle\n" +
	  "of the Magi. The water here churns with dark movement.\n" +
	  "Trails lead into the forest to the south and west.\n" +
	  "The shore of Crescent Lake continues northwest and southeast.\n");
}

int northwest()
{
    this_player()->move_player("northwest#room/south/sshore16");
    return 1;
}

int south()
{
    this_player()->move_player("south#room/south/sforst41");
    return 1;
}

int southeast()
{
    this_player()->move_player("southeast#room/south/sshore14");
    return 1;
}

int west()
{
    this_player()->move_player("west#room/south/sforst40");
    return 1;
}
