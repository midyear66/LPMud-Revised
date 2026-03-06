/*
 * Holy water vial - quest item for the Restless Dead quest.
 * Given by Brother Cedric, consumed when used on Valdris's tomb.
 */

string short_desc, long_desc;

void reset(int arg) {
    if (arg) return;
    short_desc = "A vial of holy water";
    long_desc =
"A small glass vial filled with clear water that shimmers with a faint\n" +
"silver light. Brother Cedric blessed it to break dark enchantments.\n";
}

int id(string str) {
    return str == "holy water" || str == "vial" || str == "water" ||
	   str == "holy_water";
}

string short() {
    return short_desc;
}

void long() {
    write(long_desc);
}

int get() {
    return 1;
}

int query_weight() {
    return 1;
}

int drop() {
    write("You should hold on to the holy water. You may need it.\n");
    return 1;  /* 1 = prevent drop */
}

int query_value() {
    return 0;
}
