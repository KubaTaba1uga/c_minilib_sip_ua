/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#include <unistd.h>

#include "c_minilib_mock.h"

#include "event_loop/_internal/poll_fd.h"

MOCKABLE(void cmsu_PollFd_drop(cmsu_PollFd *self)) { close(self->fd); }
