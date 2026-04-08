/*
 * The Bank - now with a functional vault system.
 * Players can deposit/withdraw gold and store/retrieve items.
 * Data persists across logoffs and server restarts.
 */

inherit "room/room";

object guard, clerk;
int door_is_open, door_is_locked;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "The bank";
	long_desc =
"You are in the bank. A sturdy counter separates you from the back\n" +
"offices. Behind the iron-barred counter, a clerk stands ready to\n" +
"assist with deposits and withdrawals.\n" +
"Commands: deposit, withdraw, balance, store, retrieve, vault.\n" +
"To the east, a heavy door leads to the back room.\n" +
"The narrow alley is to the west.\n";
	dest_dir = ({ "room/narr_alley", "west" });
	items = ({
	    "counter", "A sturdy wooden counter topped with heavy iron bars.\nThe clerk stands behind it, ready to handle transactions",
	    "bars", "Heavy iron bars set into the counter for security",
	    "door", "A heavy door leading to the back room of the bank"
	});
    }
    if (!guard || !living(guard)) {
	object key, weapon;
	guard = clone_object("obj/monster");
	guard->set_name("guard");
	guard->set_level(11);
	guard->set_hp(200);
	guard->set_al(100);
	guard->set_short("A guard");
	guard->set_long("A big and sturdy guard.\n");
	move_object(guard, this_object());
	weapon = clone_object("obj/weapon");
	weapon->set_name("shortsword");
	weapon->set_short("A shortsword");
	weapon->set_alias("sword");
	weapon->set_long(
"It is a professional looking short sword, used by warriors and guards.\n");
	weapon->set_class(15);
	weapon->set_value(700);
	weapon->set_weight(3);
	move_object(weapon, guard);
	guard->init_command("wield shortsword");
	key = clone_object("obj/treasure");
	key->set_id("key");
	key->set_alias("bank key");
	key->set_short("A bronze key");
	key->set_value(10);
	key->set_weight(1);
	move_object(key, guard);
    }
    if (!clerk || !living(clerk)) {
	clerk = clone_object("obj/monster");
	clerk->set_name("clerk");
	clerk->set_alias("bank clerk");
	clerk->set_alt_name("teller");
	clerk->set_short("The bank clerk");
	clerk->set_long(
"A meticulous-looking clerk in a crisp vest and spectacles. He stands\n" +
"behind the iron-barred counter with a thick ledger and a quill pen,\n" +
"ready to handle deposits and withdrawals.\n");
	clerk->set_level(5);
	clerk->set_al(200);
	clerk->set_aggressive(0);
	move_object(clerk, this_object());
	clerk->load_chat(2, ({
	    "The clerk adjusts his spectacles and flips through the ledger.\n",
	    "The clerk polishes the counter with a soft cloth.\n",
	    "The clerk says: Your valuables are safe with us.\n",
	    "The clerk counts a small stack of coins and makes a note.\n"
	}));
    }
    door_is_open = 0;
    door_is_locked = 1;

    /* Load the vault controller */
    if (!find_object("obj/bank_vault"))
	call_other("obj/bank_vault", "vault_path", "");
}

void init() {
    ::init();
    add_action("do_deposit", "deposit");
    add_action("do_withdraw", "withdraw");
    add_action("do_balance", "balance");
    add_action("do_store", "store");
    add_action("do_retrieve", "retrieve");
    add_action("do_vault", "vault");
    add_action("do_open", "open");
    add_action("do_unlock", "unlock");
    add_action("do_east", "east");
}

int has_clerk() {
    if (clerk && living(clerk) && present(clerk, this_object()))
	return 1;
    return 0;
}

int do_deposit(string str) {
    int amount, current;
    object player;
    string pname;

    if (!str) {
	write("Deposit how much? Usage: deposit <amount> or deposit all\n");
	return 1;
    }

    /* Ensure player directory exists for vault file */
    "obj/master"->ensure_player_dir(this_player()->query_real_name());

    player = this_player();
    pname = player->query_real_name();
    current = player->query_money();

    if (str == "all") {
	amount = current;
    } else {
	if (sscanf(str, "%d", amount) != 1 || amount <= 0) {
	    write("Deposit how much? Usage: deposit <amount> or deposit all\n");
	    return 1;
	}
    }

    if (amount <= 0) {
	write("You have no gold to deposit.\n");
	return 1;
    }
    if (amount > current) {
	write("You only have " + current + " gold coins.\n");
	return 1;
    }

    player->add_money(-amount);
    "obj/bank_vault"->set_gold(pname,
	"obj/bank_vault"->query_gold(pname) + amount);

    if (has_clerk()) {
	write("The clerk counts your coins carefully and records the amount\n" +
	    "in a thick ledger.\n");
	say("The clerk accepts a deposit from " +
	    player->query_name() + ".\n", player);
    }
    write("You deposit " + amount + " gold coins.\n");
    write("Vault balance: " + "obj/bank_vault"->query_gold(pname) +
	" gold coins.\n");
    return 1;
}

int do_withdraw(string str) {
    int amount, vault_gold;
    object player;
    string pname;

    if (!str) {
	write("Withdraw how much? Usage: withdraw <amount> or withdraw all\n");
	return 1;
    }

    player = this_player();
    pname = player->query_real_name();
    vault_gold = "obj/bank_vault"->query_gold(pname);

    if (str == "all") {
	amount = vault_gold;
    } else {
	if (sscanf(str, "%d", amount) != 1 || amount <= 0) {
	    write("Withdraw how much? Usage: withdraw <amount> or withdraw all\n");
	    return 1;
	}
    }

    if (amount <= 0) {
	write("Your vault has no gold to withdraw.\n");
	return 1;
    }
    if (amount > vault_gold) {
	write("Your vault only has " + vault_gold + " gold coins.\n");
	return 1;
    }

    "obj/bank_vault"->set_gold(pname, vault_gold - amount);
    player->add_money(amount);

    if (has_clerk()) {
	write("The clerk disappears into the back room and returns with a\n" +
	    "pouch of coins.\n");
	say("The clerk hands a pouch of coins to " +
	    player->query_name() + ".\n", player);
    }
    write("You withdraw " + amount + " gold coins.\n");
    write("Vault balance: " + "obj/bank_vault"->query_gold(pname) +
	" gold coins.\n");
    return 1;
}

int do_balance(string str) {
    string pname;
    int gold, num_items;

    pname = this_player()->query_real_name();
    gold = "obj/bank_vault"->query_gold(pname);
    num_items = "obj/bank_vault"->query_item_count(pname);

    if (has_clerk()) {
	write("The clerk consults the ledger and announces your holdings.\n");
    }
    write("=== Vault Balance ===\n");
    write("Gold: " + gold + " coins\n");
    write("Items: " + num_items + " stored\n");
    return 1;
}

int do_store(string str) {
    object player, ob;
    string pname;
    int result;

    if (!str) {
	write("Store what? Usage: store <item>\n");
	return 1;
    }

    player = this_player();
    pname = player->query_real_name();

    /* Ensure player directory exists for vault file */
    "obj/master"->ensure_player_dir(pname);

    ob = present(str, player);
    if (!ob) {
	write("You don't have that item.\n");
	return 1;
    }

    /* Don't allow storing quest items that prevent drop */
    if (ob->drop()) {
	write("You can't store that item.\n");
	return 1;
    }

    /* Check vault capacity */
    if ("obj/bank_vault"->query_item_count(pname) >= 20) {
	write("Your vault is full! Maximum 20 items.\n");
	return 1;
    }

    /* Check the item can be serialized */
    if (!"obj/bank_vault"->add_item(pname, ob)) {
	write("The clerk shakes his head. That item cannot be stored.\n");
	return 1;
    }

    /* Remove from player */
    player->add_weight(-ob->query_weight());
    if (has_clerk()) {
	write("The clerk takes the item and places it carefully in a\n" +
	    "numbered vault behind the counter.\n");
	say("The clerk stores an item for " +
	    player->query_name() + ".\n", player);
    }
    write("You store " + ob->short() + " in your vault.\n");
    destruct(ob);
    return 1;
}

int do_retrieve(string str) {
    object player, ob;
    string pname;
    string *items;
    int index, i;

    if (!str) {
	write("Retrieve what? Usage: retrieve <number> (see 'vault' for list)\n");
	return 1;
    }

    player = this_player();
    pname = player->query_real_name();
    items = "obj/bank_vault"->query_items(pname);

    if (sizeof(items) == 0) {
	write("Your vault has no items stored.\n");
	return 1;
    }

    /* Try as a number first */
    if (sscanf(str, "%d", index) == 1) {
	if (index < 1 || index > sizeof(items)) {
	    write("Invalid item number. Use 'vault' to see your items.\n");
	    return 1;
	}
    } else {
	/* Try to match by name */
	index = 0;
	i = 0;
	while (i < sizeof(items)) {
	    string item_name;
	    item_name = "obj/bank_vault"->item_short(items[i]);
	    if (item_name) {
		string a, b;
		if (item_name == str ||
		    sscanf(item_name, "%s" + str + "%s", a, b) == 2) {
		    index = i + 1;
		    break;
		}
	    }
	    i += 1;
	}
	if (index == 0) {
	    write("No item matching '" + str +
		"' found in your vault. Use 'vault' to see your items.\n");
	    return 1;
	}
    }

    ob = "obj/bank_vault"->remove_item(pname, index);
    if (!ob) {
	write("Failed to retrieve that item.\n");
	return 1;
    }

    /* Check if player can carry it */
    if (!player->add_weight(ob->query_weight())) {
	write("You can't carry that much weight.\n");
	/* Put it back */
	"obj/bank_vault"->add_item(pname, ob);
	destruct(ob);
	return 1;
    }

    move_object(ob, player);
    if (has_clerk()) {
	write("The clerk fetches the item from your vault and slides it\n" +
	    "across the counter.\n");
	say("The clerk retrieves an item for " +
	    player->query_name() + ".\n", player);
    }
    write("You retrieve " + ob->short() + " from your vault.\n");
    return 1;
}

int do_vault(string str) {
    string pname;
    string *items;
    int i;

    pname = this_player()->query_real_name();
    items = "obj/bank_vault"->query_items(pname);

    if (has_clerk()) {
	write("The clerk reads off the contents of your vault.\n");
    }
    write("=== Vault Contents ===\n");
    if (sizeof(items) == 0) {
	write("  (empty)\n");
    } else {
	i = 0;
	while (i < sizeof(items)) {
	    write("  " + (i + 1) + ". " +
		"obj/bank_vault"->item_short(items[i]) + "\n");
	    i += 1;
	}
    }
    write("Items: " + sizeof(items) + "/20\n");
    write("Gold: " + "obj/bank_vault"->query_gold(pname) + " coins\n");
    return 1;
}

int do_open(string str) {
    if (str && str != "door")
	return 0;
    if (door_is_open)
	return 0;
    if (door_is_locked) {
	write("The door is locked.\n");
	return 1;
    }
    door_is_open = 1;
    write("Ok.\n");
    say(this_player()->query_name() + " opened the door.\n");
    return 1;
}

int do_unlock(string str) {
    if (str && str != "door")
	return 0;
    if (door_is_open || !door_is_locked)
	return 0;
    if (!present("bank key", this_player())) {
	if (present("key", this_player()))
	    write("You don't have the right key.\n");
	else
	    write("You need a key.\n");
	return 1;
    }
    door_is_locked = 0;
    write("Ok.\n");
    say(this_player()->query_name() + " unlocked the door.\n");
    return 1;
}

int do_east() {
    if (!door_is_open) {
	write("The door is closed.\n");
	return 1;
    }
    if (guard && living(guard) && present(guard, this_object())) {
	write("The guard bars the way.\n");
	return 1;
    }
    this_player()->move_player("east#room/bankroom");
    return 1;
}

int query_door() {
    return !door_is_open;
}

void open_door_inside() {
    door_is_locked = 0;
    door_is_open = 1;
}

int query_drop_castle() {
    return 1;
}
