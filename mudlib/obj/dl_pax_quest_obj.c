/*
 * Quest registration object for the Liberation of Pax Tharkas quest.
 * Placed in room/quest_room inventory.
 */

string hint_string, name;

void reset(int arg) {
    if (arg) return;
    name = "liberation_pax_tharkas";
    hint_string = "Elistan in the prison of Pax Tharkas needs help liberating the fortress.";
}

int id(string str) {
    return str == name || str == "quest";
}

string short() {
    return name;
}

void long() {
    write("This is the quest '" + name + "':\n");
    write(hint_string + "\n");
}

string hint() {
    return hint_string;
}
