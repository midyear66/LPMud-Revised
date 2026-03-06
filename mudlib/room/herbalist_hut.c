/*
 * Herbalist's hut - south of forest6.
 * Old Marta hints at where to find silverleaf for the Forest Remedy quest.
 */

inherit "room/room";

object marta;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "Herbalist's hut";
	long_desc =
"A cozy hut nestled among the trees, filled with the rich aroma of\n" +
"dried herbs. Bundles of plants hang from the rafters, and a small\n" +
"cauldron bubbles gently over a low fire.\n";
	dest_dir = ({ "room/forest6", "north" });
	items = ({
	    "herbs", "Bundles of dried herbs of every variety hang from the rafters",
	    "cauldron", "A small iron cauldron bubbles with a fragrant brew",
	    "fire", "A low fire crackles beneath the cauldron",
	    "rafters", "Wooden rafters strung with bundles of drying herbs"
	});
    }
    if (!marta || !living(marta)) {
	marta = clone_object("obj/monster");
	marta->set_name("marta");
	marta->set_alias("herbalist");
	marta->set_alt_name("old marta");
	marta->set_short("Old Marta the herbalist");
	marta->set_long(
"A weathered old woman surrounded by bundles of dried herbs. Her eyes\n" +
"are sharp despite her age.\n");
	marta->set_level(3);
	marta->set_al(150);
	marta->set_aggressive(0);
	move_object(marta, this_object());
	marta->set_match(this_object(),
	    ({ "quest_talk" }),
	    ({ "says:" }),
	    ({ 0 })
	);
	marta->load_chat(3, ({
	    "Old Marta mutters about the quality of this season's herbs.\n",
	    "Old Marta says: The forest provides everything we need, if you know where to look.\n",
	    "Old Marta stirs her cauldron thoughtfully.\n"
	}));
    }
}

void quest_talk(string str) {
    object controller;
    object player;
    string who, rest;
    string a, b;

    if (sscanf(str, "%s says: %s\n", who, rest) != 2)
	return;

    player = find_living(lower_case(who));
    if (!player)
	return;

    /* Only respond to quest-related keywords */
    if (sscanf(rest, "%ssilverleaf%s", a, b) != 2 &&
	sscanf(rest, "%sherb%s", a, b) != 2 &&
	sscanf(rest, "%splant%s", a, b) != 2 &&
	sscanf(rest, "%smedicine%s", a, b) != 2 &&
	sscanf(rest, "%shelp%s", a, b) != 2 &&
	sscanf(rest, "%shello%s", a, b) != 2 &&
	sscanf(rest, "%shi%s", a, b) != 2)
	return;

    controller = find_object("obj/forest_quest");
    if (!controller) {
	call_other("obj/forest_quest", "reset", 0);
	controller = find_object("obj/forest_quest");
    }

    if (controller && controller->is_quest_player(player) &&
	controller->query_stage() >= 2) {
	tell_object(player,
"Old Marta says: Silverleaf? Aye, I know it well. A rare herb with\n" +
"silver-veined leaves. It grows in the deep parts of the forest and\n" +
"near the clearings. Search carefully -- it shimmers faintly in the\n" +
"light.\n");
	return;
    }

    tell_object(player,
"Old Marta says: Ah, just an old woman tending her herbs. The forest\n" +
"holds many secrets for those who know where to look.\n");
}
