#include "event_loop/linux_poll.h"

MOCKABLE(void cmsu_PollFd_drop(cmsu_PollFd *self)) { close(self->fd); }
