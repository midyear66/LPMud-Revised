/*
 * Temple Entry - Entry hall of the Temple of the Dark Queen.
 * Fizban (Paladine in disguise) gives Quest 4.
 */

inherit "room/room";

object fizban;

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "Temple entrance hall";
	long_desc =
"You stand in the entrance hall of the Temple of the Dark Queen.\n" +
"The air itself feels heavy with dark power. Black marble pillars\n" +
"rise to a ceiling lost in shadow, and the walls are carved with\n" +
"writhing dragon reliefs that seem to move in the flickering\n" +
"torchlight. Despite the oppressive darkness, a small circle of\n" +
"warm golden light surrounds an old man sitting on a fallen\n" +
"pillar near the entrance. The temple nave lies to the north.\n";
	dest_dir = ({
	    "room/dragonlance/temple/nave", "north"
	});
	items = ({
	    "pillars", "Black marble pillars reaching into the shadowed ceiling",
	    "reliefs", "Writhing dragon carvings that seem to shift and move",
	    "light", "A small circle of warm golden light surrounding the old man",
	    "ceiling", "Lost in shadow far above"
	});
    }
    if (!fizban || !living(fizban)) {
	fizban = clone_object("obj/monster");
	fizban->set_name("fizban");
	fizban->set_alias("old man");
	fizban->set_alt_name("wizard");
	fizban->set_short("Fizban the Fabulous");
	fizban->set_long(
"A doddering old man in tattered, singed grey robes and a battered\n" +
"pointed hat that keeps sliding over his eyes. His long white beard\n" +
"is tangled and his pockets bulge with odd trinkets. He looks\n" +
"thoroughly confused about where he is, yet an inexplicable sense\n" +
"of vast power surrounds him. His eyes, when you catch a glimpse\n" +
"of them beneath the hat brim, are surprisingly bright and kind.\n");
	fizban->set_level(1);
	fizban->set_hp(50);
	fizban->set_al(500);
	fizban->set_aggressive(0);
	move_object(fizban, this_object());
	fizban->set_match(this_object(),
	    ({ "quest_talk" }),
	    ({ "says:" }),
	    ({ 0 })
	);
	fizban->load_chat(15, ({
	    "Fizban says: Now where did I put that... oh dear.\n",
	    "Fizban's hat slides over his eyes and he bumps into a pillar.\n",
	    "Fizban says: I used to know a fireball spell... or was it a flower spell?\n",
	    "Fizban pulls a live chicken from his pocket and looks confused.\n",
	    "Fizban says: Terrible place, this. Reminds me of somewhere... can't recall.\n"
	}));
    }
    /* Load the quest controller */
    if (!find_object("obj/dl_takhisis_quest"))
	call_other("obj/dl_takhisis_quest", "reset", 0);

    /* Register quest in quest_room */
    if (find_object("room/quest_room")) {
	object ob;
	int found;
	ob = first_inventory(find_object("room/quest_room"));
	while (ob) {
	    if (ob->id("defeat_takhisis")) {
		found = 1;
		break;
	    }
	    ob = next_inventory(ob);
	}
	if (!found) {
	    ob = clone_object("obj/dl_takhisis_quest_obj");
	    move_object(ob, find_object("room/quest_room"));
	}
    }
}

void give_quest_reward(object player) {
    object lance, medallion;

    if (!player)
	return;
    player->add_exp(6000);
    player->set_quest("defeat_takhisis");

    lance = clone_object("obj/weapon");
    lance->set_name("dragonlance");
    lance->set_alias("lance");
    lance->set_short("A Dragonlance");
    lance->set_long(
"The legendary Dragonlance, forged by the Silver Arm of Ergoth\n" +
"with the blessing of Paladine himself. The lance gleams with\n" +
"holy silver light and thrums with divine power. It is the\n" +
"ultimate weapon against dragonkind and the forces of evil.\n");
    lance->set_class(20);
    lance->set_value(5000);
    lance->set_weight(6);
    move_object(lance, player);

    medallion = clone_object("obj/armour");
    medallion->set_id("medallion");
    medallion->set_alias("medallion of faith");
    medallion->set_type("amulet");
    medallion->set_short("A Medallion of Faith");
    medallion->set_long(
"A golden medallion inscribed with the symbol of Paladine, the\n" +
"platinum dragon god. It radiates warmth and protection, a\n" +
"constant reminder of the true gods' return to Krynn.\n");
    medallion->set_ac(3);
    medallion->set_value(2000);
    medallion->set_weight(1);
    move_object(medallion, player);

    tell_object(player,
"You feel incredibly experienced from your legendary deed.\n");
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

    if (sscanf(rest, "%shello%s", a, b) != 2 &&
	sscanf(rest, "%shelp%s", a, b) != 2 &&
	sscanf(rest, "%stakhisis%s", a, b) != 2 &&
	sscanf(rest, "%stemple%s", a, b) != 2 &&
	sscanf(rest, "%sgem%s", a, b) != 2 &&
	sscanf(rest, "%spaladine%s", a, b) != 2 &&
	sscanf(rest, "%squest%s", a, b) != 2 &&
	sscanf(rest, "%shi%s", a, b) != 2)
	return;

    controller = find_object("obj/dl_takhisis_quest");
    if (!controller) {
	"obj/dl_takhisis_quest"->reset(0);
	controller = find_object("obj/dl_takhisis_quest");
    }

    if (player->query_quests("defeat_takhisis")) {
	tell_object(player,
"Fizban straightens up, and for a moment you see not a doddering\n" +
"old fool but Paladine himself, radiant with divine light.\n" +
"Fizban says: Well done, champion. The world is safe... for now.\n" +
"He winks and his hat slides over his eyes again.\n");
	return;
    }

    if (controller->query_stage() > 0 &&
	!controller->is_quest_player(player)) {
	tell_object(player,
"Fizban says: Oh my, someone else is already bumbling about in\n" +
"there. Best wait your turn, yes?\n");
	return;
    }

    if (controller->is_quest_player(player)) {
	int stage;
	stage = controller->query_stage();
	if (stage >= 1 && stage <= 3) {
	    tell_object(player,
"Fizban says: The gem! Use it at the altar! Or was it the\n" +
"kitchen? No no, the altar. Definitely the altar. I think.\n");
	    return;
	}
	if (stage == 4) {
	    controller->set_stage(0);
	    tell_object(player,
"As you approach Fizban, the old man rises to his full height.\n" +
"His tattered robes transform into robes of blinding platinum\n" +
"light. His eyes blaze with divine radiance. He is no longer\n" +
"Fizban -- he is PALADINE, Father of Good, the Platinum Dragon.\n\n" +
"Paladine speaks: Well done, mortal champion. You have driven\n" +
"back the avatar of Takhisis and sealed the portal between\n" +
"worlds. The Dark Queen's return has been delayed -- though\n" +
"not forever. For your courage, I bestow upon you the greatest\n" +
"weapons against evil: a true Dragonlance and a Medallion of\n" +
"Faith. Carry them with honor.\n\n" +
"A flash of platinum light fills the temple. When it fades,\n" +
"Fizban is just a confused old man again.\n" +
"Fizban says: Oh my. Did something happen? I must have dozed off.\n");
	    say("A blinding platinum light fills the temple!\n" +
		"When it fades, Fizban hands something to " +
		player->query_name() + ".\n", player);
	    give_quest_reward(player);
	    return;
	}
	return;
    }

    /* Start the quest */
    if (controller->start_quest(player)) {
	object gem;
	gem = clone_object("obj/treasure");
	gem->set_id("gem");
	gem->set_alias("gem of paladine");
	gem->set_short("The Gem of Paladine");
	gem->set_long(
"A flawless diamond that blazes with an inner platinum light.\n" +
"It is warm to the touch and fills you with courage and hope.\n" +
"This is the Gem of Paladine, a fragment of divine power.\n");
	gem->set_value(0);
	gem->set_weight(1);
	move_object(gem, player);
	tell_object(player,
"Fizban fumbles with his hat, then suddenly looks at you with\n" +
"startling clarity.\n" +
"Fizban says: Ah! You! Yes, you. Listen carefully -- I don't\n" +
"often have moments of clarity and this one might not last.\n" +
"Takhisis, the Dark Queen, is trying to break through into the\n" +
"mortal world through this very temple. Her avatar grows more\n" +
"powerful with each passing moment.\n" +
"Take this gem -- yes, from my pocket, careful of the chicken --\n" +
"take it to the altar in the temple. Place it there to break the\n" +
"dark barrier. Then find the throne room and confront her avatar.\n" +
"Defeat it, and the portal closes. Fail, and... well, let's not\n" +
"think about that.\n" +
"Fizban hands you a gleaming diamond that pulses with platinum light.\n" +
"Fizban says: Oh dear, there goes the clarity. What were we..?\n");
	say("Fizban rummages in his pockets and hands something shiny to " +
	    player->query_name() + ".\n", player);
    }
}
