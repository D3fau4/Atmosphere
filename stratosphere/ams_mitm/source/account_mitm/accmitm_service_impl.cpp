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
#include "accmitm_service_impl.hpp"
#include "accmitm_debug.hpp"

namespace ams::mitm::account {

    Result AccountMitmService::AtmosphereReloadAccountOverrides() {
        /* Placeholder for the future account-override table loaded from
         * /atmosphere/account_overrides/. Once the IProfile proxy chain is
         * wired, this command will trigger a refresh without rebooting. */
        LogDebug("[account_mitm] AtmosphereReloadAccountOverrides: noop (no override table yet)\n");
        R_SUCCEED();
    }

}
