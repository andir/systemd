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

#include <inttypes.h>

#include "netdev/batadv.h"
#include "networkd-link.h"

#include "fileio.h"
#include "stdio-util.h"
#include "string-util.h"

const char* netdev_batadv_gateway_mode_to_string(BatadvGatewayMode mode) {
        if (mode == NETDEV_BATMAN_GW_MODE_OFF)
                return "off";

        if (mode == NETDEV_BATMAN_GW_MODE_CLIENT)
                return "client";

        if (mode == NETDEV_BATMAN_GW_MODE_SERVER)
                return "server";

        // FIXME: isn't there a better value? Does strjoina assert against NULL values?
        return "UNKOWN";
}

int config_parse_batadv_gateway_mode(const char *unit,
                                                   const char *filename,
                                                   unsigned line,
                                                   const char *section,
                                                   unsigned section_line,
                                                   const char *lvalue,
                                                   int ltype,
                                                   const char *rvalue,
                                                   void *data,
                                                   void *userdata) {

        Batadv *b = userdata;
        BatadvGatewayMode mode;

        assert(filename);
        assert(lvalue);
        assert(rvalue);
        assert(data);

        if (strcmp("off", rvalue) == 0)
                mode = NETDEV_BATMAN_GW_MODE_OFF;

        else if (strcmp("client", rvalue) == 0)
                mode = NETDEV_BATMAN_GW_MODE_CLIENT;

        else if (strcmp("server", rvalue) == 0)
                mode = NETDEV_BATMAN_GW_MODE_SERVER;

        else
                log_syntax(unit, LOG_ERR, filename, line, -EINVAL, "Batadv gateway mode '%s' is not valid. Value"
                                " should be 'off', 'client' or 'server'.", rvalue);

        b->gateway_mode = mode;

        return 0;
}

static void batadv_init(NetDev *n) {
        _unused_ Batadv *b;

        b = BATMAN(n);
}


static int netdev_batadv_post_create(NetDev *netdev, Link *link, sd_netlink_message *m) {
        Batadv *b;
        const char *p = NULL;
        char buf[DECIMAL_STR_MAX(uint32_t) * 2 + 2];
        int r;

        assert(netdev);

        b = BATMAN(netdev);

        /* Configure gateway mode */
        p = strjoina("/sys/class/net/", link->ifname, "/mesh/gw_mode");
        r = write_string_file(p, netdev_batadv_gateway_mode_to_string(b->gateway_mode), WRITE_STRING_FILE_VERIFY_ON_FAILURE);
        if (r < 0)
                log_link_warning_errno(link, r, "Cannot set gateway mode for interface: %m");


        /* Configure speeds */
        p = strjoina("/sys/class/net/", link->ifname, "/mesh/gw_bandwidth");
        xsprintf(buf, "%" PRIu32 "/%" PRIu32, b->gateway_bandwidth_down, b->gateway_bandwidth_up);
        r = write_string_file(p, buf, WRITE_STRING_FILE_VERIFY_ON_FAILURE);
        if (r < 0)
                log_link_warning_errno(link, r, "Canno set gateway bandwidth for interface: %m");


        /* Configure hop_penalty */
        p = strjoina("/sys/class/net/", link->ifname, "/mesh/hop_penalty");
        xsprintf(buf, "%u" , b->hop_penalty);
        r = write_string_file(p, buf, WRITE_STRING_FILE_VERIFY_ON_FAILURE);
        if (r < 0)
                log_link_warning_errno(link, r, "Cannot set hop_penalty for interface: %m");


        /* Configure orig interval */
        p = strjoina("/sys/class/net", link->ifname, "/mesh/orig_interval");
        xsprintf(buf, "%u", b->orig_interval);
        r = write_string_file(p, buf, WRITE_STRING_FILE_VERIFY_ON_FAILURE);
        if (r < 0)
                log_link_warning_errno(link, r, "Cannot set orig_interval for interface: %m");


        return 0;
}

const NetDevVTable batadv_vtable = {
        .object_size = sizeof(Batadv),
        .init = batadv_init,
        .sections = "Match\0NetDev\0Batadv\0",
        .post_create = netdev_batadv_post_create,
        .create_type = NETDEV_CREATE_MASTER,
};
