// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 06/22/11
// Description: optimized bitmap

#ifndef TOFT_CONTAINER_BITMAP_H
#define TOFT_CONTAINER_BITMAP_H
#pragma once

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdexcept>
#include <string>
#include <vector>

#include "toft/base/string/string_piece.h"

namespace toft {

namespace details {

// for implementation detail
class BitmapBase
{
protected:
    typedef size_t WordType;
    static const size_t kBitsPerWord = sizeof(WordType) * CHAR_BIT;
    static const WordType kAllSetMask = ~WordType(0);

    static size_t WordSizeOfBits(uint64_t num_bits)
    {
        return (num_bits + kBitsPerWord - 1) / kBitsPerWord;
    }

    template <uint64_t num_bits>
    struct StaticWordSizeOfBits
    {
        static const size_t Value = (num_bits + kBitsPerWord - 1) / kBitsPerWord;
    };

    static size_t WordIndexOfBit(size_t index)
    {
        return index / kBitsPerWord;
    }
    static WordType ShiftOfBit(size_t index)
    {
        return index % kBitsPerWord;
    }
    static WordType MaskOfBit(size_t index)
    {
        return 1ULL << ShiftOfBit(index);
    }
    static WordType FirstWordMask(uint64_t start)
    {
        return ~0ULL << (start % kBitsPerWord);
    }

    static WordType LastWordMask(uint64_t end)
    {
        return end % kBitsPerWord ? (1ULL << (end % kBitsPerWord)) - 1 : ~0ULL;
    }

    static void MaskOffTailBits(WordType* words, uint64_t size)
    {
        size_t word_size = WordIndexOfBit(size);
        size_t tail_bits = size % kBitsPerWord;
        if (tail_bits > 0)
            words[word_size] &= ~(kAllSetMask << tail_bits);
    }

    template <typename IndexType>
    __attribute__((noinline))
    static bool DoGetAt(const WordType* words, IndexType index)
    {
        size_t offset = WordIndexOfBit(index);
        WordType mask = MaskOfBit(index);
        return (words[offset] & mask) != 0;
    }

    template <typename IndexType>
    __attribute__((noinline))
    static void DoSetAt(WordType* words, IndexType index)
    {
        size_t offset = WordIndexOfBit(index);
        WordType mask = MaskOfBit(index);
        words[offset] |= mask;
    }

    template <typename IndexType>
    __attribute__((noinline))
    static void DoClearAt(WordType* words, IndexType index)
    {
        size_t offset = WordIndexOfBit(index);
        WordType mask = ~MaskOfBit(index);
        words[offset] &= mask;
    }

    template <typename IndexType>
    static void DoSetAt(WordType* words, IndexType index, bool value)
    {
        if (value)
            DoSetAt(words, index);
        else
            DoClearAt(words, index);
    }

    static void DoSetAll(WordType* words, uint64_t size);
    static void DoClearAll(WordType* words, size_t size)
    {
        memset(words, 0, size);
    }

    static bool DoAllAreSet(const WordType* words, uint64_t num_bits);
    static bool DoAllAreClear(const WordType* words, uint64_t num_bits);

    static bool DoIsSubsetOf(
        const WordType* words,
        const WordType* fullset_words,
        uint64_t num_bits);

    static bool DoAllAreClearInRange(const WordType* words, size_t start, size_t end);

    static void DoLeftShift(WordType* words, uint64_t num_bits, size_t shift);
    static void DoRightShift(WordType* words, uint64_t num_bits, size_t shift);

    static void DoAndWith(WordType* words, const WordType* words2, size_t word_size);
    static void DoOrWith(WordType* words, const WordType* words2, size_t word_size);
    static void DoXorWith(WordType* words, const WordType* words2, size_t word_size);

    static bool DoFindFirst(WordType* words, size_t word_size, size_t* result);
    static bool DoFindNext(WordType* words, size_t word_size, size_t prev, size_t* result);

    static void DoAppendToString(const WordType* words, uint64_t num_bits, std::string* out);
};

/// This class provide basic bitmap access interfaces
template <typename CocreteType, typename IndexType>
class BasicBitmap : public BitmapBase
{
    typedef BasicBitmap<CocreteType, IndexType> ThisType;
public:
    /// Total bit count
    IndexType Size() const
    {
        return static_cast<const CocreteType*>(this)->DoGetSize();
    }

    /// Return memory size in bytes, include padding bits
    size_t ByteSize() const
    {
        return WordSize() * sizeof(WordType);
    }

    /// Get a bit at specified index
    bool GetAt(IndexType index) const
    {
        assert(index < Size());
        return DoGetAt(Words(), index);
    }

    /// Set a bit at specified index
    void SetAt(IndexType index)
    {
        assert(index < Size());
        DoSetAt(Words(), index);
    }

    /// Clear a bit at specified index to zero
    void ClearAt(IndexType index)
    {
        assert(index < Size());
        DoClearAt(Words(), index);
    }

    /// Clear a bit at specified index to given value
    void SetAt(IndexType index, bool value)
    {
        assert(index < Size());
        DoSetAt(Words(), index, value);
    }

    /// Fill all bits of the bitmap
    void SetAll()
    {
        DoSetAll(Words(), Size());
    }

    /// Clear all bits of the bitmap
    void ClearAll()
    {
        memset(Words(), 0, ByteSize());
    }

    /// Are all bits set in the bitmap?
    bool AllAreSet() const
    {
        return DoAllAreSet(Words(), Size());
    }

    /// Are all bits clear in the bitmap?
    bool AllAreClear() const
    {
        return DoAllAreClear(Words(), Size());
    }

    /// Are all bits clear in the range of bitmap?
    bool AllAreClearInRange(IndexType start, IndexType end) const
    {
        assert(start <= Size());
        assert(end <= Size());
        return DoAllAreClearInRange(Words(), start, end);
    }

    /// Left shift all bits, fill 0 to the hollow
    void LeftShift(size_t shift)
    {
        if (shift != 0)
            DoLeftShift(Words(), Size(), shift);
    }

    /// Right shift all bits, fill 0 to the hollow
    void RightShift(size_t shift)
    {
        if (shift != 0)
            DoRightShift(Words(), WordSize(), shift);
    }

    /// Bitwise And with another bitmap, the two bitmap must be same size
    void AndWith(const ThisType& rhs)
    {
        assert(rhs.Size() == Size());
        DoAndWith(Words(), rhs.Words(), WordSize());
    }

    /// Bitwise Or with another bitmap, the two bitmap must be same size
    void OrWith(const ThisType& rhs)
    {
        assert(rhs.Size() == Size());
        DoOrWith(Words(), rhs.Words(), WordSize());
    }

    /// Bitwise Xor with another bitmap, the two bitmap must be same size
    void XorWith(const ThisType& rhs)
    {
        assert(rhs.Size() == Size());
        DoXorWith(Words(), rhs.Words(), WordSize());
    }

    /// Return whether this bitmap is subset of another bitmap
    bool IsSubsetOf(const ThisType& rhs) const
    {
        if (rhs.Size() != Size())
            return false;
        return DoIsSubsetOf(Words(), rhs.Words(), Size());
    }

    /// Return first bit position which is set to 1
    bool FindFirst(size_t* result)
    {
        return DoFindFirst(Words(), WordSize(), result);
    }

    /// Return next bit position which is set to 1 from prev position
    bool FindNext(size_t prev, size_t* result)
    {
        return DoFindNext(Words(), WordSize(), prev, result);
    }

    /// Append the bitmap as a 01 string to a string
    void AppendToString(std::string* out) const
    {
        DoAppendToString(Words(), Size(), out);
    }

    /// Convert this bitmap to a 01 string
    std::string ToString() const
    {
        std::string result;
        AppendToString(&result);
        return result;
    }

    bool ParseFromString(StringPiece initial_value)
    {
        if (initial_value.size() != Size())
            return false;

        for (size_t i = 0; i < initial_value.size(); ++i)
        {
            char ch = initial_value[i];
            if (ch == '1')
                this->SetAt(initial_value.size() - i - 1);
            else if (ch == '0')
                this->ClearAt(initial_value.size() - i - 1);
            else
                return false;
        }
        return true;
    }

private:
    const WordType* Words() const
    {
        return static_cast<const CocreteType*>(this)->DoGetWords();
    }
    WordType* Words()
    {
        return static_cast<CocreteType*>(this)->DoGetWords();
    }
    size_t WordSize() const
    {
        return static_cast<const CocreteType*>(this)->DoGetWordSize();
    }
};

// Using size_t as default bitmap index type
template <bool Use64BitIndex>
struct SelectLargeIndexType
{
    typedef size_t Type;
};

template <>
struct SelectLargeIndexType<true>
{
    typedef uint64_t Type;
};

template <uint64_t Size>
struct SelectIndexTypeBySize
{
    static const bool UseLargeIndexType =
        sizeof(size_t) != sizeof(uint64_t) && Size >= 0xFFFFFFFFUL; // NOLINT(runtime/sizeof)
    typedef typename SelectLargeIndexType<UseLargeIndexType>::Type Type;
};

/// Dynamic bitmap
template <typename IndexType>
class BasicDynamicBitmap :
    public BasicBitmap<BasicDynamicBitmap<IndexType>, IndexType>
{
    typedef BitmapBase::WordType WordType;
    friend class BasicBitmap<BasicDynamicBitmap<IndexType>, IndexType>;
public:
    explicit BasicDynamicBitmap(IndexType size, bool initial_value = false)
        : m_words(BitmapBase::WordSizeOfBits(size), initial_value ? BitmapBase::kAllSetMask : 0),
          m_size(size)
    {
        if (initial_value)
            BitmapBase::MaskOffTailBits(&m_words[0], size);
    }

    explicit BasicDynamicBitmap(StringPiece initial_value)
        : m_words(BitmapBase::WordSizeOfBits(initial_value.size())),
          m_size(initial_value.size())
    {
        if (!this->ParseFromString(initial_value))
            throw std::runtime_error("Invalid string to initialize bitmap");
    }

    void Resize(IndexType size)
    {
        m_words.resize(this->WordSizeOfBits(size));
        m_size = size;
    }

private:
    const BitmapBase::WordType* DoGetWords() const
    {
        return &m_words[0];
    }
    BitmapBase::WordType* DoGetWords()
    {
        return &m_words[0];
    }
    size_t DoGetSize() const
    {
        return m_size;
    }

    size_t DoGetWordSize() const
    {
        return m_words.size();
    }
private:
    std::vector<BitmapBase::WordType> m_words;
    IndexType m_size;
};

} // namespace details

template <uint64_t Size, typename IndexType = typename details::SelectIndexTypeBySize<Size>::Type >
class FixedBitmap : public details::BasicBitmap<FixedBitmap<Size, IndexType>, IndexType>
{
    typedef details::BitmapBase BitmapBase;
    typedef details::BitmapBase::WordType WordType;
    friend class details::BasicBitmap<FixedBitmap<Size, IndexType>, IndexType>;
public:
    FixedBitmap()
    {
        memset(m_words, 0, sizeof(m_words));
    }

    explicit FixedBitmap(bool initial_value)
    {
        if (initial_value)
        {
            this->DoSetAll(m_words, Size);
            this->MaskOffTailBits(m_words, Size);
        }
        else
        {
            memset(m_words, 0, sizeof(m_words));
        }
    }

    explicit FixedBitmap(StringPiece initial_value)
    {
        InitializeWithString(initial_value);
    }

    explicit FixedBitmap(const std::string& initial_value)
    {
        InitializeWithString(initial_value);
    }

    explicit FixedBitmap(const char* initial_value)
    {
        InitializeWithString(initial_value);
    }

private:
    void InitializeWithString(StringPiece initial_value)
    {
        memset(m_words, 0, sizeof(m_words));
        if (!this->ParseFromString(initial_value))
            throw std::runtime_error("Invalid string to initialize bitmap");
    }

    WordType* DoGetWords()
    {
        return m_words;
    }
    const WordType* DoGetWords() const
    {
        return m_words;
    }
    size_t DoGetWordSize() const
    {
        return sizeof(m_words)/ sizeof(m_words[0]);
    }
    IndexType DoGetSize() const
    {
        return Size;
    }
private:
    WordType m_words[BitmapBase::StaticWordSizeOfBits<Size>::Value];
};

// 32 bit bitmap
typedef details::BasicDynamicBitmap<uint32_t> DynamicBitmap;

// 64 bit bitmap
typedef details::BasicDynamicBitmap<uint64_t> DynamicBitmap64;

} // namespace toft

#endif // TOFT_CONTAINER_BITMAP_H
