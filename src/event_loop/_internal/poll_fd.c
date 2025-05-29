#include <unistd.h>

#include "c_minilib_mock.h"

#include "event_loop/_internal/poll_fd.h"

MOCKABLE(void cmsu_PollFd_drop(cmsu_PollFd *self)) { close(self->fd); }
