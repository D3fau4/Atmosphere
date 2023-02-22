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
#include <stratosphere/ncm/ncm_i_content_storage.hpp>
#include <stratosphere/ncm/ncm_integrated_list.hpp>

namespace ams::ncm {

    class IntegratedContentStorageImpl {
        NON_COPYABLE(IntegratedContentStorageImpl);
        NON_MOVEABLE(IntegratedContentStorageImpl);
        private:
            using ListType = ncm::IntegratedList<ncm::IContentStorage, 2>;
            using DataType = ListType::ListData;
        private:
            os::SdkMutex m_mutex;
            ListType m_list;
            bool m_disabled;
        public:
            IntegratedContentStorageImpl() : m_mutex(), m_list(), m_disabled(false) { /* ... */ }

            void Add(sf::SharedPointer<ncm::IContentStorage> p, u8 id) {
                DataType data = {std::move(p), id};
                m_list.Add(data);
            }
        private:
            /* Helpers. */
            Result EnsureEnabled() const {
                R_UNLESS(!m_disabled, ncm::ResultInvalidContentStorage());
                R_SUCCEED();
            }
        public:
            /* Actual commands. */
            Result GeneratePlaceHolderId(sf::Out<PlaceHolderId> out);
            Result CreatePlaceHolder(PlaceHolderId placeholder_id, ContentId content_id, s64 size);
            Result DeletePlaceHolder(PlaceHolderId placeholder_id);
            Result HasPlaceHolder(sf::Out<bool> out, PlaceHolderId placeholder_id);
            Result WritePlaceHolder(PlaceHolderId placeholder_id, s64 offset, const sf::InBuffer &data);
            Result Register(PlaceHolderId placeholder_id, ContentId content_id);
            Result Delete(ContentId content_id);
            Result Has(sf::Out<bool> out, ContentId content_id);
            Result GetPath(sf::Out<Path> out, ContentId content_id);
            Result GetPlaceHolderPath(sf::Out<Path> out, PlaceHolderId placeholder_id);
            Result CleanupAllPlaceHolder();
            Result ListPlaceHolder(sf::Out<s32> out_count, const sf::OutArray<PlaceHolderId> &out_buf);
            Result GetContentCount(sf::Out<s32> out_count);
            Result ListContentId(sf::Out<s32> out_count, const sf::OutArray<ContentId> &out_buf, s32 start_offset);
            Result GetSizeFromContentId(sf::Out<s64> out_size, ContentId content_id);
            Result DisableForcibly();
            Result RevertToPlaceHolder(PlaceHolderId placeholder_id, ContentId old_content_id, ContentId new_content_id);
            Result SetPlaceHolderSize(PlaceHolderId placeholder_id, s64 size);
            Result ReadContentIdFile(const sf::OutBuffer &buf, ContentId content_id, s64 offset);
            Result GetRightsIdFromPlaceHolderIdDeprecated(sf::Out<ams::fs::RightsId> out_rights_id, PlaceHolderId placeholder_id);
            Result GetRightsIdFromPlaceHolderIdDeprecated2(sf::Out<ncm::RightsId> out_rights_id, PlaceHolderId placeholder_id);
            Result GetRightsIdFromPlaceHolderId(sf::Out<ncm::RightsId> out_rights_id, PlaceHolderId placeholder_id, fs::ContentAttributes attr);
            Result GetRightsIdFromContentIdDeprecated(sf::Out<ams::fs::RightsId> out_rights_id, ContentId content_id);
            Result GetRightsIdFromContentIdDeprecated2(sf::Out<ncm::RightsId> out_rights_id, ContentId content_id);
            Result GetRightsIdFromContentId(sf::Out<ncm::RightsId> out_rights_id, ContentId content_id, fs::ContentAttributes attr);
            Result WriteContentForDebug(ContentId content_id, s64 offset, const sf::InBuffer &data);
            Result GetFreeSpaceSize(sf::Out<s64> out_size);
            Result GetTotalSpaceSize(sf::Out<s64> out_size);
            Result FlushPlaceHolder();
            Result GetSizeFromPlaceHolderId(sf::Out<s64> out, PlaceHolderId placeholder_id);
            Result RepairInvalidFileAttribute();
            Result GetRightsIdFromPlaceHolderIdWithCacheDeprecated(sf::Out<ncm::RightsId> out_rights_id, PlaceHolderId placeholder_id, ContentId cache_content_id);
            Result GetRightsIdFromPlaceHolderIdWithCache(sf::Out<ncm::RightsId> out_rights_id, PlaceHolderId placeholder_id, ContentId cache_content_id, fs::ContentAttributes attr);
            Result RegisterPath(const ContentId &content_id, const Path &path);
            Result ClearRegisteredPath();
    };
    static_assert(ncm::IsIContentStorage<IntegratedContentStorageImpl>);

}
