object monster;

void reset(int started)
{
    if (!started)
	set_light(1);
    if (!monster || !living(monster)) {
	object money;
	monster = clone_object("obj/monster");
	monster->set_name("ogre");
	monster->set_alias("forest ogre");
	monster->set_level(12);
	monster->set_hp(140);
	monster->set_wc(16);
	monster->set_ac(2);
	monster->set_al(-300);
	monster->set_aggressive(1);
	monster->set_short("A forest ogre");
	monster->set_long(
"A towering ogre with mottled green skin and a crude wooden\n" +
"club. It reeks of rotting meat and swamp water.\n");
	move_object(monster, this_object());
	money = clone_object("obj/money");
	money->set_money(random(80));
	move_object(money, monster);
    }
}

void init()
{
    add_action("north", "north");
    add_action("south", "south");
    add_action("east", "east");
}

string short()
{
    return "A dimly lit forest";
}

void long()
{
    write("You are in part of a dimly lit forest. Broken branches and\n" +
	  "trampled undergrowth mark the passage of something very large.\n" +
	  "Trails lead north, south and east.\n");
}

int north()
{
    this_player()->move_player("north#room/south/sforst29");
    return 1;
}

int south()
{
    this_player()->move_player("south#room/south/sshore19");
    return 1;
}

int east()
{
    this_player()->move_player("east#room/south/sshore20");
    return 1;
}
