/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* Parallel Patterns Library - Linux version
*
* For the latest on this and related APIs, please see: https://github.com/Microsoft/cpprestsdk
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

#include "stdafx.h"
#include "pplx/pplx.h"
#include "threadpool.h"
#include "sys/syscall.h"

#ifdef _WIN32
#error "ERROR: This file should only be included in non-windows Build"
#endif

namespace pplx
{

namespace details {

    namespace platform
    {
        _PPLXIMP long GetCurrentThreadId()
        {
            return reinterpret_cast<long>(reinterpret_cast<void*>(pthread_self()));
        }

        _PPLXIMP void YieldExecution()
        {
            boost::this_thread::yield();
        }
    }

    _PPLXIMP void linux_scheduler::schedule(TaskProc_t proc, void* param)
    {
        crossplat::threadpool::shared_instance().schedule(boost::bind(proc, param));
    }

} // namespace details

} // namespace pplx
