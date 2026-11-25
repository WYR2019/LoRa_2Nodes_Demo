#ifndef __CIRCULAR_BUFFER_C__
#define __CIRCULAR_BUFFER_C__

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define BUFFER_LENGTH                                                                      1024

/**
 * @brief 循环缓冲区结构体
 * @note 包含缓冲区数组和读写指针
 */
typedef struct 
{
    char buffer[BUFFER_LENGTH];
    int write_index, read_index;
}circular_buffer_t;

void init_buffer(circular_buffer_t *circular_buffer);
bool is_empty(circular_buffer_t *circular_buffer);
bool is_full(circular_buffer_t *circular_buffer);
void write_to_buffer(circular_buffer_t *circular_buffer, char data);
char read_one_byte_from_buffer(circular_buffer_t *circular_buffer);
char *read_string_from_buffer(circular_buffer_t *circular_buffer, char *str, int length);
char *read_all_string_from_buffer(circular_buffer_t *circular_buffer, char *str);

#endif
