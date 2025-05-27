#include "event_loop/poll/pollfds.h"

MOCKABLE(void cmsu_PollFd_drop(cmsu_PollFd *self)) { close(self->fd); }
