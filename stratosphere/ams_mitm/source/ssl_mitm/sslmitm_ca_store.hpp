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

namespace ams::mitm::ssl {

    /* On-disk layout:
     *   /atmosphere/ssl_trust/<hostname>.pem  (or .der)
     *
     * The hostname encoded in the filename is matched against SetServerName
     * calls. When no match exists, the SSL pipeline runs unchanged against
     * Nintendo's built-in root CA store (the "fallback to originals" behaviour
     * the user requested). The file contents are loaded raw into memory at
     * startup so they can be injected via ImportServerPki when the matching
     * host appears.
     */

    void InitializeCaStore();
    void ReloadCaStore();

    bool HasCustomCaForHost(const char *hostname);
    bool GetCustomCaForHost(const u8 **out_data, size_t *out_size, const char *hostname);

    size_t GetCustomCaCount();

}
