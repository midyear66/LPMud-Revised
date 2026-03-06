/*
 * Wizard familiar - remote eyes and ears NPC.
 * Controlled by a wizard through their staff commands.
 */

inherit "obj/monster";

object wizard;
int relay_active;

void reset(int arg) {
    ::reset(0);
    set_name("familiar");
    set_level(1);
    set_aggressive(0);
    set_hp(500);
    set_ac(10);
    set_wc(0);
    relay_active = 1;
}

void set_wizard(object wiz) {
    wizard = wiz;
}

void set_familiar_desc(string desc) {
    set_short(desc);
    set_long(desc + "\n");
}

void catch_tell(string str) {
    ::catch_tell(str);
    if (wizard && relay_active) {
        if (sscanf(str, "[Familiar]%s", str) == 1)
            return;
        tell_object(wizard, "[Familiar] " + str);
    }
}

string familiar_look() {
    string result;
    object env, ob;
    mixed *exits;
    int i;

    env = environment(this_object());
    if (!env) return "The familiar is nowhere.\n";

    result = env->short() + "\n";
    result += env->query_long();

    exits = env->query_dest_dir();
    if (exits && sizeof(exits) > 0) {
        result += "Exits: ";
        i = 0;
        while (i < sizeof(exits)) {
            if (i > 0) result += ", ";
            result += exits[i + 1];
            i += 2;
        }
        result += "\n";
    }

    ob = first_inventory(env);
    while (ob) {
        if (ob != this_object() && ob->short()) {
            result += "  " + ob->short() + "\n";
        }
        ob = next_inventory(ob);
    }

    return result;
}

void familiar_say(string msg) {
    say(short_desc + " says: " + msg + "\n");
}

void familiar_emote(string msg) {
    say(short_desc + " " + msg + "\n");
}

void familiar_move(string dir) {
    command(dir);
}

void toggle_relay() {
    relay_active = !relay_active;
}

int query_relay() {
    return relay_active;
}

int id(string str) {
    return str == "familiar" || ::id(str);
}

int get() {
    return 0;
}

void attacked_by(object attacker) {
    int n;
    ::attacked_by(attacker);
    n = random(4);
    if (n == 0) command("north");
    else if (n == 1) command("south");
    else if (n == 2) command("east");
    else command("west");
    if (wizard)
        tell_object(wizard, "[Familiar] Your familiar is under attack and flees!\n");
}
