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
#include "accmitm_debug.hpp"

namespace ams::mitm::account {

    namespace {

        constinit bool g_debug_log_enabled = false;
        constinit os::SdkMutex g_log_lock;

    }

    void InitializeDebug(bool enabled) {
        g_debug_log_enabled = enabled;
        if (enabled) {
            mitm::fs::CreateAtmosphereSdDirectory("/logs");
            mitm::fs::DeleteAtmosphereSdFile("/logs/account_mitm.log");
            R_ABORT_UNLESS(mitm::fs::CreateAtmosphereSdFile("/logs/account_mitm.log", 0, ams::fs::CreateOption_None));
        }
    }

    void LogDebug(const char *fmt, ...) {
        if (!g_debug_log_enabled) {
            return;
        }

        char buf[0x200];
        int len = 0;
        {
            std::va_list vl;
            va_start(vl, fmt);
            len = util::VSNPrintf(buf, sizeof(buf), fmt, vl);
            va_end(vl);
        }

        std::scoped_lock lk(g_log_lock);

        ::FsFile f;
        if (R_FAILED(mitm::fs::OpenAtmosphereSdFile(std::addressof(f), "/logs/account_mitm.log", ams::fs::OpenMode_ReadWrite | ams::fs::OpenMode_AllowAppend))) {
            return;
        }
        ON_SCOPE_EXIT { ::fsFileClose(std::addressof(f)); };

        s64 ofs = 0;
        if (R_FAILED(::fsFileGetSize(std::addressof(f), std::addressof(ofs)))) {
            return;
        }

        ::fsFileWrite(std::addressof(f), ofs, buf, len, ::FsWriteOption_Flush);
    }

}
