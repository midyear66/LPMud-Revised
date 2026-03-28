object monster;

void reset(int started)
{
    if (!started)
	set_light(1);
    if (!monster || !living(monster)) {
	object money;
	monster = clone_object("obj/monster");
	monster->set_name("wolf");
	monster->set_alias("dark wolf");
	monster->set_level(10);
	monster->set_hp(110);
	monster->set_wc(13);
	monster->set_ac(2);
	monster->set_al(-100);
	monster->set_aggressive(1);
	monster->set_short("A dark wolf");
	monster->set_long(
"A large wolf with jet-black fur and glowing amber eyes.\n" +
"It moves with unnatural silence through the shadows.\n");
	move_object(monster, this_object());
	money = clone_object("obj/money");
	money->set_money(random(40));
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
    write("You are in part of a dimly lit forest. Claw marks score the\n" +
	  "bark of nearby trees and a low growl echoes through the shadows.\n" +
	  "Trails lead north, east and west.\n");
}

int north()
{
    this_player()->move_player("north#room/south/sshore8");
    return 1;
}

int east()
{
    this_player()->move_player("east#room/south/sforst18");
    return 1;
}

int west()
{
    this_player()->move_player("west#room/south/sshore9");
    return 1;
}
