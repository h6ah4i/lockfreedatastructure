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

#ifndef LOCKFREE_CIRCULATION_BUFFER_HPP_
#define LOCKFREE_CIRCULATION_BUFFER_HPP_

#include <lockfree/common.hpp>

namespace lockfree {

/**
 * Lock free circulation buffer (single-producer, single-consumer)
 *
 * @tparam T data type
 * @tparam N buffer size (power of two)
 */
template <typename T, size_t N>
class lockfree_circulation_buffer {
public:
    /**
     * Data type.
     */
    typedef T data_type;

    /**
     * Buffer index type.
     */
    typedef uint16_t index_t;

    /**
     * Data type.
     */
    static LFDS_CONSTEXPR index_t INVALID_INDEX = static_cast<index_t>(-1);

    /**
     * Constructor.
     */
    lockfree_circulation_buffer();

    /**
     * Destructor.
     */
    ~lockfree_circulation_buffer();

    /**
     * Check is empty.
     * @returns whether the buffe is empty
     */
    bool empty() const LFDS_NOEXCEPT;

    /**
     * Check is full.
     * @returns whether the buffe is full
     */
    bool full() const LFDS_NOEXCEPT;

    /**
     * Get current available item count.
     * @returns number of available items
     */
    size_t size() const LFDS_NOEXCEPT;

    /**
     * Get buffer capacity.
     * @returns (N - 1)
     */
    size_t capacity() const LFDS_NOEXCEPT;

    /**
     * Write-lock operation.
     * @param index [out] locked buffer index
     * @param min_free [in] minimum buffer room to keep
     * @returns whether the operation is succeeded
     */
    bool lock_write(index_t &index, size_t min_free = 0) LFDS_NOEXCEPT;

    /**
     * Write-unlock operation.
     * @param index [in] locked buffer index
     * @param commit [in] specify commit the operation or not
     * @returns whether the operation is succeeded
     */
    bool unlock_write(index_t index, bool commit = true) LFDS_NOEXCEPT;

    /**
     * Read-lock operation.
     * @param index [out] locked buffer index
     * @param min_free [in] minimum item count to keep
     * @returns whether the operation is succeeded
     */
    bool lock_read(index_t &index, size_t min_remains = 0) LFDS_NOEXCEPT;

    /**
     * Read-unlock operation.
     * @param index [in] locked buffer index
     * @param commit [in] specify commit the operation or not
     * @returns whether the operation is succeeded
     */
    bool unlock_read(index_t index, bool commit = true) LFDS_NOEXCEPT;

    /**
     * Try clear the buffer.
     * @returns whether the operation is succeeded
     */
    bool try_clear() LFDS_NOEXCEPT;

    /**
     * Clear the buffer.
     */
    void clear() LFDS_NOEXCEPT;

    /**
     * Obtain the reference of the specified item.
     * @param index [in] index (0 .. (N - 1))
     * @returns reference of the item
     */
    /// @{
    T &at(index_t index) LFDS_NOEXCEPT;
    const T &at(index_t index) const LFDS_NOEXCEPT;
    /// @}

    /// @cond INTERNAL_FIELD
private:
    typedef std::atomic<std::uint32_t> atomic_uint32_t;

    static LFDS_CONSTEXPR uint32_t WRITE_POINTER_MASK = 0x00007fffUL;
    static LFDS_CONSTEXPR uint32_t WRITE_LOCK_BIT_MASK = 0x00008000UL;

    static LFDS_CONSTEXPR uint32_t READ_POINTER_MASK = 0x7fff0000UL;
    static LFDS_CONSTEXPR uint32_t READ_LOCK_BIT_MASK = 0x80000000UL;

    static index_t write_pointer(uint32_t x) LFDS_NOEXCEPT;
    static bool is_write_locked(uint32_t x) LFDS_NOEXCEPT;
    static index_t read_pointer(uint32_t x) LFDS_NOEXCEPT;
    static bool is_read_locked(uint32_t x) LFDS_NOEXCEPT;
    static index_t next_pointer(index_t x) LFDS_NOEXCEPT;
    static index_t prev_pointer(index_t x) LFDS_NOEXCEPT;
    static uint32_t mod_write_pointer(uint32_t x, index_t wp) LFDS_NOEXCEPT;
    static uint32_t mod_read_pointer(uint32_t x, index_t rp) LFDS_NOEXCEPT;
    static uint32_t mod_write_locked(uint32_t x, bool locked) LFDS_NOEXCEPT;
    static uint32_t mod_read_locked(uint32_t x, bool locked) LFDS_NOEXCEPT;
    static size_t count(uint32_t x) LFDS_NOEXCEPT;

private:
    atomic_uint32_t rpwp_;
    T elements_[N];
    /// @endcond
};

template <typename T, size_t N>
inline lockfree_circulation_buffer<T, N>::lockfree_circulation_buffer()
    : rpwp_(0)
{
    static_assert((N >= 2) && (N <= 32768) && ((N & (N - 1)) == 0), "template argument N is invalid");
}

template <typename T, size_t N>
inline lockfree_circulation_buffer<T, N>::~lockfree_circulation_buffer()
{
}

template <typename T, size_t N>
inline typename lockfree_circulation_buffer<T, N>::index_t lockfree_circulation_buffer<T, N>::write_pointer(uint32_t x)
    LFDS_NOEXCEPT
{
    return static_cast<index_t>((x & WRITE_POINTER_MASK) >> 0);
}

template <typename T, size_t N>
inline bool lockfree_circulation_buffer<T, N>::is_write_locked(uint32_t x) LFDS_NOEXCEPT
{
    return (x & WRITE_LOCK_BIT_MASK) != 0;
}

template <typename T, size_t N>
inline typename lockfree_circulation_buffer<T, N>::index_t lockfree_circulation_buffer<T, N>::read_pointer(uint32_t x)
    LFDS_NOEXCEPT
{
    return static_cast<index_t>((x & READ_POINTER_MASK) >> 16U);
}

template <typename T, size_t N>
inline bool lockfree_circulation_buffer<T, N>::is_read_locked(uint32_t x) LFDS_NOEXCEPT
{
    return (x & READ_LOCK_BIT_MASK) != 0;
}

template <typename T, size_t N>
inline typename lockfree_circulation_buffer<T, N>::index_t
lockfree_circulation_buffer<T, N>::next_pointer(lockfree_circulation_buffer<T, N>::index_t x) LFDS_NOEXCEPT
{
    return (x + 1U) % N;
}

template <typename T, size_t N>
inline typename lockfree_circulation_buffer<T, N>::index_t
lockfree_circulation_buffer<T, N>::prev_pointer(lockfree_circulation_buffer<T, N>::index_t x) LFDS_NOEXCEPT
{
    return (x - 1U) % N;
}

template <typename T, size_t N>
inline uint32_t lockfree_circulation_buffer<T, N>::mod_write_pointer(uint32_t x,
                                                                     lockfree_circulation_buffer<T, N>::index_t wp)
    LFDS_NOEXCEPT
{
    return (x & ~WRITE_POINTER_MASK) | (wp << 0U);
}

template <typename T, size_t N>
inline uint32_t lockfree_circulation_buffer<T, N>::mod_read_pointer(uint32_t x,
                                                                    lockfree_circulation_buffer<T, N>::index_t rp)
    LFDS_NOEXCEPT
{
    return (x & ~READ_POINTER_MASK) | (rp << 16U);
}

template <typename T, size_t N>
inline uint32_t lockfree_circulation_buffer<T, N>::mod_write_locked(uint32_t x, bool locked) LFDS_NOEXCEPT
{
    if (locked)
        return (x | WRITE_LOCK_BIT_MASK);
    else
        return (x & ~WRITE_LOCK_BIT_MASK);
}

template <typename T, size_t N>
inline uint32_t lockfree_circulation_buffer<T, N>::mod_read_locked(uint32_t x, bool locked) LFDS_NOEXCEPT
{
    if (locked)
        return (x | READ_LOCK_BIT_MASK);
    else
        return (x & ~READ_LOCK_BIT_MASK);
}

template <typename T, size_t N>
inline size_t lockfree_circulation_buffer<T, N>::count(uint32_t x) LFDS_NOEXCEPT
{
    const index_t rp = read_pointer(x);
    const index_t wp = write_pointer(x);

    if (wp >= rp) {
        return (wp - rp);
    } else {
        return (N - rp + wp);
    }
}

template <typename T, size_t N>
inline bool lockfree_circulation_buffer<T, N>::empty() const LFDS_NOEXCEPT
{
    const uint32_t rpwp = rpwp_;
    return read_pointer(rpwp) == write_pointer(rpwp);
}

template <typename T, size_t N>
inline bool lockfree_circulation_buffer<T, N>::full() const LFDS_NOEXCEPT
{
    return (size() == capacity());
}

template <typename T, size_t N>
inline size_t lockfree_circulation_buffer<T, N>::size() const LFDS_NOEXCEPT
{
    return count(rpwp_);
}

template <typename T, size_t N>
inline size_t lockfree_circulation_buffer<T, N>::capacity() const LFDS_NOEXCEPT
{
    return N - 1;
}

template <typename T, size_t N>
inline bool lockfree_circulation_buffer<T, N>::lock_write(typename lockfree_circulation_buffer<T, N>::index_t &index,
                                                          size_t min_free) LFDS_NOEXCEPT
{
    uint32_t expected;
    uint32_t desired;
    bool result = true;

    expected = rpwp_.load(std::memory_order_acquire);
    do {
        // check is already locked
        if (CXXPH_UNLIKELY(is_write_locked(expected))) {
            result = false;
            break;
        }

        // check whether it has enough free spaces
        const size_t free_count = (N - 1) - count(expected);
        if (!(free_count > min_free)) {
            result = false;
            break;
        }

        desired = mod_write_locked(expected, true);
    } while (CXXPH_UNLIKELY(!rpwp_.compare_exchange_weak(expected, desired)));

    if (result) {
        index = write_pointer(desired);
    } else {
        index = INVALID_INDEX;
    }

    return result;
}

template <typename T, size_t N>
inline bool lockfree_circulation_buffer<T, N>::unlock_write(index_t index, bool commit) LFDS_NOEXCEPT
{
    uint32_t expected;
    uint32_t desired;
    bool result = true;

    expected = rpwp_.load(std::memory_order_acquire);
    do {
        // check the current write pointer
        if (CXXPH_UNLIKELY(index != write_pointer(expected))) {
            result = false;
            break;
        }

        // check is locked
        if (CXXPH_UNLIKELY(!is_write_locked(expected))) {
            result = false;
            break;
        }

        if (commit) {
            // calculate next write position
            const index_t next_wp = next_pointer(write_pointer(expected));
            desired = mod_write_pointer(expected, next_wp);
        } else {
            desired = expected;
        }

        // clear locked status bit
        desired = mod_write_locked(desired, false);
    } while (CXXPH_UNLIKELY(!rpwp_.compare_exchange_weak(expected, desired)));

    return result;
}

template <typename T, size_t N>
inline bool lockfree_circulation_buffer<T, N>::lock_read(lockfree_circulation_buffer<T, N>::index_t &index,
                                                         size_t min_remains) LFDS_NOEXCEPT
{
    uint32_t expected;
    uint32_t desired;
    bool result = true;

    expected = rpwp_.load(std::memory_order_acquire);
    do {
        // check is already locked
        if (CXXPH_UNLIKELY(is_read_locked(expected))) {
            result = false;
            break;
        }

        // check min count
        if (!(count(expected) > min_remains)) {
            result = false;
            break;
        }

        desired = mod_read_locked(expected, true);
    } while (CXXPH_UNLIKELY(!rpwp_.compare_exchange_weak(expected, desired)));

    if (result) {
        index = read_pointer(desired);
    } else {
        index = INVALID_INDEX;
    }

    return result;
}

template <typename T, size_t N>
inline bool lockfree_circulation_buffer<T, N>::unlock_read(index_t index, bool commit) LFDS_NOEXCEPT
{
    uint32_t expected;
    uint32_t desired;
    bool result = true;

    expected = rpwp_.load(std::memory_order_acquire);
    do {
        // check the current read pointer
        if (CXXPH_UNLIKELY(index != read_pointer(expected))) {
            result = false;
            break;
        }

        // check is locked
        if (CXXPH_UNLIKELY(!is_read_locked(expected))) {
            result = false;
            break;
        }

        if (commit) {
            // calculate next read position
            const uint16_t next_rp = next_pointer(read_pointer(expected));
            desired = mod_read_pointer(expected, next_rp);
        } else {
            desired = expected;
        }

        // clear locked status bit
        desired = mod_read_locked(desired, false);
    } while (CXXPH_UNLIKELY(!rpwp_.compare_exchange_weak(expected, desired)));

    return result;
}

template <typename T, size_t N>
inline bool lockfree_circulation_buffer<T, N>::try_clear() LFDS_NOEXCEPT
{
    uint32_t expected;
    uint32_t desired;
    bool result = true;

    expected = rpwp_.load(std::memory_order_acquire);
    do {
        if (CXXPH_LIKELY(!is_write_locked(expected)) && CXXPH_LIKELY(!is_read_locked(expected))) {
            desired = mod_read_pointer(expected, write_pointer(expected));
        } else {
            result = false;
            break;
        }
    } while (CXXPH_UNLIKELY(!rpwp_.compare_exchange_weak(expected, desired)));

    return result;
}

template <typename T, size_t N>
inline void lockfree_circulation_buffer<T, N>::clear() LFDS_NOEXCEPT
{
    while (!try_clear())
        ;
}

template <typename T, size_t N>
inline T &lockfree_circulation_buffer<T, N>::at(uint16_t index) LFDS_NOEXCEPT
{
    return elements_[index];
}

template <typename T, size_t N>
inline const T &lockfree_circulation_buffer<T, N>::at(uint16_t index) const LFDS_NOEXCEPT
{
    return elements_[index];
}

} // namespace lockfree

#endif // LOCKFREE_CIRCULATION_BUFFER_HPP_
