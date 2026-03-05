inherit "obj/weapon";

void reset(int arg) {
    ::reset(arg);
    if (arg) return;
    set_name("staff");
    set_alt_name("wizard staff");
    set_alias("wizard");
    set_short("A wizard's staff");
    set_long(
        "A gnarled staff of ancient ironwood, dark as midnight. Runes of\n" +
        "power are carved deep along its length, pulsing with a faint blue\n" +
        "glow. The wood is warm to the touch and hums with arcane energy.\n" +
        "A crystal orb set into the head of the staff crackles with\n" +
        "barely contained lightning.\n");
    set_class(20);
    set_value(5000);
    set_weight(3);
    set_hit_func(this_object());
}

mixed weapon_hit(object attacker) {
    int r;
    r = random(100);
    if (r < 10) {
        tell_object(environment(this_object()),
            "The staff unleashes a bolt of lightning!\n");
        tell_object(attacker,
            "A bolt of lightning strikes you from the wizard's staff!\n");
        return random(15) + 10;
    }
    if (r < 30) {
        tell_object(environment(this_object()),
            "The runes on your staff flare with arcane power.\n");
        return random(8) + 3;
    }
    return 0;
}

void init() {
    ::init();
    add_action("go_workshop", "workshop");
}

int drop(int silently) {
    ::drop(silently);
    write("The staff crumbles to dust as it leaves your grasp.\n");
    call_out("self_destruct", 0);
    return 0;
}

void self_destruct() {
    destruct(this_object());
}

int go_workshop() {
    if (this_player()->query_real_name() != "peace") {
        write("The staff does not respond to you.\n");
        return 1;
    }
    if (!wielded) {
        write("You must wield the staff first.\n");
        return 1;
    }
    write("The staff glows brightly and you are whisked away!\n");
    say(this_player()->query_name() + " vanishes in a flash of light.\n");
    this_player()->move_player("to players/peace/workshop#players/peace/workshop");
    return 1;
}
