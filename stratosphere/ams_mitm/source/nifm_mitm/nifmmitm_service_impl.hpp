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
#pragma once
#include <stratosphere.hpp>

/* IPC interface for the nifm IGeneralService. The Atmosphère-extension
 * command (>= 65000) is reserved for the future URL-rewriting layer:
 * triggering it will eventually reload the nifm-specific redirection table.
 *
 * Standard nifm commands (CreateRequest, GetCurrentNetworkProfile, ...)
 * are auto-forwarded by the MitM framework. Intercepting them requires
 * wrapping the IRequest object returned by CreateRequest, which is parked
 * alongside the equivalent ssl proxy work.
 */
#define AMS_NIFM_MITM_INTERFACE_INFO(C, H)                                                                                                                              \
    AMS_SF_METHOD_INFO(C, H, 65000, Result, AtmosphereReloadHostsForNifm,   (),                                                                                  ())

AMS_SF_DEFINE_MITM_INTERFACE(ams::mitm::nifm, INifmMitmInterface, AMS_NIFM_MITM_INTERFACE_INFO, 0x4EF5CC91)

namespace ams::mitm::nifm {

    class NifmMitmService : public sf::MitmServiceImplBase {
        public:
            using MitmServiceImplBase::MitmServiceImplBase;
        public:
            static bool ShouldMitm(const sm::MitmProcessInfo &client_info) {
                AMS_UNUSED(client_info);
                return true;
            }
        public:
            Result AtmosphereReloadHostsForNifm();
    };
    static_assert(IsINifmMitmInterface<NifmMitmService>);

}
