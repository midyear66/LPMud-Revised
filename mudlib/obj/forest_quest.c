/*
 * Forest Remedy Quest Controller (singleton)
 *
 * Tracks the active quest player and state for the wounded traveler
 * fetch quest. Only one player can run the quest at a time.
 *
 * Stages:
 *   0 = inactive
 *   1 = player talked to traveler
 *   2 = player talked to doctor, herb spawned
 *   3 = player picked up herb
 *   4 = player gave herb to doctor, has medicine
 *   5 = complete (resets to 0)
 */

object quest_player;
int quest_stage;
int quest_start_time;
object herb_obj;

string *forest_rooms;

void reset(int arg) {
    if (!arg) {
	forest_rooms = ({
	    "room/forest1", "room/forest2", "room/forest3",
	    "room/forest4", "room/forest5", "room/forest6",
	    "room/forest7", "room/forest8", "room/forest9",
	    "room/forest10", "room/forest11", "room/forest12",
	    "room/clearing"
	});
    }
    /* Clear stale quests: player gone or timed out after 30 minutes */
    if (quest_player && quest_stage > 0 && quest_stage < 5) {
	if (!quest_player || !interactive(quest_player) ||
	    (time() - quest_start_time) > 1800) {
	    quest_player = 0;
	    quest_stage = 0;
	    quest_start_time = 0;
	    if (herb_obj)
		destruct(herb_obj);
	    herb_obj = 0;
	}
    }
}

int start_quest(object player) {
    if (quest_stage > 0 && quest_player && interactive(quest_player) &&
	(time() - quest_start_time) <= 1800)
	return 0;
    /* Reset any stale state */
    if (herb_obj)
	destruct(herb_obj);
    herb_obj = 0;
    quest_player = player;
    quest_stage = 1;
    quest_start_time = time();
    return 1;
}

void spawn_herb() {
    string room_name;
    object room_ob;

    if (herb_obj)
	destruct(herb_obj);
    room_name = forest_rooms[random(sizeof(forest_rooms))];
    room_ob = find_object(room_name);
    if (!room_ob) {
	call_other(room_name, "short");  /* Force load */
	room_ob = find_object(room_name);
    }
    if (!room_ob)
	return;  /* Room failed to load */
    herb_obj = clone_object("obj/silverleaf");
    move_object(herb_obj, room_ob);
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

object query_herb() {
    return herb_obj;
}

int is_quest_player(object player) {
    return quest_player && quest_player == player &&
	   quest_stage > 0 && quest_stage < 5;
}

void complete_quest() {
    object gift;

    if (!quest_player)
	return;
    quest_player->add_exp(500);
    quest_player->set_quest("forest_remedy");
    /* Drop a thank-you gift */
    gift = clone_object("obj/treasure");
    gift->set_id("gift");
    gift->set_alias("thank-you gift");
    gift->set_short("A small thank-you gift");
    gift->set_long("A small pouch of coins, given in gratitude by a traveler you saved.\nYou could sell this at the shop.\n");
    gift->set_value(100);
    gift->set_weight(1);
    move_object(gift, quest_player);
    tell_object(quest_player, "The traveler hands you a small pouch of coins.\n");
    quest_player = 0;
    quest_stage = 0;
    quest_start_time = 0;
    herb_obj = 0;
}
