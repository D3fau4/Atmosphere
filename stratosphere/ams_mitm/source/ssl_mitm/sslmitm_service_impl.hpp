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

/* IPC interface for the `ssl` service. Command IDs in the Atmosphère
 * extension range (>= 65000) are reserved for tooling outside the official
 * Nintendo IPC layout. The only one defined here is a trust-store reload,
 * which lets userspace re-scan /atmosphere/ssl_trust/ without a reboot.
 *
 * All ordinary ssl commands (CreateContext, GetCertificates, ...) are
 * auto-forwarded by the MitM framework because they are not listed here.
 * Once the IContext / IConnection proxy chain is added (future work), the
 * relevant commands will be added to this list with explicit signatures
 * including sf::Out<sf::SharedPointer<...>> for the object returns.
 */
#define AMS_SSL_MITM_INTERFACE_INFO(C, H)                                                                                                                        \
    AMS_SF_METHOD_INFO(C, H, 65000, Result, AtmosphereReloadTrustStore,     (),                                                              ())

AMS_SF_DEFINE_MITM_INTERFACE(ams::mitm::ssl, ISslMitmInterface, AMS_SSL_MITM_INTERFACE_INFO, 0x1A55EE73)

namespace ams::mitm::ssl {

    class SslMitmService : public sf::MitmServiceImplBase {
        public:
            using MitmServiceImplBase::MitmServiceImplBase;
        public:
            static bool ShouldMitm(const sm::MitmProcessInfo &client_info) {
                AMS_UNUSED(client_info);
                return true;
            }
        public:
            Result AtmosphereReloadTrustStore();
    };
    static_assert(IsISslMitmInterface<SslMitmService>);

}
