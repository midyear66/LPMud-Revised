/*
 * The Fairy Plague Quest Controller (singleton)
 *
 * Tracks the active quest player and state for the fairy plague quest.
 * Only one player can run the quest at a time.
 *
 * Stages:
 *   0 = inactive
 *   1 = player talked to sick fairy
 *   2 = player talked to fairy elder, moonpetal spawned
 *   3 = player picked up moonpetal
 *   4 = player gave moonpetal to elder, has purifying draught
 *   5 = player poured draught on runestone, sprite spawned
 *   6 = sprite defeated, return to elder for reward
 *   7 = complete (resets to 0)
 */

object quest_player;
int quest_stage;
int quest_start_time;
object moonpetal_obj;

string *forest_rooms;

void reset(int arg) {
    if (!arg) {
	forest_rooms = ({
	    "room/south/sforst10", "room/south/sforst11",
	    "room/south/sforst12", "room/south/sforst13",
	    "room/south/sforst14", "room/south/sforst15",
	    "room/south/sforst16", "room/south/sforst17",
	    "room/south/sforst18", "room/south/sforst19",
	    "room/south/sforst20", "room/south/sforst21",
	    "room/south/sforst22", "room/south/sforst23",
	    "room/south/sforst24", "room/south/sforst25",
	    "room/south/sforst26", "room/south/sforst27",
	    "room/south/sforst28", "room/south/sforst29",
	    "room/south/sforst30", "room/south/sforst31",
	    "room/south/sforst32", "room/south/sforst33",
	    "room/south/sforst34", "room/south/sforst35",
	    "room/south/sforst36", "room/south/sforst37",
	    "room/south/sforst38", "room/south/sforst39",
	    "room/south/sforst40", "room/south/sforst41",
	    "room/south/sforst42", "room/south/sforst43",
	    "room/south/sforst44", "room/south/sforst45",
	    "room/south/sforst46", "room/south/sforst47",
	    "room/south/sforst48", "room/south/sforst49"
	});
    }
    /* Clear stale quests: player gone or timed out after 30 minutes */
    if (quest_player && quest_stage > 0 && quest_stage < 7) {
	if (!quest_player || !interactive(quest_player) ||
	    (time() - quest_start_time) > 1800) {
	    quest_player = 0;
	    quest_stage = 0;
	    quest_start_time = 0;
	    if (moonpetal_obj)
		destruct(moonpetal_obj);
	    moonpetal_obj = 0;
	}
    }
}

int start_quest(object player) {
    if (quest_stage > 0 && quest_player && interactive(quest_player) &&
	(time() - quest_start_time) <= 1800)
	return 0;
    /* Reset any stale state */
    if (moonpetal_obj)
	destruct(moonpetal_obj);
    moonpetal_obj = 0;
    quest_player = player;
    quest_stage = 1;
    quest_start_time = time();
    return 1;
}

void spawn_moonpetal() {
    string room_name;
    object room_ob;

    if (moonpetal_obj)
	destruct(moonpetal_obj);
    room_name = forest_rooms[random(sizeof(forest_rooms))];
    room_ob = find_object(room_name);
    if (!room_ob) {
	call_other(room_name, "short");  /* Force load */
	room_ob = find_object(room_name);
    }
    if (!room_ob)
	return;  /* Room failed to load */
    moonpetal_obj = clone_object("obj/moonpetal");
    move_object(moonpetal_obj, room_ob);
}

int query_stage() {
    return quest_stage;
}

void set_stage(int s) {
    quest_stage = s;
}

object query_player() {
    return quest_player;
}

object query_moonpetal() {
    return moonpetal_obj;
}

int is_quest_player(object player) {
    return quest_player && quest_player == player &&
	   quest_stage > 0 && quest_stage < 7;
}

void complete_quest() {
    object amulet;

    if (!quest_player)
	return;
    quest_player->add_exp(1000);
    quest_player->set_quest("fairy_plague");
    /* Give fairy amulet reward */
    amulet = clone_object("obj/armour");
    amulet->set_id("amulet");
    amulet->set_alias("fairy amulet");
    amulet->set_short("A fairy amulet");
    amulet->set_long(
"A delicate amulet woven from silver threads and tiny moonpetals.\n" +
"It shimmers with a faint protective glow, a gift from the fairy\n" +
"elder in gratitude for saving the colony from the plague.\n");
    amulet->set_value(200);
    amulet->set_weight(1);
    amulet->set_type("amulet");
    amulet->set_ac(1);
    move_object(amulet, quest_player);
    tell_object(quest_player,
	"The fairy elder places a shimmering amulet around your neck.\n");
    quest_player = 0;
    quest_stage = 0;
    quest_start_time = 0;
    moonpetal_obj = 0;
}
