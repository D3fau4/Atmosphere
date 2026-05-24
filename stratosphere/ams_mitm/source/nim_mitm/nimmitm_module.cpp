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
#include "nimmitm_module.hpp"
#include "nimmitm_service_impl.hpp"
#include "nimmitm_debug.hpp"

namespace ams::mitm::nim {

    namespace {

        enum PortIndex {
            PortIndex_MitmDefault,
            PortIndex_MitmEca,
            PortIndex_MitmShp,
            PortIndex_MitmSystem,
            PortIndex_Count,
        };

        constexpr sm::ServiceName NimMitmDefaultServiceName = sm::ServiceName::Encode("nim");
        constexpr sm::ServiceName NimMitmEcaServiceName     = sm::ServiceName::Encode("nim:eca");
        constexpr sm::ServiceName NimMitmShpServiceName     = sm::ServiceName::Encode("nim:shp");
        constexpr sm::ServiceName NimMitmSystemServiceName  = sm::ServiceName::Encode("nim:s");

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
                case PortIndex_MitmDefault:
                case PortIndex_MitmEca:
                case PortIndex_MitmShp:
                case PortIndex_MitmSystem:
                    R_RETURN(this->AcceptMitmImpl(server, sf::CreateSharedObjectEmplaced<INimMitmInterface, NimMitmService>(decltype(fsrv)(fsrv), client_info), fsrv));
                AMS_UNREACHABLE_DEFAULT_CASE();
            }
        }

        bool ShouldMitmNim() {
            u8 en = 0;
            if (settings::fwdbg::GetSettingsItemValue(std::addressof(en), sizeof(en), "atmosphere", "enable_nim_mitm") == sizeof(en)) {
                return (en != 0);
            }
            return false;
        }

        bool ShouldEnableDebugLog() {
            u8 en = 0;
            if (settings::fwdbg::GetSettingsItemValue(std::addressof(en), sizeof(en), "atmosphere", "enable_nim_mitm_debug_log") == sizeof(en)) {
                return (en != 0);
            }
            return false;
        }

    }

    void MitmModule::ThreadFunction(void *) {
        mitm::WaitInitialized();

        if (!ShouldMitmNim()) {
            return;
        }

        InitializeDebug(ShouldEnableDebugLog());

        R_ABORT_UNLESS((g_server_manager.RegisterMitmServer<NimMitmService>(PortIndex_MitmDefault, NimMitmDefaultServiceName)));
        R_ABORT_UNLESS((g_server_manager.RegisterMitmServer<NimMitmService>(PortIndex_MitmEca,     NimMitmEcaServiceName)));
        R_ABORT_UNLESS((g_server_manager.RegisterMitmServer<NimMitmService>(PortIndex_MitmShp,     NimMitmShpServiceName)));
        R_ABORT_UNLESS((g_server_manager.RegisterMitmServer<NimMitmService>(PortIndex_MitmSystem,  NimMitmSystemServiceName)));

        g_server_manager.LoopProcess();
    }

}
