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

#include "netdev/batadv.h"


static void batadv_init(NetDev *n) {
        _unused_ Batadv *b;

        b = BATMAN(n);
}

static int netdev_batadv_post_create(NetDev *netdev, Link *link, sd_netlink_message *m) {
        return 0;
}

const NetDevVTable batadv_vtable = {
        .object_size = sizeof(Batadv),
        .init = batadv_init,
        .sections = "Match\0NetDev\0Batadv\0",
        .post_create = netdev_batadv_post_create,
        .create_type = NETDEV_CREATE_MASTER,
};
