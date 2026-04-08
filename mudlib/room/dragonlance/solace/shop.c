/*
 * Solace General Store - Weapon and armor shop in the treetops.
 * Theros Ironfeld runs the forge. Level 1-5 stock plus random items.
 */

inherit "room/room";
inherit "obj/weapon_shop";

object shopkeeper;
int stock_defined;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "Solace general store";
	long_desc =
"A cozy shop built into the hollow of a great vallenwood tree. A\n" +
"forge glows in the back, and the walls are lined with weapons and\n" +
"armor of solid craftsmanship. The ring of hammer on steel echoes\n" +
"through the warm air. A counter made from a cross-section of\n" +
"vallenwood separates the shop floor from the forge area.\n" +
"Use 'list' to see wares, 'buy <item>' to purchase, 'sell <item>'\n" +
"to sell. The village square lies back to the east.\n";
	dest_dir = ({
	    "room/dragonlance/solace/square", "east"
	});
	items = ({
	    "forge", "A well-maintained forge glowing with hot coals",
	    "walls", "Weapons and armor hang from hooks on the curved wooden walls",
	    "counter", "A counter made from a cross-section of vallenwood, polished smooth"
	});
    }

    /* Define stock tables once */
    if (!stock_defined) {
	init_shop();

	/* Fixed weapons -- always available */
	add_stock_weapon("club", "wooden club",
	    "A wooden club",
	    "A heavy wooden club, simple but effective.\n",
	    3, 10, 2);
	add_stock_weapon("sword", "short sword",
	    "A short sword",
	    "A short iron sword with a leather-wrapped grip.\n",
	    5, 40, 3);
	add_stock_weapon("axe", "hand axe",
	    "A hand axe",
	    "A small hand axe with a keen iron head.\n",
	    5, 35, 3);
	add_stock_weapon("spear", 0,
	    "A sturdy spear",
	    "A sturdy wooden spear tipped with forged iron.\n",
	    6, 50, 4);
	add_stock_weapon("longsword", "long sword",
	    "A longsword",
	    "A well-forged iron longsword, balanced for combat.\n",
	    7, 80, 4);

	/* Fixed armor -- always available */
	add_stock_armour("cap", "leather cap",
	    "A leather cap",
	    "A hardened leather cap that protects the head.\n",
	    "helmet", 1, 20, 1);
	add_stock_armour("leather", "leather armor",
	    "Leather armor",
	    "A suit of boiled leather armor, light and flexible.\n",
	    "armour", 1, 40, 2);
	add_stock_armour("shield", "wooden shield",
	    "A wooden shield",
	    "A round wooden shield reinforced with an iron boss.\n",
	    "shield", 1, 30, 2);
	add_stock_armour("chain", "chain shirt",
	    "A chain shirt",
	    "A shirt of interlocking chain links, offering good protection.\n",
	    "armour", 2, 80, 3);
	add_stock_armour("helm", "iron helm",
	    "An iron helm",
	    "A plain but sturdy iron helmet.\n",
	    "helmet", 1, 35, 2);

	/* Random weapons pool */
	add_random_weapon("dagger", 0,
	    "A dagger",
	    "A small, sharp dagger suitable for close combat.\n",
	    3, 15, 1);
	add_random_weapon("mace", "heavy mace",
	    "A heavy mace",
	    "A flanged iron mace that delivers crushing blows.\n",
	    5, 45, 3);
	add_random_weapon("battleaxe", "battle axe",
	    "A battle axe",
	    "A large battle axe with a double-bladed iron head.\n",
	    7, 90, 4);
	add_random_weapon("flail", 0,
	    "A flail",
	    "A spiked iron ball on a chain, attached to a wooden handle.\n",
	    6, 60, 3);
	add_random_weapon("quarterstaff", 0,
	    "A quarterstaff",
	    "A hardwood quarterstaff, iron-shod at both ends.\n",
	    4, 25, 3);
	add_random_weapon("broadsword", "broad sword",
	    "A broadsword",
	    "A heavy broadsword with a wide blade for slashing.\n",
	    8, 120, 4);
	add_random_weapon("warhammer", "war hammer",
	    "A warhammer",
	    "A sturdy warhammer with a steel head.\n",
	    7, 85, 4);
	add_random_weapon("morningstar", "morning star",
	    "A morning star",
	    "A spiked steel ball on a short haft, a brutal weapon.\n",
	    8, 110, 4);

	/* Random armor pool */
	add_random_armour("gloves", "leather gloves",
	    "Leather gloves",
	    "A pair of sturdy leather gloves reinforced at the knuckles.\n",
	    "glove", 1, 15, 1);
	add_random_armour("studded", "studded leather",
	    "Studded leather armor",
	    "Leather armor reinforced with metal studs.\n",
	    "armour", 1, 55, 2);
	add_random_armour("buckler", "small shield",
	    "A small shield",
	    "A small round buckler for deflecting blows.\n",
	    "shield", 1, 25, 2);
	add_random_armour("scale", "scale armor",
	    "Scale armor",
	    "Armor made of overlapping metal scales riveted to leather.\n",
	    "armour", 2, 100, 3);
	add_random_armour("steelshield", "steel shield",
	    "A steel shield",
	    "A round steel shield bearing the mark of Solace.\n",
	    "shield", 2, 90, 3);
	add_random_armour("halfhelm", "half helm",
	    "A half helm",
	    "An open-faced steel helm protecting the top and sides of the head.\n",
	    "helmet", 1, 30, 2);
	add_random_armour("ringmail", "ring mail",
	    "Ring mail armor",
	    "Armor made of metal rings sewn onto a leather backing.\n",
	    "ring", 1, 50, 2);

	stock_defined = 1;
    }

    restock();

    /* Spawn shopkeeper */
    if (!shopkeeper || !living(shopkeeper)) {
	shopkeeper = clone_object("obj/monster");
	shopkeeper->set_name("theros");
	shopkeeper->set_alias("theros ironfeld");
	shopkeeper->set_alt_name("shopkeeper");
	shopkeeper->set_short("Theros Ironfeld, the blacksmith");
	shopkeeper->set_long(
"A powerfully built man with arms like tree trunks and hands\n" +
"blackened by years at the forge. His leather apron is scarred\n" +
"with burn marks, and his eyes hold the steady calm of a master\n" +
"craftsman. He is Theros Ironfeld, the finest smith in Solace.\n");
	shopkeeper->set_level(8);
	shopkeeper->set_hp(106);
	shopkeeper->set_al(100);
	shopkeeper->set_aggressive(0);
	move_object(shopkeeper, this_object());
	shopkeeper->load_chat(10, ({
	    "Theros hammers a glowing piece of iron on his anvil.\n",
	    "Theros says: Fine weapons, forged right here in Solace!\n",
	    "Theros says: The Dragonarmy may have numbers, but our steel is better.\n",
	    "Theros wipes sweat from his brow and examines a blade.\n",
	    "Theros says: Vallenwood charcoal makes the hottest forge fire.\n"
	}));
    }
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
    write("You sell " + ob->short() + " for " + value + " gold coins.\n");
    say(this_player()->query_name() + " sells " + ob->short() + ".\n");
    this_player()->add_money(value);
    if (do_destroy) {
	write("The valuable item is hidden away.\n");
	destruct(ob);
	return;
    }
    "room/store"->store(ob);
}

int solace_sell(string item) {
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
    if (!ob) {
	write("You don't have that.\n");
	return 1;
    }

    do_consign_sell(ob);
    return 1;
}

int solace_buy(string item) {
    object ob;
    int value;
    string display_name;

    if (!item) {
	write("Buy what? Use 'list' to see available items.\n");
	return 1;
    }

    /* Check pre-stocked items */
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

int solace_list(string filter) {
    string store_filter;

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

void init() {
    ::init();
    /* Register shop-specific commands first so they take priority */
    add_action("shop_get", "get");
    add_action("shop_get", "take");
    add_action("solace_buy", "buy");
    add_action("solace_sell", "sell");
    add_action("solace_list", "list");
    add_action("shop_value", "value");
}

void long(string str) {
    ::long(str);
}
