/*
 * Quest registration object for the Disks of Mishakal quest.
 * Placed in room/quest_room inventory.
 */

string hint_string, name;

void reset(int arg) {
    if (arg) return;
    name = "disks_of_mishakal";
    hint_string = "Riverwind in the ruins of Xak Tsaroth seeks the Disks of Mishakal.";
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
