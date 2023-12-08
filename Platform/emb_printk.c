/**********************************************************************************************************/
/** @file     emb_printk.c
*** @author   Linghu
*** @version  V1.0.0
*** @date     2018/6/21
*** @brief    The standard C function printf, renamed to printk because of some incompatibilities.
***********************************************************************************************************
*** @par Last Commit:
***      \$Author$ \n
***      \$Date$ \n
***      \$Rev$ \n
***      \$URL$ \n
***
*** @par Change Logs:
***      2018/6/21 -- Linghu -- the first version
***********************************************************************************************************/
//lint -emacro((835), ZEROPAD)           " A zero has been given as right argument to operator '<<' "
//lint -emacro((740), va_start, va_arg)  " Unusual pointer cast (incompatible indirect types) "
//lint ++d"va_start(ap, parmN) = ( *((char**)&(ap)) = ((char*)&(parmN) + sizeof(parmN)) )"
//lint ++d"va_arg(ap, type)    = ( *((*((type**)&(ap)))++) )"
//lint ++d"va_end(ap)          = ( (void)(ap) )"

#include "ctype.h"
#include "string.h"
#include "stdint.h"

#include "emb_printk.h"

/**********************************************************************************************************/
/** @addtogroup PrintK
*** @{
*** @addtogroup PrintK_Pravate
*** @{
*** @addtogroup                 PrintK_Private_Config
*** @{
***********************************************************************************************************/

/* Config char output function of printk, Can be re-define for macro KERNEL_PUT_CHAR */
#ifndef KERNEL_PUT_CHAR
#define KERNEL_PUT_CHAR     _ttywrch
#endif

//lint -esym(526, _ttywrch)
extern void _ttywrch( int ch );


/**********************************************************************************************************/
/** @}
*** @addtogroup                 PrintK_Private_Constants
*** @{
***********************************************************************************************************/

#define ZEROPAD     (1 << 0)    /* pad with zero                    */
#define SIGN        (1 << 1)    /* unsigned/signed long             */
#define PLUS        (1 << 2)    /* show plus                        */
#define SPACE       (1 << 3)    /* space if plus                    */
#define LEFT        (1 << 4)    /* left justified                   */
#define SPECIAL     (1 << 5)    /* 0x                               */
#define LARGE       (1 << 6)    /* use 'ABCDEF' instead of 'abcdef' */

/**********************************************************************************************************/
/** @}
*** @addtogroup                 PrintK_Private_Functions
*** @{
***********************************************************************************************************/

static int32_t divide( int32_t *n, int32_t base )
{
    int32_t     res;

    /* optimized for processor which does not support divide instructions. */
    if( base == 8 ) {
        res = (int32_t)(((uint32_t)*n) % 8U);
        *n  = (int32_t)(((uint32_t)*n) / 8U);
    }
    else if( base == 16 ) {
        res = (int32_t)(((uint32_t)*n) % 16U);
        *n  = (int32_t)(((uint32_t)*n) / 16U);
    }
    else {
        res = (int32_t)(((uint32_t)*n) % 10U);
        *n  = (int32_t)(((uint32_t)*n) / 10U);
    }

    return res;
}


static int skip_atoi( const char **s )
{
    register int    i = 0;
    while( isdigit(**s) )
    {
        i = i * 10 + *((*s)++) - '0';
    }
    return( i );
}


/**********************************************************************************************************/
/** @brief      Prints a number.
***
*** @param[in]  num         Number to print.
*** @param[in]  base        Units of rates.
*** @param[in]  width       Minimum number of characters to be printed.
*** @param[in]  precision   Minimum number of digits to be printed.
*** @param[in]  type        Options for Align, Pad or Sign.
***
*** @return     The total number of put characters.
***********************************************************************************************************/

static int print_number( int32_t num, int base, int width, int precision, int type )
{
    static const char   small_digits[] = "0123456789abcdef";
    static const char   large_digits[] = "0123456789ABCDEF";
    int                 nbr = 0;
    char                sign;
    char                tmp[12];
    const char         *digits;
    int                 len;

    if( type & LEFT ) {
        type &= ~ZEROPAD;
    }
    if( type & SPECIAL ) {
        if(/**/ base == 16)  width -= 2;
        else if(base ==  8)  width -= 1;
    }

    if( sign = 0, type & SIGN ) {               /* Get sign                             */
        if(num < 0) { num = -num; sign = '-'; }
        else if(type & PLUS)   {  sign = '+'; }
        else if(type & SPACE)  {  sign = ' '; }
    }
    for(len = 0, digits = (type & LARGE) ? large_digits : small_digits;  ;)
    {                                           /* Number to string                     */
        tmp[len++] = digits[divide(&num, base)];
        if(num == 0)  break;
    }
    if( precision < len) {
        precision = len;
    }
    width -= precision;

    if( !(type & LEFT) && !(type & ZEROPAD) )   /* Put pad used ' '                     */
    {
        if((width > 0) && (sign)) {
            width--;
        }
        for(;  width > 0;  width--) {
            KERNEL_PUT_CHAR(' ');    nbr++;
        }
    }
    if( sign ) {                                /* Put sign                             */
        KERNEL_PUT_CHAR(sign);    nbr++;
        width--;
    }
    if( type & SPECIAL )                        /* Put 0 or 0x or 0X                    */
    {
        if( base == 8 ) {
            KERNEL_PUT_CHAR('0');    nbr++;
        }
        else if( base == 16 ) {
            KERNEL_PUT_CHAR('0');    nbr++;
            KERNEL_PUT_CHAR(type & LARGE ? 'X' : 'x');    nbr++;
        }
    }
    if( !(type & LEFT) && (type & ZEROPAD) )    /* Put pad used '0'                     */
    {
        for(;  width > 0;  width--) {
            KERNEL_PUT_CHAR('0');    nbr++;
        }
    }
    for(;  len < precision;  precision--) {     /* Minimum number of digits             */
        KERNEL_PUT_CHAR('0');    nbr++;
    }
    for(;  len-- > 0;) {                        /* Put number in the temporary buffer   */
        KERNEL_PUT_CHAR(tmp[len]);    nbr++;
    }
    for(;  width > 0;  width--) {
        KERNEL_PUT_CHAR(' ');    nbr++;
    }

    return( nbr );
}


/**********************************************************************************************************/
/** @brief      Prints messages for the OS Kernel and Interrupt.
***
*** @param[in]  fmt     String for format.
*** @param[in]  args    Parameter group.
***
*** @return     (>= 0)The total number of put characters, (< 0)A error occurs.
***********************************************************************************************************/

int vprintk( const char *fmt, va_list args )
{
    uint8_t     base;       /* the base of number                               */
    uint8_t     flags;      /* flags to print number                            */
    uint8_t     qualifier;  /* 'h', 'l', or 'L' for integer fields              */
    int32_t     width;      /* width of output field                            */
    int         precision;  /* min. # of digits for integers and max for string */
    int         nbr;        /* the total number of put characters               */
    uint32_t    num;
    int         i, len;
    const char *str;

    if( !fmt ) {
        return( 0 );
    }

    for(nbr = 0;  *fmt;  ++fmt) /*lint !e443*/
    {
        if( *fmt != '%' ) {
            KERNEL_PUT_CHAR(*fmt);    nbr++;
            continue;
        }

        for(flags = 0; ;)                       /* Process flags                */
        {
            ++fmt;
            if( /**/ *fmt == '-' )  flags |= LEFT;
            else if( *fmt == '+' )  flags |= PLUS;
            else if( *fmt == ' ' )  flags |= SPACE;
            else if( *fmt == '#' )  flags |= SPECIAL;
            else if( *fmt == '0' )  flags |= ZEROPAD;
            else break;
        }

        if( width = -1, isdigit(*fmt) ) {       /* Get field width              */
            width = skip_atoi(&fmt);
        }
        else if( *fmt == '*' )
        {
            ++fmt;
            width = va_arg(args, int);
            if( width < 0 ) {
                width  = -width;
                flags |=  LEFT;
            }
        }

        if( precision = -1, *fmt == '.' )       /* Get the precision            */
        {
            ++fmt;
            if( isdigit(*fmt) ) {
                precision = skip_atoi(&fmt);
            }
            else if( *fmt == '*' ) {
                ++fmt;
                precision = va_arg(args, int);
            }
            if( precision < 0 ) {
                precision = 0;
            }
        }

        if( qualifier = 0, (*fmt == 'h') || (*fmt == 'l') ) {
            qualifier = (uint8_t)*fmt;          /* Get the conversion qualifier */
            ++fmt;
        }

        switch( base = 10, *fmt )
        {
        case 'c':
            if( !(flags & LEFT) ) {
                for(width--;  width > 0;  width--) {
                    KERNEL_PUT_CHAR(' ');    nbr++;         /* Put width        */
                }
            }
            KERNEL_PUT_CHAR(va_arg(args, int));    nbr++;
            for(width--;  width > 0;  width--) {
                KERNEL_PUT_CHAR(' ');    nbr++;             /* Put width        */
            }
            continue;

        case 's':
            if( (str = va_arg(args, char*)) == NULL ) {
                str = "(NULL)";
            }
            if( ((len = (int)strlen(str)) > precision) && (precision > 0) ) {
                len = precision;
            }
            if( !(flags & LEFT) ) {
                for(;  len < width;  width--) {
                    KERNEL_PUT_CHAR(' ');    nbr++;         /* Put width        */
                }
            }
            for( i = 0;  i < len;  ++str, ++i ) {
                KERNEL_PUT_CHAR(*str);    nbr++;            /* Put string       */
            }
            for(;  len < width;  width--) {
                KERNEL_PUT_CHAR(' ');    nbr++;             /* Put width        */
            }
            continue;

        case 'p':
            if( width == -1 ) {
                width  = sizeof(void *) << 1;
                flags |= ZEROPAD;
            }
            nbr += print_number((int32_t)va_arg(args, void *), 16, width, precision, flags);
            continue;

        case '%':
            KERNEL_PUT_CHAR('%');    nbr++;                 /* Put character    */
            continue;

            /* integer number formats - set up the flags and "break" */
        case 'o':
            base = 8;
            break;

        case 'X':
            flags |= LARGE;
        case 'x':   /*lint !e616 !e825*/
            base = 16;
            break;

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':   /*lint !e616 !e825*/
            break;

        default:
            KERNEL_PUT_CHAR('%');    nbr++;
            if( *fmt ) {
                KERNEL_PUT_CHAR(*fmt);    nbr++;
            } else {
                --fmt;
            }
            continue;
        }

        if( qualifier == 'l' ) {
            num = va_arg(args, uint32_t);
          //if(flags & SIGN)  num = (int32_t)num;
        }
        else if( qualifier == 'h' ) {
            num = (uint32_t)va_arg(args, int32_t);
            if(flags & SIGN)  num = (int16_t)(num & 0xFFFF);
        }
        else {
            num = va_arg(args, uint32_t);
          //if(flags & SIGN)  num = (int32_t)num;
        }
        nbr += print_number((int32_t)num, base, width, precision, flags);
    }

    return( nbr );  //lint !e850
}


/**********************************************************************************************************/
/** @brief      Prints messages from the OS Kernel and Interrupt.
***
*** @param[in]  fmt     String for format.
***
*** @return     (>= 0)The total number of put characters, (< 0)A error occurs.
***********************************************************************************************************/

int printk( const char *fmt, ... )
{
    va_list     args;
    int         res;

    va_start(args, fmt);
    res = vprintk(fmt, args);
    va_end(args);

    return( res );
}


/*****************************  END OF FILE  **************************************************************/
/** @}
*** @}
*** @}
***********************************************************************************************************/

