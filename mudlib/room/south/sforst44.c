object monster;

void reset(int started)
{
    if (!started)
	set_light(1);
    if (!monster || !living(monster)) {
	object money;
	monster = clone_object("obj/monster");
	monster->set_name("boar");
	monster->set_alias("wild boar");
	monster->set_level(8);
	monster->set_hp(90);
	monster->set_wc(11);
	monster->set_ac(2);
	monster->set_al(-10);
	monster->set_aggressive(1);
	monster->set_short("A wild boar");
	monster->set_long(
"A massive wild boar with coarse bristly fur and long curved\n" +
"tusks. It paws at the ground and snorts aggressively.\n");
	move_object(monster, this_object());
	money = clone_object("obj/money");
	money->set_money(random(20));
	move_object(money, monster);
    }
}

void init()
{
    add_action("north", "north");
    add_action("east", "east");
    add_action("west", "west");
}

string short()
{
    return "A dimly lit forest";
}

void long()
{
    write("You are in part of a dimly lit forest. The undergrowth is\n" +
	  "trampled and torn, as if something large has been rooting here.\n" +
	  "Trails lead north, east and west.\n");
}

int north()
{
    this_player()->move_player("north#room/south/sforst38");
    return 1;
}

int east()
{
    this_player()->move_player("east#room/south/sforst43");
    return 1;
}

int west()
{
    this_player()->move_player("west#room/south/sforst45");
    return 1;
}
