#ifndef __TL__
#define __TLV__

#include <stdint.h>
#include <memory>
#include <string.h>

#define TVL_OVERHEAD_SIZE 2

/*MACRO To Type Length Value reply
 *char* -start_ptr, IN
 *unsigned char -type,OUT
 *unsigned char -lenght, OUT
 *unsigned char* -tlv_ptr OUT
 *unsigned in -total_size(excluding first 8 bytes),IN
 */
#define ITERATE_TLV_BEGIN(start_ptr, type, length, tlv_ptr, tlv_size)                                                                                                         \
    {                                                                                                                                                                         \
        unsigned int _len = 0;                                                                                                                                                \
        char _tlv_value_size = 0;                                                                                                                                             \
        type = 0;                                                                                                                                                             \
        length = 0;                                                                                                                                                           \
        char *tlv_ptr = NULL;                                                                                                                                                 \
        for (tlv_ptr = (char *)start_ptr + TVL_OVERHEAD_SIZE; _len < tlv_size; _len += _tlv_value_size + TVL_OVERHEAD_SIZE, tlv_ptr = (tlv_ptr + TVL_OVERHEAD_SIZE + length)) \
        {                                                                                                                                                                     \
            type = *(tlv_ptr - TVL_OVERHEAD_SIZE);                                                                                                                            \
            _tlv_value_size = char(*(tlv_ptr - TVL_OVERHEAD_SIZE + sizeof(char)));                                                                                            \
            length = _tlv_value_size;

#define ITERATE_TLV_END \
    }                   \
    }

static inline char *tlv_buffer_inser_tlv(char *buff, uint8_t tlv_no, uint8_t data_len, char *data)
{
    *buff = tlv_no;
    *(buff + 1) = data_len;
    if (data)
    {
        memcpy(buff + TVL_OVERHEAD_SIZE, data, data_len);
    }
    return buff + TVL_OVERHEAD_SIZE + data_len;
}

static inline char *tlv_buffer_get_particualr_tlv(char *tlv_buff, uint32_t tlv_buff_size, uint8_t tlv_no, uint8_t *tlv_data_len)
{
    char tlv_type, tlv_len, *tlv_value = NULL;
    ITERATE_TLV_BEGIN(tlv_buff, tlv_type, tlv_len, tlv_value, tlv_buff_size);
    if (tlv_type != tlv_no)
    {
        continue;
    }
    else
    {
        *tlv_data_len = tlv_len;
        return (tlv_value);
    }
    ITERATE_TLV_END(tlv_buff, tlv_type, tlv_len, tlv_value, tlv_buff_size);
    *tlv_data_len = 0;
    return NULL;
}

#endif