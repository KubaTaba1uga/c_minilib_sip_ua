/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_SMART_PTR_H
#define C_MINILIB_SIP_UA_SMART_PTR_H

/* #define SP_GET_VALUE(sp, name) (sp).get->name */
#define SP_GET_PTR(sp, name) &(*(sp).get)->name
#define SP_SET_VALUE(sp, name, value) (*(sp).get)->name = value

#endif
