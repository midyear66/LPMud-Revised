/*
 * Quest registration object for the Defeat of Takhisis quest.
 * Placed in room/quest_room inventory.
 */

string hint_string, name;

void reset(int arg) {
    if (arg) return;
    name = "defeat_takhisis";
    hint_string = "A portal in Pax Tharkas leads to the Temple of the Dark Queen. Level 16+ only.";
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
