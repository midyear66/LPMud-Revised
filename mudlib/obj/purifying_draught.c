/*
 * Purifying draught - quest item for the Fairy Plague quest.
 * Given by the fairy elder, consumed when poured on the cursed runestone.
 */

string short_desc, long_desc;

void reset(int arg) {
    if (arg) return;
    short_desc = "A vial of purifying draught";
    long_desc =
"A small crystal vial filled with a swirling, iridescent liquid. Tiny\n" +
"motes of light dance within it like captured starlight. The fairy elder\n" +
"created it from a moonpetal flower and fairy magic to cleanse corruption.\n";
}

int id(string str) {
    return str == "draught" || str == "purifying draught" || str == "vial";
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
    write("The fairy elder entrusted this to you. You should hold on to it.\n");
    return 1;  /* 1 = prevent drop */
}

int query_value() {
    return 0;
}
