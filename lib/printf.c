
#include "printf.h"
#include <defs.h>


#define MAXBUF (sizeof(long int) * 8)   /* enough for binary */

static char *printnum(char *b, unsigned int u, int base,
                      bool negflag, int length, bool ladjust,
                      char padc, bool upcase)
{
    char            buf[MAXBUF];        /* build number here */
    char           *p = &buf[MAXBUF - 1];
    int             size;
    char           *digs;
    static char     up_digs[] = "0123456789ABCDEF";
    static char     low_digs[] = "0123456789abcdef";

    digs = upcase ? up_digs : low_digs;
    do {
        *p-- = digs[u % base];
        u /= base;
    } while (u != 0);

    if (negflag)
        *b++ = '-';

    size = &buf[MAXBUF - 1] - p;

    if (size < length && !ladjust) {
        while (length > size) {
            *b++ = padc;
            length--;
        }
    }

    while (++p != &buf[MAXBUF])
        *b++ = *p;

    if (size < length) {
        /* must be ladjust */
        while (length > size) {
            *b++ = padc;
            length--;
        }
    }
    return b;
}


/* 
 *  This version implements therefore following printf features:
 *
 *      %d      decimal conversion
 *      %u      unsigned conversion
 *      %x      hexadecimal conversion
 *      %X      hexadecimal conversion with capital letters
 *      %o      octal conversion
 *      %c      character
 *      %s      string
 *      %m.n    field width, precision
 *      %-m.n   left adjustment
 *      %0m.n   zero-padding
 *      %*.*    width and precision taken from arguments
 *
 *  This version does not implement %f, %e, or %g.  It accepts, but
 *  ignores, an `l' as in %ld, %lo, %lx, and %lu, and therefore will not
 *  work correctly on machines for which sizeof(long) != sizeof(int).
 *  It does not even parse %D, %O, or %U; you should be using %ld, %o and
 *  %lu if you mean long conversion.
 *
 *  This version implements the following nonstandard features:
 *
 *      %b      binary conversion
 *
 */


#define isdigit(d) ((d) >= '0' && (d) <= '9')
#define ctod(c) ((c) - '0')


int __vsprintf(char *buf, const char *fmt, va_list argp)
{
    char           *p;
    char           *p2;
    int             length;
    int             prec;
    int             ladjust;
    char            padc;
    int             n;
    unsigned int    u;
    int             negflag;
    char            c;
    char           *start_buf = buf;

    while (*fmt != '\0') {
        if (*fmt != '%') {
            *buf++ = *fmt++;
            continue;
        }
        fmt++;
        if (*fmt == 'l')
            fmt++;              /* need to use it if sizeof(int) <
                                 * sizeof(long) */

        length = 0;
        prec = -1;
        ladjust = false;
        padc = ' ';

        if (*fmt == '-') {
            ladjust = true;
            fmt++;
        }

        if (*fmt == '0') {
            padc = '0';
            fmt++;
        }

        if (isdigit(*fmt)) {
            while (isdigit(*fmt))
                length = 10 * length + ctod(*fmt++);
        } else if (*fmt == '*') {
            length = va_arg(argp, int);
            fmt++;
            if (length < 0) {
                ladjust = !ladjust;
                length = -length;
            }
        }

        if (*fmt == '.') {
            fmt++;
            if (isdigit(*fmt)) {
                prec = 0;
                while (isdigit(*fmt))
                    prec = 10 * prec + ctod(*fmt++);
            } else if (*fmt == '*') {
                prec = va_arg(argp, int);
                fmt++;
            }
        }

        negflag = false;

        switch (*fmt) {
        case 'b':
        case 'B':
            u = va_arg(argp, unsigned int);
            buf = printnum(buf, u, 2, false, length, ladjust, padc, 0);
            break;

        case 'c':
            c = va_arg(argp, int);
            *buf++ = c;
            break;

        case 'd':
        case 'D':
            n = va_arg(argp, int);
            if (n >= 0)
                u = n;
            else {
                u = -n;
                negflag = true;
            }
            buf = printnum(buf, u, 10, negflag, length, ladjust, padc, 0);
            break;

        case 'o':
        case 'O':
            u = va_arg(argp, unsigned int);
            buf = printnum(buf, u, 8, false, length, ladjust, padc, 0);
            break;

        case 's':
            p = va_arg(argp, char *);
            if (p == (char *) 0)
                p = "(NULL)";
            if (length > 0 && !ladjust) {
                n = 0;
                p2 = p;
                for (; *p != '\0' && (prec == -1 || n < prec); p++)
                    n++;
                p = p2;
                while (n < length) {
                    *buf++ = ' ';
                    n++;
                }
            }
            n = 0;
            while (*p != '\0') {
                if (++n > prec && prec != -1)
                    break;
                *buf++ = *p++;
            }
            if (n < length && ladjust) {
                while (n < length) {
                    *buf++ = ' ';
                    n++;
                }
            }
            break;

        case 'u':
        case 'U':
            u = va_arg(argp, unsigned int);
            buf = printnum(buf, u, 10, false, length, ladjust, padc, 0);
            break;

        case 'x':
            u = va_arg(argp, unsigned int);
            buf = printnum(buf, u, 16, false, length, ladjust, padc, 0);
            break;

        case 'X':
            u = va_arg(argp, unsigned int);
            buf = printnum(buf, u, 16, false, length, ladjust, padc, 1);
            break;

        case '\0':
            fmt--;
            break;

        default:
            *buf++ = *fmt;
        }
        fmt++;
    }
    *buf = '\0';
    return buf - start_buf;
}

