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

#include <linux/libbpf.h>

#include "bpf-program.h"
#include "bpf-support.h"
#include "cgroup-util.h"
#include "fd-util.h"
#include "log.h"

int bpf_supported(void) {
        static int supported = -1;
        int r;
        /* Checks whether BPF is supported. For this, we check three things:
         *
         * a) whether we are privileged
         * b) whether the unified hierarchy is being used
         */

        if (supported >= 0)
                return supported;

        if (geteuid() != 0)
                return supported = false;

        r = cg_unified_controller(SYSTEMD_CGROUP_CONTROLLER);
        if (r < 0)
                return log_error_errno(r, "Can't determine whether the unified hierarchy is used: %m");
        if (r == 0)
                return supported = false;

        return supported == true;
}

int bpf_map_supported(void) {
        static int supported = -1;
        int fd;
        /* Check if the the BPF implementation in the kernel supports BPF LPM TRIE
maps, which we require
        */
        if (supported >= 0)
                return supported;

        if (!bpf_supported())
                return supported = false;

        fd = bpf_map_new(BPF_MAP_TYPE_LPM_TRIE,
                         offsetof(struct bpf_lpm_trie_key, data) + sizeof(uint64_t),
                         sizeof(uint64_t),
                         1,
                         BPF_F_NO_PREALLOC);
        if (fd < 0) {
                log_debug_errno(fd, "Can't allocate BPF LPM TRIE map, BPF firewalling is not supported: %m");
                return supported = false;
        }

        safe_close(fd);

        return supported = true;
}