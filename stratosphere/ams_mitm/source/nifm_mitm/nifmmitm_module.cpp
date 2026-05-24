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
#include "nifmmitm_module.hpp"
#include "nifmmitm_service_impl.hpp"
#include "nifmmitm_debug.hpp"

namespace ams::mitm::nifm {

    namespace {

        enum PortIndex {
            PortIndex_MitmUser,
            PortIndex_MitmAdmin,
            PortIndex_MitmSystem,
            PortIndex_Count,
        };

        constexpr sm::ServiceName NifmMitmUserServiceName   = sm::ServiceName::Encode("nifm:u");
        constexpr sm::ServiceName NifmMitmAdminServiceName  = sm::ServiceName::Encode("nifm:a");
        constexpr sm::ServiceName NifmMitmSystemServiceName = sm::ServiceName::Encode("nifm:s");

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
                case PortIndex_MitmUser:
                case PortIndex_MitmAdmin:
                case PortIndex_MitmSystem:
                    R_RETURN(this->AcceptMitmImpl(server, sf::CreateSharedObjectEmplaced<INifmMitmInterface, NifmMitmService>(decltype(fsrv)(fsrv), client_info), fsrv));
                AMS_UNREACHABLE_DEFAULT_CASE();
            }
        }

        bool ShouldMitmNifm() {
            u8 en = 0;
            if (settings::fwdbg::GetSettingsItemValue(std::addressof(en), sizeof(en), "atmosphere", "enable_nifm_mitm") == sizeof(en)) {
                return (en != 0);
            }
            return false;
        }

        bool ShouldEnableDebugLog() {
            u8 en = 0;
            if (settings::fwdbg::GetSettingsItemValue(std::addressof(en), sizeof(en), "atmosphere", "enable_nifm_mitm_debug_log") == sizeof(en)) {
                return (en != 0);
            }
            return false;
        }

    }

    void MitmModule::ThreadFunction(void *) {
        mitm::WaitInitialized();

        if (!ShouldMitmNifm()) {
            return;
        }

        InitializeDebug(ShouldEnableDebugLog());

        R_ABORT_UNLESS((g_server_manager.RegisterMitmServer<NifmMitmService>(PortIndex_MitmUser,   NifmMitmUserServiceName)));
        R_ABORT_UNLESS((g_server_manager.RegisterMitmServer<NifmMitmService>(PortIndex_MitmAdmin,  NifmMitmAdminServiceName)));
        R_ABORT_UNLESS((g_server_manager.RegisterMitmServer<NifmMitmService>(PortIndex_MitmSystem, NifmMitmSystemServiceName)));

        g_server_manager.LoopProcess();
    }

}
