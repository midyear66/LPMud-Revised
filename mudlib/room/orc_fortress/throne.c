inherit "room/room";

object king;
object shaman;

void reset(int arg) {
    object weapon, armour;

    if (!arg) {
        set_light(1);
        short_desc = "The throne room";
        long_desc =
            "A vast underground chamber lit by green-flamed braziers. At the\n" +
            "far end, a throne of blackened iron and bone rises on a platform\n" +
            "of skulls. Tattered war banners hang from the ceiling. The walls\n" +
            "are carved with crude depictions of orc victories over humans,\n" +
            "elves, and dwarves. This is the seat of power of the orc king.\n";
        dest_dir = ({ "room/orc_fortress/stairs", "north" });
    }

    if (!king || !living(king)) {
        king = clone_object("obj/monster");
        king->set_name("orc king");
        king->set_alias("king");
        king->set_alt_name("orc");
        king->set_race("orc");
        king->set_level(10);
        king->set_hp(150);
        king->set_wc(18);
        king->set_ac(3);
        king->set_al(-100);
        king->set_short("The orc king");
        king->set_long(
            "A towering orc clad in black iron plate armor. A crown of\n" +
            "twisted metal and jagged teeth sits upon his scarred brow.\n" +
            "His eyes burn with malice and cunning. He grips a massive\n" +
            "war axe in his gnarled fists.\n");
        king->set_aggressive(1);
        king->set_chance(20);
        king->set_spell_mess1("The orc king roars and the ground trembles beneath you!\n");
        king->set_spell_mess2("The orc king roars and the ground trembles beneath you!\n");
        king->set_spell_dam(15);
        king->load_a_chat(40, ({
            "Orc king says: I will wear your skull as a trophy!\n",
            "Orc king says: Kneel before me or die!\n",
            "Orc king bellows: MY FORTRESS! MY KINGDOM!\n",
            "Orc king says: You are nothing! NOTHING!\n",
            "The orc king's eyes blaze with fury.\n"
        }));
        weapon = clone_object("obj/weapon");
        weapon->set_name("war axe");
        weapon->set_class(18);
        weapon->set_value(200);
        weapon->set_weight(4);
        weapon->set_alt_name("axe");
        transfer(weapon, king);
        command("wield war axe", king);
        move_object(king, this_object());
    }

    if (!shaman || !living(shaman)) {
        shaman = clone_object("obj/monster");
        shaman->set_name("orc shaman");
        shaman->set_alias("shaman");
        shaman->set_alt_name("orc");
        shaman->set_race("orc");
        shaman->set_level(7);
        shaman->set_hp(80);
        shaman->set_al(-90);
        shaman->set_short("An orc shaman");
        shaman->set_long(
            "A hunched orc draped in fetishes and animal bones. His eyes\n" +
            "are milky white and he mutters dark incantations constantly.\n");
        shaman->set_aggressive(1);
        shaman->set_chance(35);
        shaman->set_spell_mess1("The orc shaman hurls a bolt of dark energy at you!\n");
        shaman->set_spell_mess2("The orc shaman hurls a bolt of dark energy at you!\n");
        shaman->set_spell_dam(12);
        shaman->load_a_chat(40, ({
            "Orc shaman cackles and waves his staff.\n",
            "Orc shaman says: The dark spirits hunger for your soul!\n",
            "Orc shaman chants in a guttural tongue.\n"
        }));
        weapon = clone_object("obj/weapon");
        weapon->set_name("gnarled staff");
        weapon->set_class(10);
        weapon->set_value(60);
        weapon->set_weight(2);
        weapon->set_alt_name("staff");
        transfer(weapon, shaman);
        command("wield gnarled staff", shaman);
        move_object(shaman, this_object());
    }
}

void init() {
    ::init();
    add_action("search_throne", "search");
}

int search_throne(string str) {
    object treasure;

    if (str != "throne" && str != "skulls" && str != "platform")
        return 0;

    if (king && living(king)) {
        write("You can't search while the orc king still lives!\n");
        return 1;
    }

    if (present("crown", this_object())) {
        write("You find nothing else of interest.\n");
        return 1;
    }

    write("Behind the throne you find a hidden compartment containing\n" +
          "the orc king's iron crown!\n");
    treasure = clone_object("obj/treasure");
    treasure->set_id("crown");
    treasure->set_alias("iron crown");
    treasure->set_short("The orc king's iron crown");
    treasure->set_long("A crown of twisted black iron set with jagged teeth and\n" +
                       "crude red gemstones. It radiates a faint aura of malice.\n");
    treasure->set_value(800);
    treasure->set_weight(2);
    move_object(treasure, this_object());
    return 1;
}
