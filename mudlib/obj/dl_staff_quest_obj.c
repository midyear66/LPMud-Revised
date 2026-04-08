/*
 * Quest registration object for the Blue Crystal Staff quest.
 * Placed in room/quest_room inventory.
 */

string hint_string, name;

void reset(int arg) {
    if (arg) return;
    name = "blue_crystal_staff";
    hint_string = "Goldmoon in the temple of Solace seeks help with a corrupted vallenwood.";
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
