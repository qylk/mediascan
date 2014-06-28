/**
 * The author of this file: qylk <www.qylk.blog.163.com>
 *
 */

typedef	unsigned char uint8_t;
typedef	unsigned short uint16_t;
#include "unicode_table_cp936.h"
#include "pinyin.h"
#include <stdint.h>
#define bswap16(x)  ((uint16_t)(( x & 0xff) << 8 | ( x & 0xff00) >> 8))


/**
 * GBK/GB2312/ISO8859-1  --->  UTF-16LE
 @result  转换的字节数量
 */
size_t conv_cp936_ucs2(const uint8_t *src, const size_t srclen,char *dest,size_t *outlen)
{
    uint8_t *w=(uint8_t*)dest;
    uint8_t cl;//next byte to check
    size_t index;
    size_t j=0;//completed byte
    for(size_t i=0; i<srclen; i++)
    {
        uint8_t ch=src[i];//first byte
        if (ch >= 0x00 && ch < 0x80)  	/** latin */
        {
            w[j++]=ch;
            w[j++]=0x00;
        }
        else if (ch == 0x80)  	/** euro sign */
        {
            w[j++]=0x20;
            w[j++]=0xac;
        }
        else if (ch > 0x80 && ch < 0xff)  	/** dbcs lead byte */
        {
            cl=src[++i];//next byte
            if(cl > 0x39 && cl < 0xff && cl != 0x7f)//valid byte check(0x40~0xfe),0x7f is undefined position
            {
                index = (ch - 0x81)*192 + (cl - 0x40);//compute offset in cp936_ucs_table
                if (index >= 0 && index < cp936_ucs_table_size)
                {
                    int uni = cp936_ucs_table[index];//unicode
                    w[j++]=(char)(uni & 0x00ff);
                    w[j++]=(char)(uni>>8);
                }
                else
                {
                    w[j++]=cl;
                    w[j++]=ch;
                }
            }
            else
            {
                w[j++]=cl;
                w[j++]=ch;
            }
        }
    }
    *outlen=j;
    return j;
}

/**
    处理UTF-16LE到UTF-8
    @result 转换的字节数量
*/
size_t conv_utf16le_utf8(const uint16_t *src, const size_t srclen,char *dest,size_t *outlen)
{
    size_t i,j=0;
    for(i=0; i<srclen; i++)
    {
        uint16_t ch=src[i];
        if(ch< 0x0080)
        {
            dest[ j ++]=(char)(0x7f & ch);
        }
        else if(ch >=0x0080 && ch<0x0800)
        {
            dest[ j ++]=(char)(0xc0 | ch>>6);
            dest[ j ++]=(char)(0x80 | (ch&0x3f));
        }
        else if(ch >=0x0800 && ch<0xFFFE && ch!=0xFEFF)//ignore BOM here 0xFFFE
        {
            dest[ j ++]=(char)(0xE0 | ch>>12);
            dest[ j ++]=(char)(0x80 | (ch&0XFC0)>>6);
            dest[ j ++]=(char)(0x80 | (ch&0X3F));
        }
    }
    *outlen=j;
    return i;
}

/**
    处理UTF-16BE(ucs2)到UTF-8
    @result 转换的字节数量
*/
size_t conv_utf16be_utf8(const uint16_t *src, const size_t srclen,char *dest,size_t *outlen)
{
    uint16_t swapedstr[srclen];
    for (size_t i = 0; i < srclen; i++)
    {
        swapedstr[i] = bswap16(src[i]);
    }
    src=swapedstr;
    return  conv_utf16le_utf8(src,srclen,dest,outlen);
}

/**
    处理UTF-16LE(ucs2)到UTF-8
    @result 转换的字节数量
*/
size_t conv_cp936_utf8(const uint8_t *src, const size_t srclen,char *dest,size_t *outlen)
{
    uint16_t tmp[srclen*2];
    size_t len;
    size_t complete_size= conv_cp936_ucs2((uint8_t *)src,srclen,(char *)tmp,&len);
    if(complete_size==0)
        return 0;
    return conv_utf16le_utf8(tmp,len/2,dest,outlen);
}

/**
 * utf-8 => utf16le
 @param *out unicode
 @result how many bytes used from @param utf when encoding the unicode
 */
inline size_t single_conv_utf8_ucs2(const uint8_t *utf,uint16_t *out)
{
    size_t ret;
    if ( (utf[0] >> 7) == 0 )//ascii
    {
        *out=(uint16_t)(utf[0] & 0x7F);
        ret=1;
    }
    else if ( (utf[0] & 0xE0) == 0xC0 )//110xxxxx
    {
        *out=(uint16_t)(((utf[1] & 0x1F)<<6) | (utf[1] & 0x3F));
        ret=2;
    }
    else if ( (utf[0] & 0xF0) == 0xE0 )//1110xxxx
    {
        *out=(uint16_t)(((utf[0] & 0x0F)<<12) | ((utf[1] & 0x3f)<<6) | (utf[2] & 0x3F));
        ret=3;
    }
    else/*unsupport*/
    {
        ret=0;
    }
    return ret;
}


/**
 * utf-16le => CP936（GBK）
 @result code of @param c in cp936
 */
inline uint16_t single_conv_ucs2_cp936(uint16_t c)
{
    uint16_t	s = 0;
    if (c >= ucs_a1_cp936_table_min && c < ucs_a1_cp936_table_max)
    {
        s = ucs_a1_cp936_table[c - ucs_a1_cp936_table_min];
    }
    else if (c >= ucs_a2_cp936_table_min && c < ucs_a2_cp936_table_max)
    {
        s = ucs_a2_cp936_table[c - ucs_a2_cp936_table_min];
    }
    else if (c >= ucs_a3_cp936_table_min && c < ucs_a3_cp936_table_max)
    {
        s = ucs_a3_cp936_table[c - ucs_a3_cp936_table_min];
    }
    else if (c >= ucs_i_cp936_table_min && c < ucs_i_cp936_table_max)
    {
        s = ucs_i_cp936_table[c - ucs_i_cp936_table_min];
    }
    else if (c >= ucs_ci_cp936_table_min && c < ucs_ci_cp936_table_max)
    {
        s = ucs_ci_cp936_table[c - ucs_ci_cp936_table_min];
    }
    else if (c >= ucs_cf_cp936_table_min && c < ucs_cf_cp936_table_max)
    {
        s = ucs_cf_cp936_table[c - ucs_cf_cp936_table_min];
    }
    else if (c >= ucs_sfv_cp936_table_min && c < ucs_sfv_cp936_table_max)
    {
        s = ucs_sfv_cp936_table[c - ucs_sfv_cp936_table_min];
    }
    else if (c >= ucs_hff_cp936_table_min && c < ucs_hff_cp936_table_max)
    {
        s = ucs_hff_cp936_table[c - ucs_hff_cp936_table_min];
    }
    return s;
}

/**
    处理UTF-16LE到GBK
    @result 转换的unicode双字节数量
*/
size_t conv_ucs2_cp936(const uint16_t *src, const size_t srclen,char *dest,size_t *outlen)
{
    size_t i,j=0;
    for(i=0; i<srclen; i++)
    {
        uint16_t val=single_conv_ucs2_cp936(src[i]);
        if(val<0x80)
        {
            if(val>0x00)//may be result from UTF BOM or other invalid code,discard
                dest[j++]=(char)val;
        }
        else
        {
            dest[j++]=val>>8;
            dest[j++]=val&0xff;
        }
    }
    *outlen=j;
    return i;
}

/**
    处理UTF-8到UTF16-LE
    @result 转换的utf8字节数量
*/
size_t conv_utf8_ucs2(const uint8_t *src, const size_t srclen,uint16_t *dest,size_t *outlen)
{
    size_t i=0,ucs2_idx=0;
    while(i<srclen)
    {
        i+=single_conv_utf8_ucs2(&src[ i ],&dest[ucs2_idx++]);
    }
    *outlen=ucs2_idx;
    return i;
}

/**
    处理UTF-8到GBK
    @result 转换的utf8字节数量
*/
size_t conv_utf8_cp936(const uint8_t *src, const size_t srclen,char *dest,size_t *outlen)
{
    uint16_t ucs2tmp[srclen];
    size_t usc2len;
    conv_utf8_ucs2(src,srclen,ucs2tmp,&usc2len);
    return  conv_ucs2_cp936(ucs2tmp,usc2len,dest,outlen);
}


size_t ucs2_getLetter(const uint16_t*ucs2,const size_t ucslen,char*dest,size_t *outlen)
{
    size_t i,j;
    for(i=0,j=0; i<ucslen; i++)
    {
        uint16_t c=ucs2[i];
        if(c<0x0080)
            dest[j++]=(char)ucs2[i];
        else if(c>=PINYIN_UCS_MIN && c<=PINYIN_UCS_MAX)
        {
            dest[j++]=pinyin_tab[c-0x4E00];
        }
    }
    *outlen=j;
    return i;
}

