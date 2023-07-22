#ifndef LIBGO_CONTEXT_FCONTEXT_H
#define LIBGO_CONTEXT_FCONTEXT_H
#include "../common/config.h"
#include <memory>
#include <string.h>

namespace co {

struct StackTraits
{
    static stack_malloc_fn_t& MallocFunc()
    {
        static stack_malloc_fn_t fn = &::std::malloc;
        return fn;
    }

    static stack_free_fn_t& FreeFunc()
    {
        static stack_free_fn_t fn = &::std::free;
        return fn;
    }

    static int & GetProtectStackPageSize()
    {
        static int size = 0;
        return size;
    }

    static bool ProtectStack(void* stack, std::size_t size, int pageSize)
    {
        return false;
    }

    static void UnprotectStack(void* stack, int pageSize)
    {
        return ;
    }
};

} // namespace co

//#include "../../third_party/boost.context/boost/context/fcontext.hpp"
//using boost::context::fcontext_t;
//using boost::context::jump_fcontext;
//using boost::context::make_fcontext;

extern "C"
{

typedef void* fcontext_t;
typedef void (FCONTEXT_CALL *fn_t)(intptr_t);

intptr_t libgo_jump_fcontext(fcontext_t * ofc, fcontext_t nfc,
        intptr_t vp, bool preserve_fpu = false);

fcontext_t libgo_make_fcontext(void* stack, std::size_t size, fn_t fn);

} // extern "C"
#endif // LIBGO_CONTEXT_FCONTEXT_H
