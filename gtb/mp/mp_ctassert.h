#ifndef __GTB_META_PROGRAMMING_CTASSERT_H
#define __GTB_META_PROGRAMMING_CTASSERT_H

#define BUILD_ASSERT( B ) typedef int \
BUILD_ASSERT[ sizeof(BUILD_ASSERT_CHECK< static_cast<bool>( B ) >) ]

template <bool x> struct BUILD_ASSERT_CHECK;

template <> struct BUILD_ASSERT_CHECK<true> {};

/*template <bool Condition>
    class CTAssert {};

template <>
class CTAssert<false> {
    CTAssert() {
	DIE_STUPID_COMPILER;
    }
    };*/

#endif
