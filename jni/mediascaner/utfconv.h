#ifndef UTFCONV_H
#define UTFCONV_H

/**
    处理UTF-16LE(ucs2)到UTF-8
    @result 转换的字节数量
*/
extern size_t conv_cp936_utf8(const uint8_t *src, const size_t srclen,char *dest,size_t *outlen);
/**
    处理UTF-16LE到UTF-8
    @result 转换的字节数量
*/
extern size_t conv_utf16le_utf8(const uint16_t *src, const size_t srclen,char *dest,size_t *outlen);
/**
    处理UTF-16BE(ucs2)到UTF-8
    @result 转换的字节数量
*/
extern size_t conv_utf16be_utf8(const uint16_t *src, const size_t srclen,char *dest,size_t *outlen);



/**
 * GBK/GB2312/ISO8859-1  --->  UTF-16LE(ucs2)
 @result  转换的字节数量
 */
extern size_t conv_cp936_ucs2(const uint8_t *src, const size_t srclen,char *dest,size_t *outlen);
/**
    处理UTF-8到UTF16-LE
    @result 转换的utf8字节数量
*/
extern size_t conv_utf8_ucs2(const uint8_t *src, const size_t srclen,uint16_t *dest,size_t *outlen);



/**
    处理UTF-8到GBK
    @result 转换的utf8字节数量
*/
extern size_t conv_utf8_cp936(const uint8_t *src, const size_t srclen,char *dest,size_t *outlen);

/**
    处理UTF-16LE到GBK
    @result 转换的unicode双字节数量
*/
extern size_t conv_ucs2_cp936(const uint8_t *src, const size_t srclen,char *dest,size_t *outlen);

/**
    获得UTF-16字符串的中文拼音首字
    @result 转换的unicode双字节数量
*/
extern  size_t ucs2_getLetter(const uint16_t*ucs2,const size_t ucslen,char*dest,size_t *outlen);
#endif // UTFCONV_H
