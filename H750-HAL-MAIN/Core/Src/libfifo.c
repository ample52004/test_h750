
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "libfifo.h"
#include <stddef.h>

/**
  * @brief  注册一个fifo
  * @param  pfifo: fifo结构体指针
            pfifo_buf: fifo内存块
            size: 长度
  * @retval none
*/
void fifo_register(_fifo_t *pfifo, uint8_t *pfifo_buf, uint32_t size,
                   lock_fun lock, lock_fun unlock)
{
    pfifo->buf_size = size;
    pfifo->buf  = pfifo_buf;
    pfifo->pwrite = pfifo->buf;
    pfifo->pread  = pfifo->buf;
    pfifo->occupy_size = 0;
    pfifo->lock = lock;
    pfifo->unlock = unlock;
}

/**
  * @brief  释放fifo
  * @param  pfifo: fifo结构体指针
  * @retval none
*/
void fifo_release(_fifo_t *pfifo)
{
    pfifo->buf_size = 0;
    pfifo->occupy_size = 0;
    pfifo->buf  = NULL;
    pfifo->pwrite = 0;
    pfifo->pread  = 0;
    pfifo->lock = NULL;
    pfifo->unlock = NULL;
}

/**
  * @brief  往fifo写数据
  * @param  pfifo: fifo结构体指针
            pbuf: 待写数据
            size: 待写数据大小
  * @retval 实际写大小
*/
uint32_t fifo_write(_fifo_t *pfifo, const uint8_t *pbuf, uint32_t size)
{
    uint32_t w_size = 0, free_size = 0;

    if ((size == 0) || (pfifo == NULL) || (pbuf == NULL))
    {
        return 0;
    }

    free_size = fifo_get_free_size(pfifo);//获取空闲空间大小
    if (free_size == 0)
    {
        return 0;
    }

    if (free_size < size)//判断空闲空间是否足够
    {
        size = free_size;
    }
    w_size = size;
    if (pfifo->lock != NULL)
        pfifo->lock();
    while (w_size-- > 0)
    {
        *pfifo->pwrite++ = *pbuf++;//buf里的数据赋值到写入到fifo,地址+1
        if (pfifo->pwrite >= (pfifo->buf + pfifo->buf_size))//判断内存有没有溢出
        {
            pfifo->pwrite = pfifo->buf;//回到初始地址
        }
        pfifo->occupy_size++;//占用空间+1
    }
    if (pfifo->unlock != NULL)
        pfifo->unlock();
    return size;
}

/**
  * @brief  从fifo读数据
  * @param  pfifo: fifo结构体指针
            pbuf: 待读数据缓存
            size: 待读数据大小
  * @retval 实际读大小
*/
uint32_t fifo_read(_fifo_t *pfifo, uint8_t *pbuf, uint32_t size)
{
    uint32_t r_size = 0, occupy_size = 0;

    if ((size == 0) || (pfifo == NULL) || (pbuf == NULL))
    {
        return 0;
    }
	
    occupy_size = fifo_get_occupy_size(pfifo);//获取获取fifo已用空间大小
    if (occupy_size == 0)
    {
        return 0;
    }

    if (occupy_size < size)
    {
        size = occupy_size;
    }
    if (pfifo->lock != NULL)
        pfifo->lock();
    r_size = size;
    while (r_size-- > 0)
    {
        *pbuf++ = *pfifo->pread++;
        if (pfifo->pread >= (pfifo->buf + pfifo->buf_size))//判断地址是否超出
        {
            pfifo->pread = pfifo->buf;//初始地址
        }
        pfifo->occupy_size--;//占用空间-1，每读取一个字节就释放一个
    }
    if (pfifo->unlock != NULL)
        pfifo->unlock();
    return size;
}

/**
  * @brief  获取fifo空间大小
  * @param  pfifo: fifo结构体指针
  * @retval fifo大小
*/
uint32_t fifo_get_total_size(_fifo_t *pfifo)
{
    if (pfifo == NULL)
        return 0;

    return pfifo->buf_size;
}

/**
  * @brief  获取fifo空闲空间大小
  * @param  pfifo: fifo结构体指针
  * @retval 空闲空间大小
*/
uint32_t fifo_get_free_size(_fifo_t *pfifo)
{
    uint32_t size;

    if (pfifo == NULL)
        return 0;

    size = pfifo->buf_size - fifo_get_occupy_size(pfifo);

    return size;
}

/**
  * @brief  获取fifo已用空间大小
  * @param  pfifo: fifo结构体指针
  * @retval fifo已用大小
*/
uint32_t fifo_get_occupy_size(_fifo_t *pfifo)
{
    if (pfifo == NULL)
        return 0;

    return  pfifo->occupy_size;
}

