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

/* IPC interface for the account group of services (acc:u0, acc:u1, acc:su,
 * acc:aa). Authentication and Nintendo Account token flows travel over
 * HTTPS, where ssl.mitm + dns.mitm already see them and can be pointed at
 * an alternate auth backend.
 *
 * What this MitM is for is overriding *local* account state (account ID,
 * nickname, opt-in flags) so an alt-network's identity can be presented to
 * games and applets without touching the on-device account database.
 * Those overrides need the sf::Out<sf::SharedPointer<...>> proxy chain
 * for the per-user IProfile / IBaasAccessTokenAccessor objects; the chain
 * is parked behind the equivalent ssl.mitm work and only the extension
 * command lives here today.
 */
#define AMS_ACCOUNT_MITM_INTERFACE_INFO(C, H)                                                                                                                            \
    AMS_SF_METHOD_INFO(C, H, 65000, Result, AtmosphereReloadAccountOverrides,    (),                                                                              ())

AMS_SF_DEFINE_MITM_INTERFACE(ams::mitm::account, IAccountMitmInterface, AMS_ACCOUNT_MITM_INTERFACE_INFO, 0x3CCB7401)

namespace ams::mitm::account {

    class AccountMitmService : public sf::MitmServiceImplBase {
        public:
            using MitmServiceImplBase::MitmServiceImplBase;
        public:
            static bool ShouldMitm(const sm::MitmProcessInfo &client_info) {
                AMS_UNUSED(client_info);
                return true;
            }
        public:
            Result AtmosphereReloadAccountOverrides();
    };
    static_assert(IsIAccountMitmInterface<AccountMitmService>);

}
