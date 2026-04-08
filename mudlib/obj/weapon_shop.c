/*
 * weapon_shop.c -- Reusable weapon/armor shop base.
 *
 * Inherit this in a shop room alongside "room/room" to get buy/sell/list/value
 * commands and automatic stock management. Configure stock in reset() using
 * the add_stock_* and add_random_* functions, then call restock().
 *
 * Usage:
 *   inherit "room/room";
 *   inherit "obj/weapon_shop";
 *
 *   void reset(int arg) {
 *       if (!arg) {
 *           // configure room...
 *           init_shop();  // call once on first load
 *           add_stock_weapon("sword", "A short sword", "...", 5, 40, 3);
 *           add_stock_armour("helmet", "A leather cap", "...", "helmet", 1, 20, 1);
 *           add_random_weapon("mace", "A heavy mace", "...", 5, 45, 3);
 *           // etc.
 *       }
 *       restock();
 *   }
 */

/*
 * Stock definition arrays. Each entry is an array:
 *   Weapon: ({ name, short, long, class, value, weight })
 *   Armour: ({ name, short, long, type, ac, value, weight })
 */
mixed *fixed_weapons;
mixed *fixed_armours;
mixed *random_weapons;
mixed *random_armours;
int shop_initialized;

/* Track which objects are shop stock (not player-sold consignment) */
string *stock_ids;

void init_shop() {
    if (shop_initialized)
	return;
    fixed_weapons = ({});
    fixed_armours = ({});
    random_weapons = ({});
    random_armours = ({});
    stock_ids = ({});
    shop_initialized = 1;
}

void add_stock_weapon(string name, string sh, string lo,
		      int class, int value, int weight) {
    if (!shop_initialized) init_shop();
    fixed_weapons += ({ ({ name, sh, lo, class, value, weight }) });
}

void add_stock_armour(string name, string sh, string lo,
		      string type, int ac, int value, int weight) {
    if (!shop_initialized) init_shop();
    fixed_armours += ({ ({ name, sh, lo, type, ac, value, weight }) });
}

void add_random_weapon(string name, string sh, string lo,
		       int class, int value, int weight) {
    if (!shop_initialized) init_shop();
    random_weapons += ({ ({ name, sh, lo, class, value, weight }) });
}

void add_random_armour(string name, string sh, string lo,
		       string type, int ac, int value, int weight) {
    if (!shop_initialized) init_shop();
    random_armours += ({ ({ name, sh, lo, type, ac, value, weight }) });
}

/* Create a weapon from a definition array and move it to the shop.
 * Short desc is stored in info so items are invisible in room inventory.
 * The short is restored when a player buys the item. */
object spawn_weapon(mixed *def) {
    object w;
    w = clone_object("obj/weapon");
    w->set_name(def[0]);
    w->set_long(def[2]);
    w->set_class(def[3]);
    w->set_value(def[4]);
    w->set_weight(def[5]);
    w->set_info(def[1]);     /* store display name in info */
    w->set_short(0);         /* hide from room inventory */
    move_object(w, this_object());
    return w;
}

/* Create an armour from a definition array and move it to the shop.
 * Same hidden-short approach as weapons. */
object spawn_armour(mixed *def) {
    object a;
    a = clone_object("obj/armour");
    a->set_id(def[0]);
    a->set_long(def[2]);
    a->set_type(def[3]);
    a->set_ac(def[4]);
    a->set_value(def[5]);
    a->set_weight(def[6]);
    a->set_info(def[1]);     /* store display name in info */
    a->set_short(0);         /* hide from room inventory */
    move_object(a, this_object());
    return a;
}

/* Check if a fixed stock item with given name is present in shop */
int stock_present(string name) {
    object ob;
    ob = first_inventory(this_object());
    while (ob) {
	if (ob->id(name) && !living(ob))
	    return 1;
	ob = next_inventory(ob);
    }
    return 0;
}

/* Remove all non-fixed, non-living objects tagged as random stock */
void clear_random_stock() {
    object ob, next;
    int i, is_fixed;

    ob = first_inventory(this_object());
    while (ob) {
	next = next_inventory(ob);
	if (!living(ob) && ob->query_value()) {
	    /* Check if this is a fixed stock item by id */
	    is_fixed = 0;
	    for (i = 0; i < sizeof(fixed_weapons); i++) {
		if (ob->id(fixed_weapons[i][0])) {
		    is_fixed = 1;
		    break;
		}
	    }
	    if (!is_fixed) {
		for (i = 0; i < sizeof(fixed_armours); i++) {
		    if (ob->id(fixed_armours[i][0])) {
			is_fixed = 1;
			break;
		    }
		}
	    }
	    if (!is_fixed)
		destruct(ob);
	}
	ob = next;
    }
}

/* Restock the shop. Call from room's reset(). */
void restock() {
    int i, count, idx, j, tmp;
    mixed *pool;
    mixed *indices;

    if (!shop_initialized)
	return;

    /* Restock fixed weapons */
    for (i = 0; i < sizeof(fixed_weapons); i++) {
	if (!stock_present(fixed_weapons[i][0]))
	    spawn_weapon(fixed_weapons[i]);
    }

    /* Restock fixed armours */
    for (i = 0; i < sizeof(fixed_armours); i++) {
	if (!stock_present(fixed_armours[i][0]))
	    spawn_armour(fixed_armours[i]);
    }

    /* Clear and respawn random stock */
    clear_random_stock();

    pool = random_weapons + random_armours;
    if (sizeof(pool) == 0)
	return;

    count = 5 + random(6); /* 5-10 items */
    if (count > sizeof(pool))
	count = sizeof(pool);

    /* Shuffle pool indices and pick first 'count' */
    indices = allocate(sizeof(pool));
    for (i = 0; i < sizeof(pool); i++)
	indices[i] = i;

    /* Fisher-Yates shuffle */
    for (i = sizeof(indices) - 1; i > 0; i--) {
	j = random(i + 1);
	tmp = indices[i];
	indices[i] = indices[j];
	indices[j] = tmp;
    }

    for (i = 0; i < count; i++) {
	idx = indices[i];
	if (idx < sizeof(random_weapons))
	    spawn_weapon(random_weapons[idx]);
	else
	    spawn_armour(random_armours[idx - sizeof(random_weapons)]);
    }
}

/* Pad a number string to a fixed width with leading spaces */
string pad_price(int price) {
    string s;
    s = "" + price;
    while (sizeof(s) < 4)
	s = " " + s;
    return s;
}

/* Show shop inventory. Called from room's long() or list command. */
void show_stock(string filter) {
    object ob;
    int value, found, show;
    string display_name;

    if (!filter)
	filter = "all";
    if (filter == "armor" || filter == "armour")
	filter = "armours";
    if (filter == "weapon")
	filter = "weapons";

    write("\nItems for sale:\n");
    write("---------------\n");
    found = 0;
    ob = first_inventory(this_object());
    while (ob) {
	if (!living(ob)) {
	    value = ob->query_value();
	    display_name = ob->query_info();
	    if (value && display_name) {
		show = 0;
		if (filter == "all")
		    show = 1;
		else if (filter == "weapons" && ob->weapon_class())
		    show = 1;
		else if (filter == "armours" && ob->armour_class())
		    show = 1;
		if (show) {
		    write("  " + pad_price(value * 2) +
			  " gp   " + display_name + "\n");
		    found = 1;
		}
	    }
	}
	ob = next_inventory(ob);
    }
    if (!found)
	write("  Nothing available.\n");
    write("\nUse 'buy <item>' to purchase. 'sell <item>' to sell.\n\n");
}

/* --- COMMANDS --- */

void shop_init() {
    add_action("shop_get", "get");
    add_action("shop_get", "take");
    add_action("shop_buy", "buy");
    add_action("shop_sell", "sell");
    add_action("shop_list", "list");
    add_action("shop_value", "value");
}

/* Intercept get/take to prevent taking shop items without buying */
int shop_get(string item) {
    object ob;

    if (!item)
	return 0;
    ob = present(item, this_object());
    if (ob && !living(ob) && ob->query_info()) {
	write("You must 'buy " + item + "' to purchase that.\n");
	return 1;
    }
    return 0;
}

int shop_buy(string item) {
    object ob;
    int value;
    string display_name;

    if (!item) {
	write("Buy what? Use 'list' to see available items.\n");
	return 1;
    }

    ob = present(item, this_object());
    if (!ob || living(ob)) {
	/* Not found in shop stock -- return 0 to allow fallthrough */
	return 0;
    }

    value = ob->query_value();
    if (!value) {
	write("That item is not for sale.\n");
	return 1;
    }

    display_name = ob->query_info();
    if (!display_name)
	display_name = item;

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
    /* Restore short description so item shows in player inventory */
    ob->set_short(display_name);
    write("You buy " + display_name + " for " + value + " gold coins.\n");
    say(this_player()->query_name() + " buys " + display_name + ".\n");
    move_object(ob, this_player());
    return 1;
}

void shop_do_sell(object ob) {
    int value;

    value = ob->query_value();
    if (!value) {
	write(ob->short() + " has no value.\n");
	return;
    }

    if (ob->drop()) {
	write("You can't let go of it!\n");
	return;
    }

    this_player()->add_weight(0 - ob->query_weight());

    if (value > 1000) {
	write("The shop is low on money...\n");
	value = 1000;
    }

    write("You sell " + ob->short() + " for " + value + " gold coins.\n");
    say(this_player()->query_name() + " sells " + ob->short() + ".\n");
    this_player()->add_money(value);
    destruct(ob);
}

int shop_sell(string item) {
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
		shop_do_sell(ob);
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

    shop_do_sell(ob);
    return 1;
}

int shop_list(string filter) {
    show_stock(filter);
    return 1;
}

int shop_value(string item) {
    object ob;
    int value;

    if (!item) {
	write("Value what?\n");
	return 1;
    }

    ob = present(item, this_player());
    if (!ob) {
	write("You don't have that.\n");
	return 1;
    }

    value = ob->query_value();
    if (!value) {
	write(ob->short() + " has no value.\n");
	return 1;
    }

    if (value > 1000)
	value = 1000;
    write("You would get " + value + " gold coins for " +
	  ob->short() + ".\n");
    return 1;
}
