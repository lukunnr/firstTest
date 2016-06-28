#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "api_webpage.h"


/**
* @param s ��Ҫ�����url�ַ���
* @param len ��Ҫ�����url�ĳ���
* @param new_length ������url�ĳ���
* @return char * ���ر�����url
* @note �洢������url�洢��һ����������ڴ��У�
* ����󣬵�����Ӧ���ͷ���
*/
char * Utf8EncodeToUrl(char const *s, int len, int *new_length)
{
    fprintf(stderr, "string len:%d src:{%s}\n",strlen(s), s);

    unsigned char const *from;
    unsigned char const *end;
    unsigned char* start;
    unsigned char* to;
    char c;
    
    from = s;
    end = s + len;
    start = to = (unsigned char *) malloc(3 * len + 1);

    unsigned char hexchars[] = "0123456789ABCDEF";

    while (from < end) {
        c = *from++;

        if (c == ' ') {
            *to++ = '+';
        } else if ((c < '0' && c != '-' && c != '.')
                   ||(c < 'A' && c > '9')
                   ||(c > 'Z' && c < 'a' && c != '_')
                   ||(c > 'z')) {
            to[0] = '%';
            to[1] = hexchars[c >> 4];
            to[2] = hexchars[c & 15];
            to += 3;
        } else {
            *to++ = c;
        }
    }
    
    *to = 0;
    
    if (new_length) {
        *new_length = to - start;
    }

    fprintf(stderr, "string len:%d des:{%s}\n",strlen((char *)start) ,(char *) start);
    
    return (char *) start;
}



/**
* @param str ��Ҫ�����url�ַ���
* @param len ��Ҫ�����url�ĳ���
* @param buffer ���ؽ������ַ���
* @return int ���ؽ�����url����
*/
int UrlDecodeToUtf8(char *str, int len, char* buffer)
{
    char *dest = str;
    char *data = str;

    int value;
    int c;

    fprintf(stderr, "url len:%d src:{%s}\n", strlen(str), str);

    while (len--) {
        fprintf(stderr, "1 len:%d data:{%d}\n", len, *data);
        if (*data == '+') {
        *dest = ' ';
        }
        else if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1))
                 && isxdigit((int) *(data + 2)))
        {
            fprintf(stderr, "2 len:%d data1:{%d}\n", len, *(data+1));
            c = ((unsigned char *)(data+1))[0];
            if (isupper(c))
            {
                c = tolower(c);
            }

            fprintf(stderr, "2 len:%d data2:{%d}\n", len, *(data+2));
            value = ((c >= '0' && c <= '9') ?( c - '0') : (c - 'a' + 10)) * 16;
            c = ((unsigned char *)(data+1))[1];
            if (isupper(c))
            {
                c = tolower(c);
            }
            
            value += (c >= '0' && c <= '9') ? (c - '0') : (c - 'a' + 10);

            *dest = (char)value ;
            data += 2;
            len -= 2;
        } 
        else {
            fprintf(stderr, "3 len:%d data:{%d}\n", len, *data);
            *dest = *data;
        }
        data++;
        dest++;
    }
    
    *dest = '\0';

    fprintf(stderr, "url len:%d des:{%s}, return len:%d\n",strlen(str) ,str ,(dest - str));

    memset(buffer, 0, URL_LEN_MAX);
    memcpy(buffer, str, strlen(str));
    fprintf(stderr, "url len:%d buffer:{%s}\n",strlen(buffer) ,buffer);
    
    return (dest - str);
}


