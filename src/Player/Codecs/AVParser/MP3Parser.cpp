//读取Mp3文件头部和尾部的信息
//MP3的文件头长10Byte,主要保存ID3,ID3版本和副版本信息,以及Flag和帧长的信息
//尾部的128字节保存了Header(尾部头,3Byte),Title(歌曲名称,30Byte),Artilst(艺术家,30Byte),Album(专辑,30Byte),Year(发行年份,4Byte),Comment(评论,30Byte),Cenre(?,1Byte)
//3+30+30+30+4+30+1=128

#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include "MP3Parser.h"

FILE *fp;

//Tag_header_Start
struct Tag_header{
 char Header[3]; //it must be ID3
 char ver;            //the version of ID3
 char ReVer;      //the subversion of ID3
 char Flag;
 char Size[4];
};
//Tag_header_End

//Tag_Trailer_Start
struct Tag_Trailer{
 char Header[3];
 char Title[30];
 char Artist[30];
 char Album[30];
 char Year[4];
 char Comment[30];
 char Cenre[1];
};
//Tag_Trailer_End


//定义结构体变量,文件头和文件尾
struct Tag_Header Mp3Header;
struct Tag_Trailer Mp3Trailer;


int main(int argc,char **argv)
{
    fp=fopen("test.mp3","r+");
    if(fp==NULL){
        printf("Can't open file\n");
    }else{
        Tag_GetHeader();
        Tag_GetTrailer();
    }

    printf("\n%s\n",Mp3Trailer.Title);
    printf("%s\n",Mp3Trailer.Artist);
    printf("%s\n",Mp3Trailer.Album);
    printf("%s\n",Mp3Trailer.Year);
    printf("%s\n",Mp3Trailer.Comment);
    fclose(fp);
    
    return 0;
}

void Tag_GetTrailer(void){
    //get header
    fseek(fp,-128L,SEEK_END);
    fread(Mp3Trailer.Header,3,1,fp);

    //get title
    fseek(fp,-125L,SEEK_END);
    fread(Mp3Trailer.Title,30,1,fp);

    //get artist
    fseek(fp,-95L,SEEK_END);
    fread(Mp3Trailer.Artist,30,1,fp);

    //get album
    fseek(fp,-65L,SEEK_END);
    fread(Mp3Trailer.Album,30,1,fp);

    //get year
    fseek(fp,-35L,SEEK_END);
    fread(Mp3Trailer.Year,4,1,fp);

    //get comment
    fseek(fp,-31L,SEEK_END);
    fread(Mp3Trailer.Comment,30,1,fp);

    //get Cenre
    fseek(fp,-1L,SEEK_END);
    fread(Mp3Trailer.Cenre,1,1,fp);
    
    return ;
}

void Tag_GetHeader(void){
    //get ID3 Tag
    fseek(fp,0,SEEK_SET);
    fread(Mp3Header.Header,sizeof(Mp3Header.Header),1,fp);

    //get ID3 version
    fseek(fp,3,SEEK_CUR);
    fread(Mp3Header.ver,sizeof(Mp3Header.ver),1,fp);

    //get ID3 Reversion
    fseek(fp,4,SEEK_CUR);
    fread(Mp3Header.Rever,sizeof(Mp3Header.Rever),1,fp);

    //get ID3 flag
    fseek(fp,5,SEEK_CUR);
    fread(Mp3Header.Flag,sizeof(Mp3Header.Flag),1,fp);

    //get ID3 size
    fseek(fp,6,SEEK_CUR);
    fread(Mp3Header.Size,sizeof(Mp3Header.Size),1,fp);
    
    return ;
}