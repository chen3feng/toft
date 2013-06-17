// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/container/bitmap.h"

#include <algorithm>

namespace toft {

namespace details {

void BitmapBase::DoAndWith(WordType* words, const WordType* words2, size_t word_size)
{
    for (size_t i = 0; i < word_size; ++i)
         words[i] &= words2[i];
}

void BitmapBase::DoOrWith(WordType* words, const WordType* words2, size_t word_size)
{
    for (size_t i = 0; i < word_size; ++i)
         words[i] |= words2[i];
}

void BitmapBase::DoXorWith(WordType* words, const WordType* words2, size_t word_size)
{
    for (size_t i = 0; i < word_size; ++i)
         words[i] ^= words2[i];
}

void BitmapBase::DoSetAll(WordType* words, uint64_t size)
{
    size_t word_size = WordSizeOfBits(size);
    memset(words, 0xFF, word_size * sizeof(WordType));
    MaskOffTailBits(words, size);
}

bool BitmapBase::DoAllAreSet(const WordType* words, uint64_t num_bits)
{
    const size_t last_word_index = num_bits / kBitsPerWord;
    const size_t tail_bits = num_bits % kBitsPerWord;

    for (size_t i = 0; i < last_word_index; ++i)
    {
        if (words[i] != kAllSetMask)
            return false;
    }

    if (tail_bits > 0)
    {
        WordType tail_word = words[last_word_index];
        WordType mask = (1ULL << tail_bits) - 1;
        if ((tail_word & mask) != mask)
            return false;
    }

    return true;
}

bool BitmapBase::DoAllAreClear(const WordType* words, uint64_t num_bits)
{
    const size_t last_word_index = num_bits / kBitsPerWord;
    const size_t tail_bits = num_bits % kBitsPerWord;

    for (size_t i = 0; i < last_word_index; ++i)
    {
        if (words[i] != 0)
            return false;
    }

    if (tail_bits > 0)
    {
        WordType tail_word = words[last_word_index];
        if ((tail_word >> tail_bits << tail_bits) != tail_word)
            return false;
    }

    return true;
}

bool BitmapBase::DoAllAreClearInRange(const WordType* words, size_t start, size_t end)
{
    size_t start_word_index = start / kBitsPerWord;
    size_t start_tail_bits = start % kBitsPerWord;

    size_t end_word_index = end / kBitsPerWord;
    size_t end_tail_bits = end % kBitsPerWord;

    // span different words
    if (start_word_index != end_word_index)
    {
        // check header bits
        if (start_tail_bits > 0)
        {
            if ((words[start_word_index] >> start_tail_bits) != 0)
                return false;
            // skip these tested bits
            ++start_word_index;
        }

        // check middle part, can checked be word by word because all the bits of
        // these words are in the range
        for (size_t i = start_word_index; i < end_word_index; ++i)
        {
            if (words[i] != 0)
                return false;
        }

        // check tail bits
        if (end_tail_bits > 0)
        {
            WordType mask = ~WordType((1ULL << end_tail_bits) - 1);
            if ((words[end_word_index] | mask) != mask)
                return false;
        }
    }
    else // in same words
    {
        WordType word = words[start_word_index];
        return ((word >> start_tail_bits) << (kBitsPerWord - end_tail_bits + start_tail_bits)) == 0;
    }

    return true;
}

bool BitmapBase::DoIsSubsetOf(
    const WordType* words,
    const WordType* fullset_words,
    uint64_t num_bits)
{
    const size_t last_word_index = num_bits / kBitsPerWord;
    const size_t tail_bits = num_bits % kBitsPerWord;

    for (size_t i = 0; i < last_word_index; ++i)
    {
        if ((words[i] | fullset_words[i]) != fullset_words[i])
            return false;
    }

    if (tail_bits > 0)
    {
        WordType mask = (1ULL << tail_bits) - 1;
        WordType tail_word = words[last_word_index] & mask;
        WordType fullset_tail_word = fullset_words[last_word_index] & mask;
        if ((tail_word | fullset_tail_word) != fullset_tail_word)
            return false;
    }

    return true;
}

void BitmapBase::DoLeftShift(WordType* words, uint64_t num_bits, size_t shift)
{
    const size_t last_word_index = shift / kBitsPerWord;
    const size_t tail_bits = shift % kBitsPerWord;
    const size_t word_size = WordSizeOfBits(num_bits);

    if (tail_bits == 0)
    {
        for (size_t n = word_size - 1; n >= last_word_index; --n)
            words[n] = words[n - last_word_index];
    }
    else
    {
        const size_t sub_offset = kBitsPerWord - tail_bits;
        for (size_t n = word_size - 1; n > last_word_index; --n)
            words[n] = ((words[n - last_word_index] << tail_bits)
                        | (words[n - last_word_index - 1] >> sub_offset));
        words[last_word_index] = words[0] << tail_bits;
    }

    std::fill(words, words + last_word_index, static_cast<WordType>(0));
    MaskOffTailBits(words, num_bits);
}

void BitmapBase::DoRightShift(WordType* words, uint64_t word_size, size_t shift)
{
    const size_t last_word_index = shift / kBitsPerWord;
    const size_t tail_bits = shift % kBitsPerWord;
    const size_t limit = word_size - last_word_index - 1;

    if (tail_bits == 0)
    {
        for (size_t n = 0; n <= limit; ++n)
            words[n] = words[n + last_word_index];
    }
    else
    {
        const size_t sub_offset = kBitsPerWord - tail_bits;
        for (size_t n = 0; n < limit; ++n)
            words[n] = ((words[n + last_word_index] >> tail_bits)
                        | (words[n + last_word_index + 1] << sub_offset));
        words[limit] = words[word_size-1] >> tail_bits;
    }

    std::fill(words + limit + 1, words + word_size, static_cast<WordType>(0));
}

bool BitmapBase::DoFindFirst(WordType* words, size_t word_size, size_t* result)
{
    for (size_t i = 0; i < word_size; i++)
    {
        WordType thisword = words[i];
        if (thisword != static_cast<WordType>(0))
        {
            *result = (i * kBitsPerWord
                    + __builtin_ctzl(thisword));
            return true;
        }
    }
    return false;
}

bool BitmapBase::DoFindNext(WordType* words, size_t word_size, size_t prev, size_t* result)
{
    // make bound inclusive
    ++prev;

    // check out of bounds
    if (prev >= word_size * kBitsPerWord)
        return false;

    // search first word
    size_t i = WordIndexOfBit(prev);
    WordType thisword = words[i];

    // mask off bits below bound
    thisword &= kAllSetMask << ShiftOfBit(prev);

    if (thisword != static_cast<WordType>(0))
    {
        *result = (i * kBitsPerWord
                + __builtin_ctzl(thisword));
        return true;
    }

    // check subsequent words
    i++;
    for (; i < word_size; i++)
    {
        thisword = words[i];
        if (thisword != static_cast<WordType>(0))
        {
            *result = (i * kBitsPerWord
                    + __builtin_ctzl(thisword));
            return true;
        }
    }

    return false;
}

void BitmapBase::DoAppendToString(const WordType* words, uint64_t num_bits, std::string* out)
{
    out->reserve(out->size() + num_bits);
    for (uint64_t i = num_bits; i > 0; --i)
        out->push_back(DoGetAt(words, i - 1) ? '1' : '0');
}

} // namespace details

} // namespace toft

