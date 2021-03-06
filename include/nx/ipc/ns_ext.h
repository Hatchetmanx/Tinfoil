#pragma once

#include <switch.h>
#include <switch/services/ncm.h>

typedef struct {
    u64 titleID;
    u64 unk;
    u64 size;
} ApplicationRecord;

typedef struct {
    NcmMetaRecord metaRecord;
    u64 storageId;
} ContentStorageRecord;

Result nsextInitialize(void);
void nsextExit(void);

Result nsCalculateApplicationOccupiedSize(u64 titleID, void *out_buf);
Result nsPushApplicationRecord(u64 title_id, u8 last_modified_event, ContentStorageRecord *content_records_buf, size_t buf_size);
Result nsListApplicationRecord(u64 offset, void *out_buf, size_t out_buf_size, u32 *entries_read_out);
Result nsListApplicationRecordContentMeta(u64 offset, u64 titleID, void *out_buf, size_t out_buf_size, u32 *entries_read_out);
Result nsDeleteApplicationRecord(u64 titleID);
Result nsTouchApplication(u64 titleID);
Result nsLaunchApplication(u64 titleID);
Result nsPushLaunchVersion(u64 titleID, u32 version);
Result nsCheckApplicationLaunchVersion(u64 titleID);
Result nsDisableApplicationAutoUpdate(u64 titleID);
Result nsGetContentMetaStorage(const NcmMetaRecord *record, u8 *out);
Result nsBeginInstallApplication(u64 tid, u32 unk, u8 storageId);
Result nsInvalidateAllApplicationControlCache(void);
Result nsInvalidateApplicationControlCache(u64 tid);
Result nsCheckApplicationLaunchRights(u64 tid);
Result nsGetApplicationContentPath(u64 titleId, u8 type, char *outBuf, size_t bufSize);