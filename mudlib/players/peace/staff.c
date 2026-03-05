inherit "obj/weapon";

void reset(int arg) {
    ::reset(arg);
    if (arg) return;
    set_name("staff");
    set_alt_name("wizard staff");
    set_alias("wizard");
    set_short("A wizard's staff");
    set_long("A gnarled wooden staff thrumming with arcane energy.\n" +
             "Runes carved along its length glow faintly.\n");
    set_class(10);
    set_value(500);
    set_weight(3);
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
