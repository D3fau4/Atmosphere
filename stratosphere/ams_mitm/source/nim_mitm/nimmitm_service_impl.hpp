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

/* IPC interface for the nim group of services. nim itself exposes several
 * named ports (nim, nim:eca, nim:shp, nim:s) that hand out objects like
 * IShopServiceAccessor; the heavy lifting (eShop calls, ticket downloads,
 * system update fetches) happens inside those objects and travels out as
 * HTTPS via ssl/bsd, where ssl.mitm + dns.mitm already get a look at it.
 *
 * For now this MitM only exposes an Atmosphère extension command. Real
 * interception of the IShopServiceAccessor chain (RequestSystemUpdateInfo,
 * Get*UpdateUrl, etc.) requires the same sf::Out<sf::SharedPointer<...>>
 * proxy chain that ssl.mitm needs and is deliberately staged behind it.
 */
#define AMS_NIM_MITM_INTERFACE_INFO(C, H)                                                                                                              \
    AMS_SF_METHOD_INFO(C, H, 65000, Result, AtmosphereReloadNimRedirects,    (),                                                                ())

AMS_SF_DEFINE_MITM_INTERFACE(ams::mitm::nim, INimMitmInterface, AMS_NIM_MITM_INTERFACE_INFO, 0x6D11AAFE)

namespace ams::mitm::nim {

    class NimMitmService : public sf::MitmServiceImplBase {
        public:
            using MitmServiceImplBase::MitmServiceImplBase;
        public:
            static bool ShouldMitm(const sm::MitmProcessInfo &client_info) {
                AMS_UNUSED(client_info);
                return true;
            }
        public:
            Result AtmosphereReloadNimRedirects();
    };
    static_assert(IsINimMitmInterface<NimMitmService>);

}
