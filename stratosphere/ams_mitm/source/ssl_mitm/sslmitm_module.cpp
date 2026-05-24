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
#include "sslmitm_module.hpp"
#include "sslmitm_service_impl.hpp"
#include "sslmitm_ca_store.hpp"
#include "sslmitm_debug.hpp"

namespace ams::mitm::ssl {

    namespace {

        enum PortIndex {
            PortIndex_Mitm,
            PortIndex_Count,
        };

        constexpr sm::ServiceName SslMitmServiceName = sm::ServiceName::Encode("ssl");

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
                case PortIndex_Mitm:
                    R_RETURN(this->AcceptMitmImpl(server, sf::CreateSharedObjectEmplaced<ISslMitmInterface, SslMitmService>(decltype(fsrv)(fsrv), client_info), fsrv));
                AMS_UNREACHABLE_DEFAULT_CASE();
            }
        }

        bool ShouldMitmSsl() {
            u8 en = 0;
            if (settings::fwdbg::GetSettingsItemValue(std::addressof(en), sizeof(en), "atmosphere", "enable_ssl_mitm") == sizeof(en)) {
                return (en != 0);
            }
            return false;
        }

        bool ShouldEnableDebugLog() {
            u8 en = 0;
            if (settings::fwdbg::GetSettingsItemValue(std::addressof(en), sizeof(en), "atmosphere", "enable_ssl_mitm_debug_log") == sizeof(en)) {
                return (en != 0);
            }
            return false;
        }

    }

    void MitmModule::ThreadFunction(void *) {
        mitm::WaitInitialized();

        if (!ShouldMitmSsl()) {
            return;
        }

        InitializeDebug(ShouldEnableDebugLog());
        InitializeCaStore();

        R_ABORT_UNLESS((g_server_manager.RegisterMitmServer<SslMitmService>(PortIndex_Mitm, SslMitmServiceName)));

        g_server_manager.LoopProcess();
    }

}
