/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_UA_UTILS_SIP_STATUS_CODES_H
#define C_MINILIB_SIP_UA_UTILS_SIP_STATUS_CODES_H

// 1xx Provisional
#define SIP_STATUS_TRYING                     100
#define SIP_STATUS_RINGING                    180
#define SIP_STATUS_CALL_IS_BEING_FORWARDED    181
#define SIP_STATUS_QUEUED                     182
#define SIP_STATUS_PROGRESS                   183
#define SIP_STATUS_EARLY_DIALOG_TERMINATED    199

// 2xx Success
#define SIP_STATUS_OK                         200
#define SIP_STATUS_ACCEPTED                   202

// 3xx Redirection
#define SIP_STATUS_MULTIPLE_CHOICES           300
#define SIP_STATUS_MOVED_PERMANENTLY          301
#define SIP_STATUS_MOVED_TEMPORARILY          302
#define SIP_STATUS_USE_PROXY                  305
#define SIP_STATUS_ALTERNATIVE_SERVICE        380

// 4xx Client Error
#define SIP_STATUS_BAD_REQUEST                400
#define SIP_STATUS_UNAUTHORIZED               401
#define SIP_STATUS_PAYMENT_REQUIRED           402
#define SIP_STATUS_FORBIDDEN                  403
#define SIP_STATUS_NOT_FOUND                  404
#define SIP_STATUS_METHOD_NOT_ALLOWED         405
#define SIP_STATUS_NOT_ACCEPTABLE             406
#define SIP_STATUS_PROXY_AUTH_REQUIRED        407
#define SIP_STATUS_REQUEST_TIMEOUT            408
#define SIP_STATUS_GONE                       410
#define SIP_STATUS_REQUEST_ENTITY_TOO_LARGE   413
#define SIP_STATUS_REQUEST_URI_TOO_LONG       414
#define SIP_STATUS_UNSUPPORTED_MEDIA_TYPE     415
#define SIP_STATUS_UNSUPPORTED_URI_SCHEME     416
#define SIP_STATUS_BAD_EXTENSION              420
#define SIP_STATUS_EXTENSION_REQUIRED         421
#define SIP_STATUS_INTERVAL_TOO_BRIEF         423
#define SIP_STATUS_TEMPORARILY_UNAVAILABLE    480
#define SIP_STATUS_CALL_TRANSACTION_DOES_NOT_EXIST 481
#define SIP_STATUS_LOOP_DETECTED              482
#define SIP_STATUS_TOO_MANY_HOPS              483
#define SIP_STATUS_ADDRESS_INCOMPLETE         484
#define SIP_STATUS_AMBIGUOUS                  485
#define SIP_STATUS_BUSY_HERE                  486
#define SIP_STATUS_REQUEST_TERMINATED         487
#define SIP_STATUS_NOT_ACCEPTABLE_HERE        488
#define SIP_STATUS_BAD_EVENT                  489
#define SIP_STATUS_REQUEST_PENDING            491
#define SIP_STATUS_UNDECIPHERABLE             493

// 5xx Server Error
#define SIP_STATUS_SERVER_INTERNAL_ERROR      500
#define SIP_STATUS_NOT_IMPLEMENTED            501
#define SIP_STATUS_BAD_GATEWAY                502
#define SIP_STATUS_SERVICE_UNAVAILABLE        503
#define SIP_STATUS_SERVER_TIMEOUT             504
#define SIP_STATUS_VERSION_NOT_SUPPORTED      505
#define SIP_STATUS_MESSAGE_TOO_LARGE          513

// 6xx Global Failure
#define SIP_STATUS_BUSY_EVERYWHERE            600
#define SIP_STATUS_DECLINE                    603
#define SIP_STATUS_DOES_NOT_EXIST_ANYWHERE    604
#define SIP_STATUS_NOT_ACCEPTABLE_GLOBAL      606

#endif // C_MINILIB_SIP_UA_UTILS_SIP_STATUS_CODES_H
