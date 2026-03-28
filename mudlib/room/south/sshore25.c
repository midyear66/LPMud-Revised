object monster;

void reset(int started)
{
    if (!started)
	set_light(1);
    if (!monster || !living(monster)) {
	object money;
	monster = clone_object("obj/monster");
	monster->set_name("troll");
	monster->set_alias("shore troll");
	monster->set_level(13);
	monster->set_hp(150);
	monster->set_wc(16);
	monster->set_ac(3);
	monster->set_al(-300);
	monster->set_aggressive(1);
	monster->set_short("A shore troll");
	monster->set_long(
"A squat, muscular troll with greenish skin covered in lake\n" +
"weed. It has made a crude shelter from driftwood and bones\n" +
"near the water's edge.\n");
	move_object(monster, this_object());
	money = clone_object("obj/money");
	money->set_money(random(90));
	move_object(money, monster);
    }
}

void init()
{
    add_action("north", "north");
    add_action("east", "east");
    add_action("northwest", "northwest");
    add_action("southeast", "southeast");
}

string short()
{
    return "The shore of Crescent Lake";
}

void long()
{
    write("You are standing on the shore of Crescent Lake. A crude shelter\n" +
	  "of driftwood and bones stands near the water's edge.\n" +
	  "Trails lead into the forest to the north and east.\n" +
	  "The shore of Crescent Lake continues northwest and southeast.\n");
}

int north()
{
    this_player()->move_player("north#room/south/sforst49");
    return 1;
}

int east()
{
    this_player()->move_player("east#room/south/sforst46");
    return 1;
}

int northwest()
{
    this_player()->move_player("northwest#room/south/sshore24");
    return 1;
}

int southeast()
{
    this_player()->move_player("southeast#room/south/sshore26");
    return 1;
}
