/*
 * Copyright (c) Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stratosphere.hpp>
#include "nimmitm_service_impl.hpp"
#include "nimmitm_debug.hpp"

namespace ams::mitm::nim {

    Result NimMitmService::AtmosphereReloadNimRedirects() {
        /* Placeholder for the future nim URL-redirect table. The hosts file
         * that backs dns.mitm already steers nim's HTTPS calls; this command
         * exists so userspace tooling has a single entrypoint once nim-side
         * rewrites land. */
        LogDebug("[nim_mitm] AtmosphereReloadNimRedirects: noop (handled today via dns.mitm + ssl.mitm)\n");
        R_SUCCEED();
    }

}
