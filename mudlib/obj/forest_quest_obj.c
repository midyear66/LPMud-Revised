/*
 * Quest registration object for the Forest Remedy quest.
 * Placed in room/quest_room inventory so it counts toward wizard promotion.
 */

string hint_string, name;

void reset(int arg) {
    if (arg) return;
    name = "forest_remedy";
    hint_string = "A wounded traveler in the deep forest needs your help.";
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
