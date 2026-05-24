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
#include "nifmmitm_service_impl.hpp"
#include "nifmmitm_debug.hpp"

namespace ams::mitm::nifm {

    Result NifmMitmService::AtmosphereReloadHostsForNifm() {
        /* The DNS MitM already owns /atmosphere/hosts/default.txt and reloads
         * itself when its own AtmosphereReloadHostsFile is invoked. This
         * command is reserved for the future nifm URL map and currently
         * does nothing. */
        LogDebug("[nifm_mitm] AtmosphereReloadHostsForNifm: noop (handled by dns_mitm)\n");
        R_SUCCEED();
    }

}
