/*
 * The Disks of Mishakal Quest Controller (singleton)
 *
 * Tracks the active quest player and state for the Xak Tsaroth quest.
 * Only one player can run the quest at a time.
 *
 * Stages:
 *   0 = inactive
 *   1 = player talked to Riverwind, received rope
 *   2 = rope used at well, descended underground
 *   3 = reached dragon lair, Khisanth spawned
 *   4 = Khisanth defeated, disks dropped
 */

object quest_player;
int quest_stage;
int quest_start_time;

void reset(int arg) {
    if (quest_player && quest_stage > 0) {
	if (!quest_player || !interactive(quest_player) ||
	    (time() - quest_start_time) > 1800) {
	    quest_player = 0;
	    quest_stage = 0;
	    quest_start_time = 0;
	}
    }
}

int start_quest(object player) {
    if (quest_stage > 0 && quest_player && interactive(quest_player) &&
	(time() - quest_start_time) <= 1800)
	return 0;
    quest_player = player;
    quest_stage = 1;
    quest_start_time = time();
    return 1;
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

int is_quest_player(object player) {
    return quest_player && quest_player == player &&
	   quest_stage > 0;
}
