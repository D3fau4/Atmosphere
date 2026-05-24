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
#include "../amsmitm_initialization.hpp"
#include "accmitm_module.hpp"
#include "accmitm_service_impl.hpp"
#include "accmitm_debug.hpp"

namespace ams::mitm::account {

    namespace {

        enum PortIndex {
            PortIndex_MitmU0,
            PortIndex_MitmU1,
            PortIndex_MitmSu,
            PortIndex_MitmAa,
            PortIndex_Count,
        };

        constexpr sm::ServiceName AccountMitmU0ServiceName = sm::ServiceName::Encode("acc:u0");
        constexpr sm::ServiceName AccountMitmU1ServiceName = sm::ServiceName::Encode("acc:u1");
        constexpr sm::ServiceName AccountMitmSuServiceName = sm::ServiceName::Encode("acc:su");
        constexpr sm::ServiceName AccountMitmAaServiceName = sm::ServiceName::Encode("acc:aa");

        constexpr size_t MaxSessions = 30;

        struct ServerOptions {
            static constexpr size_t PointerBufferSize   = sf::hipc::DefaultServerManagerOptions::PointerBufferSize;
            static constexpr size_t MaxDomains          = sf::hipc::DefaultServerManagerOptions::MaxDomains;
            static constexpr size_t MaxDomainObjects    = sf::hipc::DefaultServerManagerOptions::MaxDomainObjects;
            static constexpr bool CanDeferInvokeRequest = sf::hipc::DefaultServerManagerOptions::CanDeferInvokeRequest;
            static constexpr bool CanManageMitmServers  = true;
        };

        class ServerManager final : public sf::hipc::ServerManager<PortIndex_Count, ServerOptions, MaxSessions> {
            private:
                virtual Result OnNeedsToAccept(int port_index, Server *server) override;
        };

        ServerManager g_server_manager;

        Result ServerManager::OnNeedsToAccept(int port_index, Server *server) {
            std::shared_ptr<::Service> fsrv;
            sm::MitmProcessInfo client_info;
            server->AcknowledgeMitmSession(std::addressof(fsrv), std::addressof(client_info));

            switch (port_index) {
                case PortIndex_MitmU0:
                case PortIndex_MitmU1:
                case PortIndex_MitmSu:
                case PortIndex_MitmAa:
                    R_RETURN(this->AcceptMitmImpl(server, sf::CreateSharedObjectEmplaced<IAccountMitmInterface, AccountMitmService>(decltype(fsrv)(fsrv), client_info), fsrv));
                AMS_UNREACHABLE_DEFAULT_CASE();
            }
        }

        bool ShouldMitmAccount() {
            u8 en = 0;
            if (settings::fwdbg::GetSettingsItemValue(std::addressof(en), sizeof(en), "atmosphere", "enable_account_mitm") == sizeof(en)) {
                return (en != 0);
            }
            return false;
        }

        bool ShouldEnableDebugLog() {
            u8 en = 0;
            if (settings::fwdbg::GetSettingsItemValue(std::addressof(en), sizeof(en), "atmosphere", "enable_account_mitm_debug_log") == sizeof(en)) {
                return (en != 0);
            }
            return false;
        }

    }

    void MitmModule::ThreadFunction(void *) {
        mitm::WaitInitialized();

        if (!ShouldMitmAccount()) {
            return;
        }

        InitializeDebug(ShouldEnableDebugLog());

        R_ABORT_UNLESS((g_server_manager.RegisterMitmServer<AccountMitmService>(PortIndex_MitmU0, AccountMitmU0ServiceName)));
        R_ABORT_UNLESS((g_server_manager.RegisterMitmServer<AccountMitmService>(PortIndex_MitmU1, AccountMitmU1ServiceName)));
        R_ABORT_UNLESS((g_server_manager.RegisterMitmServer<AccountMitmService>(PortIndex_MitmSu, AccountMitmSuServiceName)));
        R_ABORT_UNLESS((g_server_manager.RegisterMitmServer<AccountMitmService>(PortIndex_MitmAa, AccountMitmAaServiceName)));

        g_server_manager.LoopProcess();
    }

}
