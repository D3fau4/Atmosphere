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
#include "../amsmitm_fs_utils.hpp"
#include "sslmitm_ca_store.hpp"
#include "sslmitm_debug.hpp"

namespace ams::mitm::ssl {

    namespace {

        constexpr const char SslTrustDir[]   = "/ssl_trust";
        constexpr size_t MaxHostnameLen      = 0x100;
        constexpr size_t MaxCaFileSize       = 0x4000;

        struct CaEntry {
            std::string hostname;
            std::vector<u8> data;
        };

        constinit os::SdkMutex g_store_lock;
        std::vector<CaEntry> g_store;

        bool EndsWith(const char *str, const char *suffix) {
            const size_t str_len    = std::strlen(str);
            const size_t suffix_len = std::strlen(suffix);
            if (suffix_len > str_len) {
                return false;
            }
            for (size_t i = 0; i < suffix_len; ++i) {
                const char a = str[str_len - suffix_len + i];
                const char b = suffix[i];
                const char la = (a >= 'A' && a <= 'Z') ? (a + ('a' - 'A')) : a;
                const char lb = (b >= 'A' && b <= 'Z') ? (b + ('a' - 'A')) : b;
                if (la != lb) {
                    return false;
                }
            }
            return true;
        }

        void StripExtension(char *name) {
            for (int i = static_cast<int>(std::strlen(name)) - 1; i >= 0; --i) {
                if (name[i] == '.') {
                    name[i] = '\0';
                    return;
                }
                if (name[i] == '/') {
                    return;
                }
            }
        }

        bool LoadOneFile(const char *file_name) {
            char rel_path[0x140];
            util::SNPrintf(rel_path, sizeof(rel_path), "%s/%s", SslTrustDir, file_name);

            ::FsFile f;
            if (R_FAILED(mitm::fs::OpenAtmosphereSdFile(std::addressof(f), rel_path, ams::fs::OpenMode_Read))) {
                LogDebug("[ssl_mitm] failed to open %s\n", rel_path);
                return false;
            }
            ON_SCOPE_EXIT { ::fsFileClose(std::addressof(f)); };

            s64 size = 0;
            if (R_FAILED(::fsFileGetSize(std::addressof(f), std::addressof(size)))) {
                return false;
            }
            if (size <= 0 || static_cast<size_t>(size) > MaxCaFileSize) {
                LogDebug("[ssl_mitm] skipping %s (bad size %ld)\n", file_name, static_cast<long>(size));
                return false;
            }

            CaEntry entry;
            entry.data.resize(static_cast<size_t>(size));

            u64 br = 0;
            if (R_FAILED(::fsFileRead(std::addressof(f), 0, entry.data.data(), entry.data.size(), ::FsReadOption_None, std::addressof(br)))) {
                return false;
            }
            if (br != static_cast<u64>(size)) {
                return false;
            }

            char host[MaxHostnameLen];
            util::Strlcpy(host, file_name, sizeof(host));
            StripExtension(host);
            entry.hostname.assign(host);

            LogDebug("[ssl_mitm] loaded CA for `%s` (%zu bytes)\n", entry.hostname.c_str(), entry.data.size());
            g_store.emplace_back(std::move(entry));
            return true;
        }

    }

    void InitializeCaStore() {
        ReloadCaStore();
    }

    void ReloadCaStore() {
        std::scoped_lock lk(g_store_lock);
        g_store.clear();

        /* Ensure the trust directory exists; create empty if missing so users
         * can drop certs without first running anything. */
        mitm::fs::CreateAtmosphereSdDirectory(SslTrustDir);

        ::FsDir dir;
        if (R_FAILED(mitm::fs::OpenAtmosphereSdDirectory(std::addressof(dir), SslTrustDir, ams::fs::OpenDirectoryMode_File))) {
            LogDebug("[ssl_mitm] no trust dir, fallback to original CAs for all hosts\n");
            return;
        }
        ON_SCOPE_EXIT { ::fsDirClose(std::addressof(dir)); };

        ::FsDirectoryEntry entry;
        s64 read_count = 0;
        while (R_SUCCEEDED(::fsDirRead(std::addressof(dir), std::addressof(read_count), 1, std::addressof(entry))) && read_count > 0) {
            const char *name = entry.name;
            if (!EndsWith(name, ".pem") && !EndsWith(name, ".der") && !EndsWith(name, ".crt") && !EndsWith(name, ".cer")) {
                continue;
            }
            LoadOneFile(name);
        }

        LogDebug("[ssl_mitm] trust store loaded with %zu entries\n", g_store.size());
    }

    bool HasCustomCaForHost(const char *hostname) {
        if (hostname == nullptr) {
            return false;
        }
        std::scoped_lock lk(g_store_lock);
        for (const auto &e : g_store) {
            if (std::strcmp(e.hostname.c_str(), hostname) == 0) {
                return true;
            }
        }
        return false;
    }

    bool GetCustomCaForHost(const u8 **out_data, size_t *out_size, const char *hostname) {
        if (hostname == nullptr || out_data == nullptr || out_size == nullptr) {
            return false;
        }
        std::scoped_lock lk(g_store_lock);
        for (const auto &e : g_store) {
            if (std::strcmp(e.hostname.c_str(), hostname) == 0) {
                *out_data = e.data.data();
                *out_size = e.data.size();
                return true;
            }
        }
        return false;
    }

    size_t GetCustomCaCount() {
        std::scoped_lock lk(g_store_lock);
        return g_store.size();
    }

}
