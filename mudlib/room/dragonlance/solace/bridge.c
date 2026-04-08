/*
 * Solace Bridge - Rope bridge connecting the gate stairs to the village.
 */

inherit "room/room";

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "A rope bridge";
	long_desc =
"You are on a swaying rope bridge strung between two massive vallenwood\n" +
"trees, high above the forest floor. The bridge is sturdily built with\n" +
"thick rope rails and wooden planks, but it sways gently in the breeze.\n" +
"Far below, the ground is barely visible through the leaves. The bridge\n" +
"leads north to the central square of Solace. Stairs spiral down to\n" +
"the gate below.\n";
	dest_dir = ({
	    "room/dragonlance/solace/gate", "down",
	    "room/dragonlance/solace/square", "north"
	});
	items = ({
	    "bridge", "A well-maintained rope bridge with thick rails and sturdy planks",
	    "planks", "Solid wooden planks, worn smooth by many feet",
	    "ground", "The forest floor is dizzily far below"
	});
    }
}
