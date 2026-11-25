#include "App_CircularBuffer.h"

/**
 * @brief 初始化循环缓冲区。
 * @note  将缓冲区的所有元素初始化为0，并将写指针和读指针都初始化为0。
 * @param circular_buffer 循环缓冲区结构体指针。
 * @retval None
 */
void init_buffer(circular_buffer_t *circular_buffer)
{
    memset(circular_buffer->buffer, 0, BUFFER_LENGTH);                                          // 数组的实际长度
    circular_buffer->write_index = circular_buffer->read_index = 0;
}

/**
 * @brief 检查循环缓冲区是否为空。
 * @note 如果写指针和读指针相等，则缓冲区为空。
 * @param circular_buffer 循环缓冲区结构体指针。
 * @retval true 缓冲区为空，false 缓冲区不为空。
 */
bool is_empty(circular_buffer_t *circular_buffer)
{
    return (circular_buffer->write_index == circular_buffer->read_index);
}

/**
 * @brief 检查循环缓冲区是否已满。
 * @note 如果写指针的下一个位置是读指针，则缓冲区已满。
 * @param circular_buffer 循环缓冲区结构体指针。
 * @retval true 缓冲区已满，false 缓冲区未满。
 */
bool is_full(circular_buffer_t *circular_buffer)
{
    return ((circular_buffer->write_index + 1) % BUFFER_LENGTH == circular_buffer->read_index);
}

/**
 * @brief 将数据写入循环缓冲区。
 * @note 写指针初始化为0，从0-BUFFER_LENGTH-1循环写入数据，写入数据后写指针加1，如果写指针加1后等于缓冲区长度，则将写指针置为0。
 * @param circular_buffer 循环缓冲区结构体指针。
 * @param data 要写入的数据。
 * @retval None
 */
void write_to_buffer(circular_buffer_t *circular_buffer, char data)
{
    if (is_full(circular_buffer))
        return;
    circular_buffer->buffer[circular_buffer->write_index] = data;                               // 将写指针对应的数值赋值给data
    circular_buffer->write_index++;
    if (circular_buffer->write_index == BUFFER_LENGTH)
        circular_buffer->write_index = 0;
}


/**
 * @brief 从循环缓冲区读取一个字节。
 * @note 读指针初始化为0，从0-BUFFER_LENGTH-1循环读取数据，读取数据后读指针加1，如果读指针加1后等于缓冲区长度，则将读指针置为0。
 * @param circular_buffer 循环缓冲区结构体指针。
 * @retval 读取到的数据。
 */
char read_one_byte_from_buffer(circular_buffer_t *circular_buffer)
{
    char is_empty_note = 'E';                                                                   // 用于标记缓冲区为空的情况
    if (is_empty(circular_buffer))
    {
        return is_empty_note;
    }
    char byte = 0;
    byte = circular_buffer->buffer[circular_buffer->read_index];
    circular_buffer->buffer[circular_buffer->read_index] = 0;
    circular_buffer->read_index++;
    if (circular_buffer->read_index == BUFFER_LENGTH)
        circular_buffer->read_index = 0;
    return byte;
}

/**
 * @brief 从循环缓冲区读取一个字符串。
 * @note 读取指定长度的字符串，如果缓冲区为空，则返回"Buffer is empty"。
 * @param circular_buffer 循环缓冲区结构体指针。
 * @param str 用于存储读取到的字符串的字符数组指针。
 * @param length 要读取的字符串长度。
 * @retval 读取到的字符串指针。
 */
char *read_string_from_buffer(circular_buffer_t *circular_buffer, char *str, int length)
{
    char *is_empty_note = "Buffer is empty.";                                                   // 用于标记缓冲区为空的情况
    if (is_empty(circular_buffer))
    {
        return is_empty_note;
    }
    if (length <= 0)
        return str;
    memset(str, 0, length);                                                                     // 初始化解析指针
    for (int i = 0; i < length; i++)
    {
        str[i] = read_one_byte_from_buffer(circular_buffer);
        if (str[i] == '\0')
            break;                                                                              // 检测到结束符自动停止接收
    }
    return str;
}

char *read_all_string_from_buffer(circular_buffer_t *circular_buffer, char *str)
{
    char *is_empty_note = "Buffer is empty";                                                    // 用于标记缓冲区为空的情况
    if (is_empty(circular_buffer))
    {
        return is_empty_note;
    }
    int length = 0;
    if (circular_buffer->write_index >= circular_buffer->read_index)
    {
        length = circular_buffer->write_index - circular_buffer->read_index;                    // 如果写指针大于读指针，则长度为写指针减去读指针
    }
    else
    {
        length = BUFFER_LENGTH - circular_buffer->read_index + circular_buffer->write_index;    // 反之则为缓冲区长度减去读指针加上写指针
    }
    memset(str, 0, length);
    int valid_length = 0;
    for (int i = 0; i < length; i++)
    {
        char ch = read_one_byte_from_buffer(circular_buffer);
        if (ch != '\0')
            str[valid_length++] = ch;
    }
    str[valid_length] = '\0';
    return str;
}
