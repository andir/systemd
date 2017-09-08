#pragma once

/***
  This file is part of systemd.

  Copyright 2017 Andreas Rammhold <andreas@rammhold.de>

  systemd is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  systemd is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with systemd; If not, see <http://www.gnu.org/licenses/>.
***/

#include "netdev/netdev.h"

typedef enum BatmanGatewayMode {
        NETDEV_BATMAN_GW_MODE_OFF,
        NETDEV_BATMAN_GW_MODE_CLIENT,
        NETDEV_BATMAN_GW_MODE_SERVER
} BatmanGatewayMode;



typedef struct Batman {
        NetDev meta;

        BatmanGatewayMode gateway_mode;
/* FIXME: add the remaining fields and stuff...
        bool dat;
        bool mm_mode;
*/
} Batman;

DEFINE_NETDEV_CAST(BATMAN, Batman);
extern const NetDevVTable batman_vtable;


const char* netdev_batman_gateway_mode_to_string(BatmanGatewayMode mode);

int config_parse_batman_gateway_mode(const char *unit,
                                     const char *filename,
                                     unsigned line,
                                     const char *section,
                                     unsigned section_line,
                                     const char *lvalue,
                                     int ltype,
                                     const char *rvalue,
                                     void *data,
                                     void *userdata);
