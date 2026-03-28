/*
 * Shutdown watcher daemon.
 *
 * Checks for a /shutdown_request file written by the admin web interface.
 * When found, triggers the Armageddon shutdown daemon to give players a
 * countdown warning before the game shuts down.
 *
 * The file should contain a single integer: the delay in minutes.
 * Do not clone this object.
 */

void reset(int arg) {
    if (arg)
	return;
    /* Clean up any stale request from a previous boot */
    rm("/shutdown_request");
    set_heart_beat(1);
}

void heart_beat() {
    string data;
    int delay;
    object arma;

    data = read_file("/shutdown_request");
    if (!data)
	return;

    rm("/shutdown_request");

    sscanf(data, "%d", delay);
    if (delay < 1)
	delay = 1;

    arma = find_object("obj/shut");
    if (!arma)
	arma = load_object("obj/shut");
    arma->shut(delay);

    /* Job done -- stop polling */
    set_heart_beat(0);
}
