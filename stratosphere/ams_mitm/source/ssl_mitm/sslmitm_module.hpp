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
#include "../amsmitm_module.hpp"

namespace ams::mitm::ssl {

    /* Stack 0x4000; priority one above the official ssl process's main
     * thread priority (28 per Nintendo's ssl.desc.partial.xml) so we wake
     * before the real service when both have requests. */
    DEFINE_MITM_MODULE_CLASS(0x4000, 27);

}
