/*
 * Quest registration object for the Restless Dead quest.
 * Placed in room/quest_room inventory so it counts toward wizard promotion.
 */

string hint_string, name;

void reset(int arg) {
    if (arg) return;
    name = "restless_dead";
    hint_string = "The dead stir in the old cemetery north of the church.";
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
