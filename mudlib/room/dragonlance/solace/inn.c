/*
 * The Inn of the Last Home - Solace's famous inn, built in a vallenwood.
 * Serves Otik's famous spiced potatoes and ale.
 */

inherit "room/room";

#define Speak(s) write("Otik says: " + s + "\n")
#define Name this_player()->query_name()
#define Level this_player()->query_level()

int sp, mp, rp;
string last_eater;

void reset(int arg) {
    sp = 8;
    mp = 4;
    rp = 2;
    if (arg) return;
    set_light(1);
    short_desc = "The Inn of the Last Home";
    long_desc =
"You are inside the legendary Inn of the Last Home, built among the\n" +
"branches of the largest vallenwood tree in Solace. A great stone\n" +
"fireplace dominates the room, its chimney rising straight through\n" +
"the trunk of the tree. The air is thick with the delicious aroma\n" +
"of Otik's famous spiced potatoes. Wooden tables and chairs fill\n" +
"the room, and warm firelight dances on the curved wooden walls.\n" +
"Stairs lead up to the guest rooms. The village square is to the\n" +
"south.\n";
    dest_dir = ({
	"room/dragonlance/solace/square", "south",
	"room/dragonlance/solace/inn_upper", "up"
    });
    items = ({
	"fireplace", "A great stone fireplace built right into the tree trunk",
	"tables", "Sturdy wooden tables scarred by years of use",
	"menu", "The menu looks inviting"
    });
}

void init() {
    ::init();
    add_action("buy", "buy");
    add_action("buy", "order");
}

void show_menu() {
    write("\n");
    if (!(sp || mp || rp)) {
	Speak("We have completely sold out...come back later.");
    } else {
	write("1: Otik's Spiced Potatoes    " + sp + " at 15 gp\n");
	write("2: Roast Venison Platter     " + mp + " at 40 gp\n");
	write("3: Feast of the Companions   " + rp + " at 80 gp\n");
	write("4: Otik's Ale                      3 gp\n");
	write("\nUse 'buy <number>' to order. Food is served immediately.\n\n");
    }
}

void long(string s) {
    ::long(s);
    show_menu();
}

int no_food() {
    Speak("Sorry - we have sold out of that.");
    if (sp || mp || rp)
	Speak("Why don't you try something else?");
    else
	Speak("Come back later, friend.");
    return 1;
}

int pays(int n) {
    if (this_player()->query_money() < n) {
	Speak("You cannot afford that, friend.");
	return 0;
    }
    this_player()->add_money(0 - n);
    return 1;
}

int tease(int n) {
    if (Name == last_eater)
	Speak("My, we are hungry today!");
    last_eater = Name;
    this_player()->heal_self(n);
    return 1;
}

int buy(string str) {
    if (!str) {
	Speak("What would you like? Check the menu!");
	return 1;
    }
    /* Otik's Spiced Potatoes - 15gp, heals 4, levels 1-6 */
    if (str == "1" || str == "<1>" || str == "potatoes") {
	if (!sp) return no_food();
	if (!pays(15)) return 1;
	if (Level > 6) {
	    Speak("A seasoned adventurer like you deserves more!");
	    if (mp || rp) {
		Speak("Try one of our finer dishes.");
		return 1;
	    }
	    Speak("But this is all we have left - here you go.");
	}
	write("You are served a steaming plate of Otik's famous spiced potatoes.\n");
	say(Name + " digs into a plate of Otik's spiced potatoes.\n");
	sp = sp - 1;
	return tease(4);
    }
    /* Roast Venison - 40gp, heals 8, levels 7-12 */
    if (str == "2" || str == "<2>" || str == "venison") {
	if (!mp) return no_food();
	if (!pays(40)) return 1;
	if (Level > 12) {
	    Speak("You look like you need something more substantial!");
	    if (rp) {
		Speak("Try the Feast of the Companions.");
		return 1;
	    }
	    Speak("But this is all we have - enjoy.");
	}
	write("You are served a generous platter of roast venison with herbs.\n");
	say(Name + " enjoys a roast venison platter.\n");
	mp = mp - 1;
	return tease(8);
    }
    /* Feast of the Companions - 80gp, heals 12, levels 13+ */
    if (str == "3" || str == "<3>" || str == "feast") {
	if (!rp) return no_food();
	if (!pays(80)) return 1;
	write("You are served a magnificent feast fit for the Heroes of the Lance!\n");
	say(Name + " indulges in the Feast of the Companions.\n");
	rp = rp - 1;
	return tease(12);
    }
    /* Otik's Ale - 3gp */
    if (str == "4" || str == "<4>" || str == "ale") {
	if (!pays(3)) return 1;
	if (!this_player()->drink_alcohol(2)) {
	    Speak("You look a bit unsteady, friend. Let me take that back.");
	    this_player()->add_money(3);
	    return 1;
	}
	write("You drink a mug of Otik's dark ale. Smooth and warming!\n");
	say(Name + " enjoys a mug of Otik's ale.\n");
	return 1;
    }
    Speak("That's not on the menu. Try 1, 2, 3, or 4.");
    return 1;
}
