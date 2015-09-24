

/* GIFSCAN - scans through a GIF file and reports all parameters */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void    colormap(int out, FILE *dev, int times);
unsigned int getbytes(FILE *dev);
void    extninfo(FILE *dev);
void    chkunexp(int *unexpected, int determiner);
void    imageinf(FILE *dev, int bits_to_use, int color_style);
#define MAX 255
/************************************************************************/
/* MAIN - the main routine reads the GIF file for the global GIF        */
/*        information which it displays to the user's screen.  This     */
/*        routine also determines which subroutine to call and which    */
/*        information to process.                                       */
#define DEC "-d"
#define HEX "-h"
#define PCT "-p"
#define DECIMAL 1
#define PERCENT 2
#define HEXIDEC 3
int gifscan(int argc, char *argv[])
{
    char filename[15];
    char version[7];
    char style[5];
    int byte1;
    int byte2;
    int byte3;
    int color_res;
    int unexpected;
    int image_cnt;
    int bits_per_pix;
    int bits_to_use;
    int colors;
    int i;
    int globl;
    int end_gif_fnd;
    int color_style;
    int switch_present;
    unsigned int width;
    unsigned int height;
    FILE *in;
    /* Start of Processing */
    /* If just one argument then display the message */
    if (argc == 1) {
        printf("nUSAGE:    gifscan  color_opt  filenamen");
        printf("ncolor_opt   specifies how color lookup table");
        printf("n            definitions are displayed");
        printf("n  -h  :     output as hexidecimal (0 - FF) (default)");
        printf("n  -d  :     output as decimal (0 - 255)");
        printf("n  -p  :     output as percentage (0 - 100)n");
        return -1;
    }
    color_style = 0;
    switch_present = 0;
    if ((strnicmp(argv[1], "-", 1) == 0)) {
        switch_present = 1;
        strcpy(style, argv[1]);
    } else
        switch_present = 0;
    do {
        if ((strnicmp(style, DEC, 2) == 0))
            color_style = DECIMAL;
        else if ((strnicmp(style, PCT, 2) == 0))
            color_style = PERCENT;
        else
            color_style = HEXIDEC;
#if 0
        else if ((strnicmp(style, HEX, 2) == 0))
            color_style = HEXIDEC;
        else {
            printf("nEnter Colormap style (-h, -p, -d) : ");
            gets (style);
        }
#endif
    } while (color_style == 0)
        ;
    /* Check for GIF filename */
    do {
        if ((argc == 1) || ((argc == 2) && (switch_present == 1))) {
            printf("nEnter GIF filename:  ");
            gets (filename);
        } else if (argc == 3)
            strcpy(filename, argv[2]);
        else
            strcpy(filename, argv[1]);
        in = fopen (filename, "rb");
        argc = 1;
    } while (in == NULL)
        ;
    image_cnt = 0;
    end_gif_fnd = 0;
    unexpected = 0;
    /* get version from file */
    if ((version[0] = getc(in)) == 0x47) {
        for (i = 1; (i < 6); i++)
            version[i] = getc(in);
        version[6] = '';
        printf("nVersion: %s", version);
    } else {
        printf("n? -- NOT a GIF filen");
        exit(1);
    }
    /* determine screen width */
    width = getbytes(in);
    /* determine screen height */
    height = getbytes(in);
    printf("nScreen Width:     %5dtScreen Height:    %5d", width, height);
    /* check for a Global Map */
    byte1 = getc(in);
    byte2 = byte1 & 0x80;
    if (byte2 == 0x80) {
        printf("nGlobal Color Map: Yes");
        globl = 1;
    } else {
        printf("nGlobal Color Map: No");
        globl = 0;
    }
    /* Check for the 0 bit */
    byte2 = byte1 & 0x08;
    if (byte2 != 0)
        printf("n? -- Reserved zero bit is not zero.n");
    /* determine the color resolution */
    byte2 = byte1 & 0x70;
    color_res = byte2 >> 4;
    /* get the background index */
    byte3 = getc(in);
    printf("nColor res:  %5dtBackground index:  %5d", ++color_res, byte3);
    /* determine the bits per pixel */
    bits_per_pix = byte1 & 0x07;
    bits_per_pix++;
    bits_to_use = bits_per_pix;
    /* determine # of colors in global map */
    colors = 1 << bits_per_pix;
    printf("nBits per pixel: %5dt# colors:  %5dn", bits_per_pix, colors);
    /* check for the 0 byte */
    byte1 = getc(in);
    if (byte1 != 0)
        printf("n? -- Reserved byte after Background index is not zero.n");
    if (globl == 1)
        colormap (color_style, in, colors);
    /* check for the zero byte count, a new image, or */
    /* the end marker for the gif file */
    while ((byte1 = getc(in)) != EOF) {
        if (byte1 == ',') {
            image_cnt++;
            if (unexpected != 0)
                chkunexp(&unexpected, image_cnt);
            printf("nImage # %2d", image_cnt);
            imageinf(in, bits_to_use, color_style);
        } else if (byte1 == '!')
            extninfo (in);
        else if (byte1 == ';') {
            if (unexpected != 0)
                chkunexp(&unexpected, -1);
            end_gif_fnd = 1;
        } else
            unexpected++;
    }
    /* EOF has been reached - check last bytes read */
    if (end_gif_fnd == 0)
        printf("n? -- GIF file terminator ';' was not found.n");
    else if (unexpected != 0)
        chkunexp(&unexpected, -2);
    
    return 0;
}
/************************************************************************/
/* COLORMAP - reads color information in from the GIF file and displays */
/*            it in a user selected method.  This display may be in :   */
/*            hexidecimal (default), percentage, or decimal.  User      */
/*            selects output method by placing a switch (-d, -p, -h)    */
/*            between the program name and GIF filename at request time.*/
void
colormap(int out, FILE *dev, int times)
{
    unsigned int red;
    unsigned int green;
    unsigned int blue;
    int print_cnt;
    int i;
    /* Start of procedure */
    if (out == DECIMAL)
        printf("nColor definitions in decimal   (index #, R, G, B)n");
    if (out == PERCENT)
        printf("nColor definitions by percentage   (index #, R, G, B)n");
    if (out == HEXIDEC)
        printf("nColor definitions in hexidecimal    (index #, R, G, B)n");
    /* read and print the color definitions */
    print_cnt = 0;
    for (i = 0; (i < times); i++) {
        red = getc(dev);
        green = getc(dev);
        blue = getc(dev);
        switch (out) {
        case DECIMAL :
            printf("%3d - %3d %3d %3d  ", i, red, green, blue);
            break;
        case PERCENT :
            red = (red * 100) / MAX;
            green = (green * 100) / MAX;
            blue = (blue * 100) / MAX;
            printf("%3d - %3d %3d %3d  ", i, red, green, blue);
            break;
        case HEXIDEC :
            printf("%3d - %2x %2x %2x   ", i, red, green, blue);
            break;
        }
        print_cnt++;
        if (print_cnt == 4) {
            printf("n");
            print_cnt = 0;
        }
    }
    if ((times % 4) != 0)
        printf("n");
    
    return ;
}
/************************************************************************/
/* GETBYTES - routine to retrieve two bytes of information from the GIF */
/*            file and then shift them into correct byte order.  The    */
/*            information is stored in Least Significant Byte order.    */
unsigned int getbytes(FILE *dev)
{
    int byte1;
    int byte2;
    int result;
    /* read bytes and shift over */
    byte1 = getc(dev);
    byte2 = getc(dev);
    result = (byte2 << 8) | byte1;
    return result;
}
/***********************************************************************/
/* IMAGEINF - routine to read the GIF image information and display it */
/*            to the user's screen in an orderly fasion.  If there are */
/*            multiple images then IMAGEINF will be called to display  */
/*            multiple screens.                                        */
void
imageinf(FILE *dev, int bits_to_use, int color_style)
{
    int byte1;
    int byte2;
    int image_left;
    int image_top;
    int data_byte_cnt;
    int bits_per_pix;
    int colors;
    int i;
    int local;
    unsigned int width;
    unsigned int height;
    unsigned long bytetot;
    unsigned long possbytes;
    /* determine the image left value */
    image_left = getbytes(dev);
    /* determine the image top value */
    image_top = getbytes(dev);
    printf("nImage Left: %5dttImage Top: %5d", image_left, image_top);
    /* determine the image width */
    width = getbytes(dev);
    /* determine the image height */
    height = getbytes(dev);
    printf("nImage Width:    %5dttImage Height:  %5d", width, height);
    /* check for interlaced image */
    byte1 = getc(dev);
    byte2 = byte1 & 0x40;
    if (byte2 == 0x40)
        printf("nInterlaced: Yes");
    else
        printf("nInterlaced: No");
    /* check for a local map */
    byte2 = byte1 & 0x80;
    if (byte2 == 0x80) {
        local = 1;
        printf("nLocal Color Map: Yes");
    } else {
        local = 0;
        printf("nLocal Color Map: No");
    }
    /* check for the 3 zero bits */
    byte2 = byte1 & 0x38;
    if (byte2 != 0)
        printf("n? -- Reserved zero bits in image not zeros.n");
    /* determine the # of color bits in local map */
    bits_per_pix = byte1 & 0x07;
    bits_per_pix++;
    colors = 1 << bits_per_pix;
    if (local == 1) {
        bits_to_use = bits_per_pix;
        printf("nBits per pixel: %5dtt# colors :     %5d", bits_per_pix,
               colors);
        colormap (color_style, dev, colors);
    }
    /* retrieve the code size */
    byte1 = getc(dev);
    if ((byte1 < 2) || (byte1 > 8)) {
        printf("n? -- Code size %d at start of image");
        printf("n     is out of range (2-8).n");
    } else
        printf("nLZW min code size (bits):  %3d", byte1);
    /* tally up the total bytes and read past each data block */
    bytetot = 0;
    possbytes = 0;
    while ((data_byte_cnt = getc(dev)) > 0) {
        bytetot = bytetot + data_byte_cnt;
        for (i = 0; (i < data_byte_cnt); i++) {
            byte2 = getc(dev);
            if (byte2 == EOF) {
                printf("n? -- EOF reached inside image data block.n");
                exit (2);
            }
        }
    }
    possbytes = (unsigned long) width * height;
    i = 8 / bits_to_use;
    possbytes = possbytes / i;
    printf("nTotal bytes:  %ld out of possible  %ldn", bytetot, possbytes);
    if (data_byte_cnt == EOF) {
        printf("n? -- EOF reached before zero byte count");
        printf("n     of image was read.n");
        exit (3);
    }
    
    return ;
}
/************************************************************************/
/* EXTNINFO - routine to read the GIF file for extension data and       */
/*            display it to the screen in an orderly fasion.  This      */
/*            extension information may be located before, between, or  */
/*            after any of the image data.                              */
void
extninfo(FILE *dev)
{
    int byte1;
    int byte2;
    int i;
    int data_byte_cnt;
    unsigned long bytetot;
    /* retrieve the function code */
    byte1 = getc(dev);
    printf("nGIF extension seen, code :  %d", byte1);
    /* tally up the total bytes and read past each data block */
    bytetot = 0;
    while ((data_byte_cnt = getc(dev)) > 0) {
        bytetot = bytetot + data_byte_cnt;
        for (i = 0; (i < data_byte_cnt); i++) {
            byte2 = getc(dev);
            if (byte2 == EOF) {
                printf("n? -- EOF reached inside extension data block.n");
                exit (2);
            }
        }
    }
    printf("nTotal number of bytes in extension:  %ldn", bytetot);
    if (data_byte_cnt == EOF) {
        printf("n? -- EOF was reached before zero byte count");
        printf("n     of extension was read.n");
        exit (3);
    }
    
    return ;
}
/************************************************************************/
/* CHKUNEXP - routine to check for any unexpected nonzero data found    */
/*            within the GIF file.  This routine will help determine    */
/*            where the unexpected data may reside in the file.         */
void
chkunexp (int *unexpected, int determiner)
{
    /* Determine place in the GIF file */
    if (determiner > 0) {
        printf("n? -- %d bytes of unexpected data found before", *unexpected);
        printf("n     image %d.n", determiner);
    } else if (determiner == -1) {
        printf("n? -- %d bytes of unexpected data found before",
               *unexpected);
        printf("n     GIF file terminator.n");
    } else {
        printf("n? -- %d bytes of unexpected data found after",
               *unexpected);
        printf("n     GIF file terminator.n");
    }
    /* Zero out unexpected variable for next group that may be encountered */
    *unexpected = 0;
    
    return ;
}

/**
--
jdm@hodge.cts.com [uunet zardoz vdelta crash]!hodge!jdm
James D. Murray, Ethnounixologist
Hodge Computer Research Corporation
1588 North Batavia Street
Orange, California 92667  USA
TEL: (714) 998-7750 Ask for James
FAX: (714) 921-8038 Wait for the carrier
**/

