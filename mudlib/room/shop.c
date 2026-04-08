/*
 * The Village Shop - Weapons, armor, and general goods.
 * Pre-stocks weapons and armor via weapon_shop.c, and also serves as
 * the front-end for player consignment sales via room/store.c.
 */

inherit "room/room";
inherit "obj/weapon_shop";

object shopkeeper;
int stock_defined;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "The shop";
	long_desc =
"You are in a shop. Weapons and armor hang from the walls, and\n" +
"a gruff shopkeeper stands behind a sturdy wooden counter.\n" +
"Commands are: 'buy item', 'sell item', 'sell all', 'list',\n" +
"'list weapons', 'list armours', and 'value item'.\n" +
"There is an opening to the north, and some shimmering\n" +
"blue light in the doorway. To the west you see a small room.\n";
	dest_dir = ({
	    "room/vill_road2", "south",
	    "room/storage", "west"
	});
	items = ({
	    "counter", "A sturdy wooden counter scarred by years of trade",
	    "walls", "Weapons and armor hang from hooks on the walls",
	    "opening", "An opening to the north with a shimmering blue light",
	    "light", "A shimmering blue force field blocks the north opening"
	});
    }

    /* Define stock tables once */
    if (!stock_defined) {
	init_shop();

	/* Fixed weapons -- always available */
	add_stock_weapon("knife",
	    "A small knife",
	    "A small iron knife, useful for a beginner.\n",
	    3, 8, 1);
	add_stock_weapon("sword",
	    "A short sword",
	    "A short iron sword with a simple crossguard.\n",
	    5, 40, 3);
	add_stock_weapon("longsword",
	    "A long sword",
	    "A reliable iron longsword, good for most situations.\n",
	    7, 80, 4);
	add_stock_weapon("broadsword",
	    "A broad sword",
	    "A broad-bladed sword designed for heavy cutting strokes.\n",
	    8, 120, 4);

	/* Fixed armor -- always available */
	add_stock_armour("hat",
	    "A leather hat",
	    "A hardened leather hat offering basic head protection.\n",
	    "helmet", 1, 15, 1);
	add_stock_armour("vest",
	    "A leather vest",
	    "A vest of thick leather, offering some protection.\n",
	    "armour", 1, 35, 2);
	add_stock_armour("shield",
	    "A small shield",
	    "A small wooden shield with an iron rim.\n",
	    "shield", 1, 25, 2);
	add_stock_armour("chainmail",
	    "Chain mail armor",
	    "A coat of interlocking iron rings, solid protection.\n",
	    "armour", 2, 90, 3);

	/* Random weapons pool */
	add_random_weapon("dagger",
	    "A dagger",
	    "A thin-bladed dagger for close fighting.\n",
	    3, 12, 1);
	add_random_weapon("club",
	    "A club",
	    "A simple wooden club, heavy and blunt.\n",
	    3, 10, 2);
	add_random_weapon("mace",
	    "A mace",
	    "An iron mace with a flanged head.\n",
	    5, 45, 3);
	add_random_weapon("battleaxe",
	    "A battle axe",
	    "A heavy battle axe with a broad iron head.\n",
	    7, 90, 4);
	add_random_weapon("flail",
	    "A flail",
	    "A spiked ball on a chain, attached to a handle.\n",
	    6, 60, 3);
	add_random_weapon("greatsword",
	    "A two-handed sword",
	    "An enormous two-handed sword requiring great strength.\n",
	    9, 150, 5);
	add_random_weapon("pike",
	    "A pike",
	    "A long pike with a sharp steel head.\n",
	    6, 55, 4);
	add_random_weapon("hammer",
	    "A hammer",
	    "A heavy iron hammer, good for bashing.\n",
	    4, 30, 3);

	/* Random armor pool */
	add_random_armour("clothcap",
	    "A cloth cap",
	    "A padded cloth cap, better than nothing.\n",
	    "helmet", 1, 10, 1);
	add_random_armour("ironshield",
	    "An iron shield",
	    "A sturdy iron shield with a leather grip.\n",
	    "shield", 2, 85, 3);
	add_random_armour("padded",
	    "Padded armor",
	    "Thick quilted armor offering modest protection.\n",
	    "armour", 1, 45, 2);
	add_random_armour("ringmail",
	    "Ring mail armor",
	    "Armor of metal rings sewn onto leather.\n",
	    "ring", 1, 50, 2);
	add_random_armour("steelhelm",
	    "A steel helm",
	    "A polished steel helmet with a nose guard.\n",
	    "helmet", 1, 40, 2);
	add_random_armour("gloves",
	    "Leather gloves",
	    "Sturdy leather gloves with reinforced palms.\n",
	    "glove", 1, 15, 1);
	add_random_armour("scalemail",
	    "Scale mail armor",
	    "Armor of overlapping metal scales on leather.\n",
	    "armour", 2, 100, 3);

	stock_defined = 1;
    }

    restock();

    /* Spawn shopkeeper */
    if (!shopkeeper || !living(shopkeeper)) {
	shopkeeper = clone_object("obj/monster");
	shopkeeper->set_name("shopkeeper");
	shopkeeper->set_alias("keeper");
	shopkeeper->set_short("The shopkeeper");
	shopkeeper->set_long(
"A gruff, barrel-chested man with a permanent scowl and thick\n" +
"forearms. He watches customers with suspicious eyes, his meaty\n" +
"hands resting on the counter. Despite his surly demeanor, he\n" +
"runs an honest shop and his prices are fair.\n");
	shopkeeper->set_level(5);
	shopkeeper->set_hp(82);
	shopkeeper->set_al(50);
	shopkeeper->set_aggressive(0);
	move_object(shopkeeper, this_object());
	shopkeeper->load_chat(8, ({
	    "The shopkeeper grumbles: Business has been slow lately.\n",
	    "The shopkeeper polishes a sword blade with an oily rag.\n",
	    "The shopkeeper says: You break it, you buy it.\n",
	    "The shopkeeper eyes you suspiciously.\n",
	    "The shopkeeper says: Best prices in the village, I guarantee it.\n"
	}));
    }
}

void init() {
    ::init();
    /* Register village-specific commands before shop_init so they take priority */
    add_action("shop_get", "get");
    add_action("shop_get", "take");
    add_action("village_buy", "buy");
    add_action("village_sell", "sell");
    add_action("village_list", "list");
    add_action("shop_value", "value");
    add_action("go_north", "north");
}

int go_north() {
    if (this_player()->query_level() < 20) {
	write("A strong magic force stops you.\n");
	say(this_player()->query_name() +
	    " tries to go through the field, but fails.\n");
	return 1;
    }
    write("You wriggle through the force field...\n");
    this_player()->move_player("north#room/store");
    return 1;
}

int village_buy(string item) {
    object ob;
    int value;
    string display_name;

    if (!item) {
	write("Buy what? Use 'list' to see available items.\n");
	return 1;
    }

    /* First check pre-stocked items in this room */
    ob = present(item, this_object());
    if (ob && !living(ob) && ob->query_info()) {
	value = ob->query_value();
	if (value) {
	    display_name = ob->query_info();
	    value = value * 2;
	    if (this_player()->query_money() < value) {
		write("That costs " + value +
		      " gold coins, which you don't have.\n");
		return 1;
	    }
	    if (!this_player()->add_weight(ob->query_weight())) {
		write("You can't carry that much.\n");
		return 1;
	    }
	    this_player()->add_money(0 - value);
	    ob->set_short(display_name);
	    write("You buy " + display_name + " for " + value +
		  " gold coins.\n");
	    say(this_player()->query_name() + " buys " +
		display_name + ".\n");
	    move_object(ob, this_player());
	    return 1;
	}
    }

    /* Fall through to store.c consignment */
    "room/store"->buy(item);
    return 1;
}

void do_consign_sell(object ob) {
    int value, do_destroy, weight;

    value = ob->query_value();
    if (!value) {
	write(ob->short() + " has no value.\n");
	return;
    }
    if (environment(ob) == this_player()) {
	if (ob->drop()) {
	    write("You can't let go of it!\n");
	    return;
	}
	weight = ob->query_weight();
	this_player()->add_weight(0 - weight);
    }
    if (value > 2000)
	do_destroy = 1;
    if (value > 1000) {
	write("The shop is low on money...\n");
	value = 1000;
    }
    write("You get " + value + " gold coins.\n");
    say(this_player()->query_name() + " sells " + ob->short() + ".\n");
    this_player()->add_money(value);
    add_worth(value, ob);
    if (do_destroy) {
	write("The valuable item is hidden away.\n");
	destruct(ob);
	return;
    }
    "room/store"->store(ob);
}

int village_sell(string item) {
    object ob, next;

    if (!item) {
	write("Sell what?\n");
	return 1;
    }

    if (item == "all") {
	ob = first_inventory(this_player());
	while (ob) {
	    next = next_inventory(ob);
	    if (!ob->drop() && ob->query_value()) {
		write(ob->short() + ":\t");
		do_consign_sell(ob);
	    }
	    ob = next;
	}
	write("Ok.\n");
	return 1;
    }

    ob = present(item, this_player());
    if (!ob)
	ob = present(item, this_object());
    if (!ob) {
	write("No such item.\n");
	return 1;
    }

    do_consign_sell(ob);
    return 1;
}

int village_list(string filter) {
    string store_filter;

    /* Normalize filter for both show_stock and store */
    if (!filter)
	store_filter = "all";
    else if (filter == "armor" || filter == "armour" || filter == "armours")
	store_filter = "armours";
    else if (filter == "weapon" || filter == "weapons")
	store_filter = "weapons";
    else
	store_filter = filter;

    show_stock(filter);
    write("Consignment items (sold by other players):\n");
    "room/store"->inventory(store_filter);
    return 1;
}

void long(string str) {
    ::long(str);
}

int query_drop_castle() {
    return 1;
}
