object monster;

void reset(int started)
{
    if (!started)
	set_light(1);
    if (!monster || !living(monster)) {
	object money, shard;
	monster = clone_object("obj/monster");
	monster->set_name("stalker");
	monster->set_alias("shadow stalker");
	monster->set_level(13);
	monster->set_hp(160);
	monster->set_wc(17);
	monster->set_ac(3);
	monster->set_al(-500);
	monster->set_aggressive(1);
	monster->set_spell_chance(15);
	monster->set_spell_dam(20);
	monster->set_spell_mesg(
"The shadow stalker reaches into your mind with cold shadow!\n");
	monster->set_short("A shadow stalker");
	monster->set_long(
"A dark, barely visible figure that seems to be made of living\n" +
"shadow. It drifts between the trees with predatory grace,\n" +
"leaving frost on every surface it passes.\n");
	move_object(monster, this_object());
	money = clone_object("obj/money");
	money->set_money(random(100));
	move_object(money, monster);
	shard = clone_object("obj/treasure");
	shard->set_id("shard");
	shard->set_alias("dark shard");
	shard->set_short("A dark shard");
	shard->set_long(
"A fragment of solidified shadow, cold to the touch.\n");
	shard->set_value(150);
	shard->set_weight(1);
	move_object(shard, monster);
    }
}

void init()
{
    add_action("north", "north");
    add_action("east", "east");
}

string short()
{
    return "A dimly lit forest";
}

void long()
{
    write("You are in part of a dimly lit forest. The shadows here seem\n" +
	  "unnaturally deep, and the air is bitterly cold despite the\n" +
	  "shelter of the trees. Trails lead north and east.\n");
}

int north()
{
    this_player()->move_player("north#room/south/sforst37");
    return 1;
}

int east()
{
    this_player()->move_player("east#room/south/sforst44");
    return 1;
}
