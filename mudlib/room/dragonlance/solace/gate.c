/*
 * Solace Gate - Base of the vallenwood trees, entrance to Solace.
 */

inherit "room/room";

void reset(int arg) {
    if (!arg) {
	set_light(1);
	short_desc = "The gate of Solace";
	long_desc =
"You stand at the base of the most enormous vallenwood trees you have\n" +
"ever seen. Their trunks are as wide as houses, and far above, an\n" +
"entire village nestles among the massive branches. A sturdy wooden\n" +
"staircase spirals up around the nearest trunk, leading to a rope\n" +
"bridge that connects the tree-top buildings. A sign reads:\n" +
"'Welcome to Solace - Home of the Inn of the Last Home'.\n" +
"The forest road lies to the east.\n";
	dest_dir = ({
	    "room/dragonlance/roads/road2", "east",
	    "room/dragonlance/solace/bridge", "up"
	});
	items = ({
	    "trees", "Enormous vallenwood trees, the largest you have ever seen",
	    "staircase", "A sturdy wooden staircase spiraling up around the trunk",
	    "sign", "A weathered sign reads: 'Welcome to Solace'",
	    "village", "An entire village built among the treetops, connected by rope bridges"
	});
    }
}
