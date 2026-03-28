/*
 * Quest registration object for the Lost Miners quest.
 * Placed in room/quest_room inventory so it counts toward wizard promotion.
 */

string hint_string, name;

void reset(int arg) {
    if (arg) return;
    name = "lost_miners";
    hint_string = "An old miner mourns his lost crew near the mine entrance.";
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
