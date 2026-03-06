/*
 * The Restless Dead Quest Controller (singleton)
 *
 * Tracks the active quest player and state for the crypt quest.
 * Only one player can run the quest at a time.
 *
 * Stages:
 *   0 = inactive
 *   1 = player talked to Cedric, received holy water
 *   2 = holy water used on tomb, wraith spawned
 *   3 = wraith defeated, tomb sealed
 *   4 = complete (resets to 0)
 */

object quest_player;
int quest_stage;
int quest_start_time;

void reset(int arg) {
    /* Clear stale quests: player gone or timed out after 30 minutes */
    if (quest_player && quest_stage > 0 && quest_stage < 4) {
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
	   quest_stage > 0 && quest_stage < 4;
}

void complete_quest() {
    object ring;

    if (!quest_player)
	return;
    quest_player->add_exp(750);
    /* Give ring before setting quest flag, so if anything fails the
       player can retry and won't be locked out of the reward. */
    ring = clone_object("obj/treasure");
    ring->set_id("ring");
    ring->set_alias("silver ring");
    ring->set_short("An enchanted silver ring");
    ring->set_long("A silver ring etched with holy symbols, glowing faintly with\nprotective enchantment. You could sell this at the shop.\n");
    ring->set_value(150);
    ring->set_weight(1);
    move_object(ring, quest_player);
    tell_object(quest_player, "Brother Cedric places a silver ring in your hand.\n");
    quest_player->set_quest("restless_dead");
    quest_player = 0;
    quest_stage = 0;
    quest_start_time = 0;
}
