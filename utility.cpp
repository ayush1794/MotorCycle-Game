#include "includes.h"

#define LEN 8192

void printv(va_list args, const char * format)
{
    char buf[LEN];
    char * ch=buf;
    vsnprintf(buf,LEN,format,args);
    while(*ch)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

void print(const char * format, ...)
{
    va_list args;
    va_start(args,format);
    printv(args,format);
    va_end(args);
}

void printAt(int x,int y, const char * format, ...)
{
    va_list args;
    glWindowPos2i(x,y);
    va_start(args,format);
    printv(args,format);
    va_end(args);
}

GLuint loadBMP_custom(const char * imagepath){
    // Data read from the header of the BMP file
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data  begins
    unsigned int width, height;
    unsigned int imageSize;   // = width*height*3
    // Actual RGB data
    unsigned char * data;

    FILE * file = fopen(imagepath,"rb");
    if (!file)                              
    {
        printf("Image could not be opened\n"); 
        return 0;
    }
    if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
        printf("Not a correct BMP file\n");
        return false;
    }
    if ( header[0]!='B' || header[1]!='M' ){
        printf("Not a correct BMP file\n");
        return 0;
    }
    //Read ints from byte array
    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    width      = *(int*)&(header[0x12]);
    height     = *(int*)&(header[0x16]);

    // Some BMP files are misformatted, guess missing information
    if (imageSize==0)    
        imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos==0)      
        dataPos=54; // The BMP header is done that way
    data = new unsigned char [imageSize];
    fread(data,1,imageSize,file);
    fclose(file);

    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
     
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return textureID;
}
