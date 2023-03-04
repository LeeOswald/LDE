#ifndef __LIBLDE_HELPERS_H_INCLUDED__
#define __LIBLDE_HELPERS_H_INCLUDED__

static inline bool find_8(const uint32_t* array, uint8_t val)
{
    return ((array[(val >> 5) & 7] & (0x80000000 >> (val & 0x1F))) != 0);
}

static inline bool find_2(const uint32_t* array, uint8_t val)
{
    if (val < 0x40)
    {
        return ((array[((val >> 5) & 7)] & (0x80000000 >> (val & 0x1F))) != 0);
    }

    return false;
}

static inline bool in_range(uint8_t lo, uint8_t hi, uint8_t val)
{
    return ((val >= lo) && (val < hi));
}

typedef struct
{
    const uint8_t* start;
    uint32_t length;
    uint32_t position;
} byte_iterator;

static inline byte_iterator byte_iterator_init(const uint8_t* array, uint32_t size)
{
    byte_iterator it =
    {
        .start = array,
        .length = size,
        .position = 0
    };

    return it;
}

static inline bool byte_iterator_next(byte_iterator* it, uint8_t* v)
{
    if (it->position >= it->length)
        return false;

    *v = it->start[it->position];
    ++it->position;

    return true;
}

#endif // __LIBLDE_HELPERS_H_INCLUDED__
