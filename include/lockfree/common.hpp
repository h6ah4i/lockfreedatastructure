//
//    Copyright (C) 2014 Haruki Hasegawa
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//

#ifndef LOCKFREE_COMMON_HPP_
#define LOCKFREE_COMMON_HPP_

#ifdef LFDS_HAVE_CXXPORTHELPER

// use cxxporthelper
#include <cxxporthelper/atomic>
#include <cxxporthelper/cstdint>
#include <cxxporthelper/compiler.hpp>

#define LFDS_NOEXCEPT CXXPH_NOEXCEPT
#if CXXPH_COMPILER_SUPPORTS_CONSTEXPR
#define LFDS_CONSTEXPR constexpr
#else
#define LFDS_CONSTEXPR const
#endif
#define LFDS_LIKELY(cond) CXXPH_LIKELY(cond)
#define LFDS_UNLIKELY(cond) CXXPH_UNLIKELY(cond)

#else

// without cxxporthelper
#include <atomic>
#include <cstdint>

#define LFDS_NOEXCEPT noexcept
#define LFDS_CONSTEXPR constexpr
#define LFDS_LIKELY(cond) (cond)
#define LFDS_UNLIKELY(cond) (cond)

#endif

#endif // LOCKFREE_COMMON_HPP_
