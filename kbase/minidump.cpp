/*
 @ 0xCCCCCCCC
*/

#include "kbase/minidump.h"

#include <windows.h>
#pragma warning(push)
#pragma warning(disable: 4091)
#include <DbgHelp.h>
#pragma warning(pop)

#include "kbase/file_path.h"
#include "kbase/scoped_handle.h"

#if !defined(NDEBUG)
#include "kbase/error_exception_util.h"
#include "kbase/logging.h"
#endif

namespace {

using kbase::FilePath;
using kbase::ScopedSysHandle;

bool WriteMiniDumpFile(const FilePath& dump_path, EXCEPTION_POINTERS* ex_ptrs)
{
    ScopedSysHandle dump_file(CreateFileW(dump_path.value().c_str(),
                                          GENERIC_WRITE,
                                          0,
                                          nullptr,
                                          CREATE_ALWAYS,
                                          FILE_ATTRIBUTE_NORMAL,
                                          nullptr));
    if (!dump_file) {
#if !defined(NDEBUG)
        kbase::LastError error;
        DLOG(ERROR) << "Failed to create dump file.\n\t" << error;
#endif
        return false;
    }

    MINIDUMP_EXCEPTION_INFORMATION exception_information { 0 };
    exception_information.ThreadId = GetCurrentThreadId();
    exception_information.ClientPointers = FALSE;
    exception_information.ExceptionPointers = ex_ptrs;

    BOOL rv = MiniDumpWriteDump(GetCurrentProcess(),
                                GetCurrentProcessId(),
                                dump_file.Get(),
                                MiniDumpNormal,
                                &exception_information,
                                nullptr,
                                nullptr);
#if !defined(NDEBUG)
    // According to the MSDN, error code obtained actually is a HRESULT value.
    if (!rv) {
        kbase::LastError error;
        DLOG(ERROR) << "Writing minidump error. (" << error.error_code() << ")";
    }
#endif

    return !!rv;
}

void HandleException(const FilePath& dump_path, EXCEPTION_POINTERS* ex_ptrs, bool* succeeded)
{
    *succeeded = WriteMiniDumpFile(dump_path, ex_ptrs);
}

}   // namespace

namespace kbase {

bool CreateMiniDump(const FilePath& dump_path)
{
    bool succeeded = false;
    __try {
        RaiseException(EXCEPTION_BREAKPOINT, 0, 0, nullptr);
    } __except (HandleException(dump_path, GetExceptionInformation(), &succeeded),
                EXCEPTION_EXECUTE_HANDLER) {}

    return succeeded;
}

}   // namespace kbase