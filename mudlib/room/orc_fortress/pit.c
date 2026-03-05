#include "room/room.h"
#undef EXTRA_RESET
#define EXTRA_RESET extra_reset();

void extra_reset() {
    object wolf;
    int i;

    if (!present("warg")) {
        i = 0;
        while (i < 2) {
            i += 1;
            wolf = clone_object("obj/monster");
            wolf->set_name("warg");
            wolf->set_alias("orc wolf");
            wolf->set_alt_name("wolf");
            wolf->set_race("wolf");
            wolf->set_level(4);
            wolf->set_hp(45);
            wolf->set_wc(12);
            wolf->set_al(-40);
            wolf->set_short("A warg");
            wolf->set_long("A massive wolf with matted black fur and glowing yellow eyes.\n" +
                           "It snarls and snaps, straining against its chains.\n");
            wolf->set_aggressive(1);
            wolf->load_a_chat(40, ({
                "The warg snarls viciously.\n",
                "The warg lunges at you, teeth bared.\n",
                "The warg howls, a chilling sound.\n"
            }));
            move_object(wolf, this_object());
        }
    }
}

THREE_EXIT("room/orc_fortress/mess", "north",
     "room/orc_fortress/hall", "east",
     "room/orc_fortress/stairs", "south",
     "The warg pit",
     "A sunken pit in the center of the fortress serves as a pen for\n" +
     "the orcs' war beasts. The ground is torn up by massive claws\n" +
     "and littered with bones. Heavy iron chains are bolted to stakes\n" +
     "driven deep into the earth. The stench of the animals is overpowering.\n", 1)
