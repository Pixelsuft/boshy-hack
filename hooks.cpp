#include "hooks.hpp"
#include "mouse.hpp"
#include "msg.hpp"
#include "rng.hpp"
#include "ren.hpp"

void init_hooks() {
	init_mouse();
	init_msg();
	init_rng();
    init_ren();
}
