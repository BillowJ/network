#ifndef __ENDIAN_H__
#define __ENDIAN_H__

#include <byteswap.h>
#include <stdint.h>

#define GlOBAL_BIG_ENDIAN 1
#define GLOBAL_LITTLE_ENDIAN 2

namespace Global
{

/**
 * @brief 8字节类型的字节序转化
 */
template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T >::type
byteswap(T value)
{
    return (T)bswap_64((uint64_t)value);
}

/**
 * @brief 4字节类型的字节序转化
 */
template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T >::type
byteswap(T value)
{
    return (T)bswap_32((uint32_t)value);
}

/**
 * @brief 2字节类型的字节序转化
 */
template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
byteswap(T value) {
    return (T)bswap_16((uint16_t)value);
}

#if BYTE_ORDER == BIG_ENDIAN
#define GLOBAL_BYTE_ORDER GLOBAL_BIG_ENDIAN
#else
#define GLOBAL_BYTE_ORDER GLOBAL_LITTLE_ENDIAN
#endif

#if GLOBAL_BYTE_ORDER == GLOBAL_BIG_ENDIAN

/**
 * @brief 只在小端机器上执行byteswap, 在大端机器上什么都不做
 */
template<class T>
T byteswapOnLittleEndian(T t)
{
    return t;
}

template<class T>
T byteswapOnBigEndian(T t)
{
    return byteswap(t);
}

#else

template<class T>
T byteswapOnLittleEndian(T t)
{
    return byteswap(t);
}

template<class T>
T byteswapOnBigEndian(T t)
{
    return t;
}

#endif


} // namespace Global


#endif