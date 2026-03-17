/*
 * Bank Vault Controller
 *
 * Handles per-player persistent vault storage for gold and items.
 * Data is stored in /players/<name>.vault as a text file.
 *
 * Format:
 *   GOLD:<amount>
 *   W:<id>|<alias>|<short>|<long>|<wc>|<value>|<weight>
 *   A:<id>|<alias>|<short>|<long>|<ac>|<value>|<weight>|<type>
 *   T:<id>|<alias>|<short>|<long>|<value>|<weight>
 */

#define MAX_VAULT_ITEMS 20

/*
 * Replace newlines with literal \n for storage, and vice versa.
 */
string encode_str(string str) {
    string a, b;
    string result;

    if (!str) return "";
    result = str;
    /* Replace actual newlines with the two-char sequence \n */
    while (sscanf(result, "%s\n%s", a, b) == 2)
	result = a + "\\n" + b;
    /* Replace pipe chars to avoid field separator collision */
    while (sscanf(result, "%s|%s", a, b) == 2)
	result = a + "\\p" + b;
    return result;
}

string decode_str(string str) {
    string a, b;
    string result;

    if (!str) return "";
    result = str;
    while (sscanf(result, "%s\\n%s", a, b) == 2)
	result = a + "\n" + b;
    while (sscanf(result, "%s\\p%s", a, b) == 2)
	result = a + "|" + b;
    return result;
}

string vault_path(string player_name) {
    return "bank.vault";
}

/*
 * Full path for read_file (reads don't go through valid_write prepend).
 */
string vault_read_path(string player_name) {
    return "/players/" + player_name + "/bank.vault";
}

/*
 * Parse a vault file and return ({ gold, item1, item2, ... })
 * where gold is the gold line string and items are serialized strings.
 */
mixed *load_vault(string player_name) {
    string data, line, rest;
    int gold;
    string *items;

    gold = 0;
    items = ({});
    data = read_file(vault_read_path(player_name));
    if (!data || data == "")
	return ({ gold, items });

    rest = data;
    while (rest && rest != "") {
	if (sscanf(rest, "%s\n%s", line, rest) != 2) {
	    line = rest;
	    rest = "";
	}
	if (!line || line == "")
	    continue;
	if (sscanf(line, "GOLD:%d", gold) == 1)
	    continue;
	/* Must be an item line */
	items = items + ({ line });
    }
    return ({ gold, items });
}

void save_vault(string player_name, int gold, string *items) {
    string data;
    int i;

    data = "GOLD:" + gold + "\n";
    i = 0;
    while (i < sizeof(items)) {
	data = data + items[i] + "\n";
	i += 1;
    }
    rm(vault_path(player_name));
    write_file(vault_path(player_name), data);
}

int query_gold(string player_name) {
    mixed *vault;
    vault = load_vault(player_name);
    return vault[0];
}

void set_gold(string player_name, int amount) {
    mixed *vault;
    vault = load_vault(player_name);
    save_vault(player_name, amount, vault[1]);
}

string *query_items(string player_name) {
    mixed *vault;
    vault = load_vault(player_name);
    return vault[1];
}

int query_item_count(string player_name) {
    return sizeof(query_items(player_name));
}

/*
 * Serialize an object into a vault line.
 * Detects type by checking weapon_class / armour_class.
 */
string serialize_item(object ob) {
    string id_str, alias_str, short_str, long_str;
    int val, wt;

    if (!ob) return 0;

    id_str = encode_str(ob->query_name());
    if (!id_str || id_str == "")
	id_str = "item";

    /* Get short without (wielded)/(worn) suffix */
    short_str = ob->short();
    if (short_str) {
	string tmp;
	if (sscanf(short_str, "%s (wielded)", tmp) == 1)
	    short_str = tmp;
	if (sscanf(short_str, "%s (worn)", tmp) == 1)
	    short_str = tmp;
    }
    short_str = encode_str(short_str);

    /* For long desc, we need to call long() but it writes to the player.
     * Instead, check for common property access patterns.
     * We'll use query_info or reconstruct from short. */
    long_str = encode_str(short_str + ".\\n");

    val = ob->query_value();
    wt = ob->query_weight();

    if (ob->weapon_class() > 0) {
	int wc;
	wc = ob->weapon_class();
	alias_str = encode_str("");
	return "W:" + id_str + "|" + alias_str + "|" + short_str +
	    "|" + long_str + "|" + wc + "|" + val + "|" + wt;
    }

    if (ob->armour_class() > 0) {
	int ac;
	string type_str;
	ac = ob->armour_class();
	type_str = ob->query_type();
	if (!type_str) type_str = "armour";
	alias_str = encode_str("");
	return "A:" + id_str + "|" + alias_str + "|" + short_str +
	    "|" + long_str + "|" + ac + "|" + val + "|" + wt +
	    "|" + encode_str(type_str);
    }

    /* Default: treasure */
    alias_str = encode_str("");
    return "T:" + id_str + "|" + alias_str + "|" + short_str +
	"|" + long_str + "|" + val + "|" + wt;
}

/*
 * Restore an object from a serialized vault line.
 * Returns the cloned, configured object (not yet moved anywhere).
 */
object restore_item(string line) {
    string type_prefix, fields;
    string *parts;
    object ob;
    string id_str, alias_str, short_str, long_str;
    int val, wt;

    if (!line || sizeof(line) < 3) return 0;

    type_prefix = line[0..1];
    fields = line[2..];

    if (type_prefix == "W:") {
	string wc_str;
	int wc;
	/* W:id|alias|short|long|wc|value|weight */
	if (sscanf(fields, "%s|%s|%s|%s|%d|%d|%d",
		   id_str, alias_str, short_str, long_str,
		   wc, val, wt) != 7)
	    return 0;
	ob = clone_object("obj/weapon");
	ob->set_id(decode_str(id_str));
	if (alias_str && alias_str != "")
	    ob->set_alias(decode_str(alias_str));
	ob->set_short(decode_str(short_str));
	ob->set_long(decode_str(long_str));
	ob->set_class(wc);
	ob->set_value(val);
	ob->set_weight(wt);
	return ob;
    }

    if (type_prefix == "A:") {
	string type_str;
	int ac;
	/* A:id|alias|short|long|ac|value|weight|type */
	if (sscanf(fields, "%s|%s|%s|%s|%d|%d|%d|%s",
		   id_str, alias_str, short_str, long_str,
		   ac, val, wt, type_str) != 8)
	    return 0;
	ob = clone_object("obj/armour");
	ob->set_id(decode_str(id_str));
	if (alias_str && alias_str != "")
	    ob->set_alias(decode_str(alias_str));
	ob->set_short(decode_str(short_str));
	ob->set_long(decode_str(long_str));
	ob->set_ac(ac);
	ob->set_value(val);
	ob->set_weight(wt);
	ob->set_type(decode_str(type_str));
	return ob;
    }

    if (type_prefix == "T:") {
	/* T:id|alias|short|long|value|weight */
	if (sscanf(fields, "%s|%s|%s|%s|%d|%d",
		   id_str, alias_str, short_str, long_str,
		   val, wt) != 6)
	    return 0;
	ob = clone_object("obj/treasure");
	ob->set_id(decode_str(id_str));
	if (alias_str && alias_str != "")
	    ob->set_alias(decode_str(alias_str));
	ob->set_short(decode_str(short_str));
	ob->set_long(decode_str(long_str));
	ob->set_value(val);
	ob->set_weight(wt);
	return ob;
    }

    return 0;
}

/*
 * Add an item to a player's vault. Returns 1 on success, 0 if full.
 */
int add_item(string player_name, object ob) {
    mixed *vault;
    string serialized;
    string *items;

    vault = load_vault(player_name);
    items = vault[1];
    if (sizeof(items) >= MAX_VAULT_ITEMS)
	return 0;

    serialized = serialize_item(ob);
    if (!serialized)
	return 0;

    items = items + ({ serialized });
    save_vault(player_name, vault[0], items);
    return 1;
}

/*
 * Remove an item from a player's vault by index (1-based).
 * Returns the restored object, or 0 on failure.
 */
object remove_item(string player_name, int index) {
    mixed *vault;
    string *items;
    string *new_items;
    object ob;
    int i;

    vault = load_vault(player_name);
    items = vault[1];

    if (index < 1 || index > sizeof(items))
	return 0;

    ob = restore_item(items[index - 1]);
    if (!ob)
	return 0;

    /* Rebuild items array without the removed item */
    new_items = ({});
    i = 0;
    while (i < sizeof(items)) {
	if (i != index - 1)
	    new_items = new_items + ({ items[i] });
	i += 1;
    }
    save_vault(player_name, vault[0], new_items);
    return ob;
}

/*
 * Get a short description for a vault item by index (1-based).
 */
string item_short(string line) {
    string type_prefix, fields;
    string id_str, alias_str, short_str, rest;

    if (!line || sizeof(line) < 3) return "Unknown item";
    type_prefix = line[0..1];
    fields = line[2..];

    if (sscanf(fields, "%s|%s|%s|%s", id_str, alias_str, short_str, rest) >= 3)
	return decode_str(short_str);

    return "Unknown item";
}
