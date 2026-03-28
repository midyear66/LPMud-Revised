/*
 * Collapsed passage - trapped miner for the Lost Miners quest.
 * Player gives rope to the miner to rescue them.
 */

object miner;

void reset(int started)
{
    if (!started)
	set_light(0);
    if (!miner || !living(miner)) {
	miner = clone_object("obj/monster");
	miner->set_name("miner");
	miner->set_alias("trapped miner");
	miner->set_short("A trapped miner");
	miner->set_long(
"A battered miner pinned behind a wall of fallen rock and\n" +
"broken timbers. He is bruised and exhausted but still alive.\n" +
"He calls out weakly for help.\n");
	miner->set_level(1);
	miner->set_hp(10);
	miner->set_al(100);
	miner->set_aggressive(0);
	move_object(miner, this_object());
	miner->load_chat(10, ({
	    "The trapped miner taps weakly on the rocks.\n",
	    "The trapped miner calls out: Is someone there? Help!\n",
	    "The trapped miner coughs in the dusty air.\n"
	}));
    }
}

void init()
{
    add_action("south", "south");
    add_action("give_item", "give");
}

string short()
{
    return "Collapsed passage";
}

void long()
{
    write("The tunnel has collapsed here. Broken timbers and fallen rock\n" +
	  "block the passage ahead. Through a gap in the rubble you can\n" +
	  "see a miner trapped on the other side, unable to climb free.\n" +
	  "The only way out is back south.\n");
}

int south()
{
    this_player()->move_player("south#room/mine/tunnel26");
    return 1;
}

int give_item(string str)
{
    object controller;
    object player;
    object rope;
    object token;
    string item, target;

    if (!str)
	return 0;
    if (sscanf(str, "%s to miner", item) != 1 &&
	sscanf(str, "%s to trapped miner", item) != 1)
	return 0;

    player = this_player();

    if (item != "rope" && item != "coil of rope") {
	write("The trapped miner says: I need a rope to climb free!\n");
	return 1;
    }

    rope = present("rope", player);
    if (!rope) {
	write("You don't have a rope.\n");
	return 1;
    }

    controller = find_object("obj/mine_quest");
    if (!controller) {
	write("The trapped miner says: Thank you, but I think I can\n" +
	      "manage on my own now.\n");
	return 1;
    }

    if (!controller->is_quest_player(player)) {
	write("The trapped miner says: Thank you kindly, stranger.\n");
	destruct(rope);
	return 1;
    }

    /* Quest player giving rope - advance quest */
    destruct(rope);
    token = clone_object("obj/treasure");
    token->set_id("token");
    token->set_alias("mining token");
    token->set_short("A mining token");
    token->set_long(
"A small brass token stamped with a pickaxe symbol. Miners\n" +
"carry these as identification in the deep tunnels.\n");
    token->set_value(0);
    token->set_weight(1);
    move_object(token, player);
    controller->set_stage(2);
    write("You toss the rope through the gap in the rubble.\n" +
"The trapped miner grabs it and hauls himself free, scrambling\n" +
"over the fallen rocks with desperate energy.\n" +
"The trapped miner says: Thank you! I thought I was done for.\n" +
"Take this -- show it to old Grimjaw so he knows I'm alive.\n" +
"The miner presses a brass mining token into your hand and\n" +
"limps away toward the surface.\n");
    say(player->query_name() + " tosses a rope to the trapped miner\n" +
	"and pulls him free from the rubble.\n", player);
    return 1;
}
