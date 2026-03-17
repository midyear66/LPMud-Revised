/*
 * Quest registration object for the Fairy Plague quest.
 * Placed in room/quest_room inventory so it counts toward wizard promotion.
 */

string hint_string, name;

void reset(int arg) {
    if (arg) return;
    name = "fairy_plague";
    hint_string = "Forest fairies near Crescent Lake are dying from a mysterious plague.";
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
