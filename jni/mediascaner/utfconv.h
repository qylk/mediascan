#ifndef UTFCONV_H
#define UTFCONV_H

/**
    ����UTF-16LE(ucs2)��UTF-8
    @result ת�����ֽ�����
*/
extern size_t conv_cp936_utf8(const uint8_t *src, const size_t srclen,char *dest,size_t *outlen);
/**
    ����UTF-16LE��UTF-8
    @result ת�����ֽ�����
*/
extern size_t conv_utf16le_utf8(const uint16_t *src, const size_t srclen,char *dest,size_t *outlen);
/**
    ����UTF-16BE(ucs2)��UTF-8
    @result ת�����ֽ�����
*/
extern size_t conv_utf16be_utf8(const uint16_t *src, const size_t srclen,char *dest,size_t *outlen);



/**
 * GBK/GB2312/ISO8859-1  --->  UTF-16LE(ucs2)
 @result  ת�����ֽ�����
 */
extern size_t conv_cp936_ucs2(const uint8_t *src, const size_t srclen,char *dest,size_t *outlen);
/**
    ����UTF-8��UTF16-LE
    @result ת����utf8�ֽ�����
*/
extern size_t conv_utf8_ucs2(const uint8_t *src, const size_t srclen,uint16_t *dest,size_t *outlen);



/**
    ����UTF-8��GBK
    @result ת����utf8�ֽ�����
*/
extern size_t conv_utf8_cp936(const uint8_t *src, const size_t srclen,char *dest,size_t *outlen);

/**
    ����UTF-16LE��GBK
    @result ת����unicode˫�ֽ�����
*/
extern size_t conv_ucs2_cp936(const uint8_t *src, const size_t srclen,char *dest,size_t *outlen);

/**
    ���UTF-16�ַ���������ƴ������
    @result ת����unicode˫�ֽ�����
*/
extern  size_t ucs2_getLetter(const uint16_t*ucs2,const size_t ucslen,char*dest,size_t *outlen);
#endif // UTFCONV_H
