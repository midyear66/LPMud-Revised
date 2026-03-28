object monster;

void reset(int started)
{
    if (!started)
	set_light(1);
    if (!monster || !living(monster)) {
	object money;
	monster = clone_object("obj/monster");
	monster->set_name("ghoul");
	monster->set_alias("marsh ghoul");
	monster->set_level(11);
	monster->set_hp(120);
	monster->set_wc(14);
	monster->set_ac(2);
	monster->set_al(-600);
	monster->set_aggressive(1);
	monster->set_short("A marsh ghoul");
	monster->set_long(
"A gaunt, waterlogged corpse that has clawed its way out of the\n" +
"marshy lakeshore. Its skin is grey and bloated, and it moves\n" +
"with a horrible lurching gait.\n");
	move_object(monster, this_object());
	money = clone_object("obj/money");
	money->set_money(random(70));
	move_object(money, monster);
    }
}

void init()
{
    add_action("north", "north");
    add_action("west", "west");
    add_action("northeast", "northeast");
    add_action("southwest", "southwest");
}

string short()
{
    return "The shore of Crescent Lake";
}

void long()
{
    write("You are standing on the shore of Crescent Lake. The ground\n" +
	  "here is marshy and waterlogged, and a foul smell rises from\n" +
	  "the mud. Trails lead into the forest to the north and west.\n" +
	  "The shore of Crescent Lake continues northeast and southwest.\n");
}

int north()
{
    this_player()->move_player("north#room/south/sforst28");
    return 1;
}

int west()
{
    this_player()->move_player("west#room/south/sforst30");
    return 1;
}

int northeast()
{
    this_player()->move_player("northeast#room/south/sshore21");
    return 1;
}

int southwest()
{
    this_player()->move_player("southwest#room/south/sshore19");
    return 1;
}
