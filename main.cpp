#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <cstring>

#define RED 0
#define GREEN 1
#define BLUE 2

using namespace std;
int bmpwidth,bmpheight,linebyte;
int colornum;
unsigned char ***newBmpBuf;
unsigned char ***pBmpBuf;  //存储图像数据
unsigned char *pColorBuf;  //存储图像数据
BITMAPFILEHEADER fileheader;
BITMAPINFOHEADER infoHead;
bool readBmp(char *bmpName) {

    FILE *fp;
    if( (fp = fopen(bmpName,"rb")) == NULL){
        cout<<"The file "<<bmpName<<"was not opened"<<endl;
        return FALSE;
    }
    if(!fread(&fileheader,sizeof(BITMAPFILEHEADER),1,fp)){
        cout<<"跳转失败"<<endl;
        return FALSE;
    }

    fread(&infoHead,sizeof(BITMAPINFOHEADER),1,fp);   //从fp中读取BITMAPINFOHEADER信息到infoHead中,同时fp的指针移动

    if((sizeof(infoHead)+ sizeof(fileheader))!=fileheader.bfOffBits){
        cout<<fileheader.bfOffBits<<" "<<infoHead.biSize<<"  "<< sizeof(fileheader)<<"  "<< sizeof(char);
        colornum = (fileheader.bfOffBits-(infoHead.biSize+ sizeof(fileheader)))/ sizeof(char);
        pColorBuf = new unsigned char[colornum];
        fread(pColorBuf,sizeof(char),colornum,fp);
    }
    bmpwidth = infoHead.biWidth;
    bmpheight = infoHead.biHeight;
    linebyte = (bmpwidth*infoHead.biBitCount/8+3)/4*4; //计算每行的字节数，24：该图片是24位的bmp图，3：确保不丢失像素
    cout<<infoHead.biBitCount;
    linebyte = 512; //计算每行的字节数，24：该图片是24位的bmp图，3：确保不丢失像素
    cout<<bmpwidth<<" "<<bmpheight<<"  "<< sizeof(fileheader)<<"  "<< sizeof(infoHead)<<"   "<<
        fileheader.bfOffBits<<infoHead.biCompression<<endl;
    pBmpBuf = new unsigned char** [bmpheight];
    for(int i=0;i<bmpheight;i++){
        pBmpBuf[i] = new unsigned char* [bmpwidth];
        for(int j=0;j<bmpwidth;j++){
            pBmpBuf[i][j] = new unsigned char [infoHead.biBitCount/8];
        }
    }
    for(int i=0;i<bmpheight;i++){
        for(int j=0;j<bmpwidth;j++){
//            cout<<sizeof(char)<<"  "<<infoHead.biBitCount/8<<endl;
            fread(pBmpBuf[i][j],sizeof(char),infoHead.biBitCount/8,fp);
//            cout<<(int)pBmpBuf[i][j][0]<<"   "<<(int)pBmpBuf[i][j][1]<<"   "<<(int)pBmpBuf[i][j][2]<<"   ";
//            cout<<(int)pBmpBuf[i][j][0];
        }
    }

    fclose(fp);   //关闭文件
    return TRUE;
}


unsigned char * ResampleBilinear(double x, double y, unsigned char *** pdfValue, int nWidth, int nHeight,int color)
{
    double dfBilinearValue;
    //双线性插值计算
    int x_floor = x;
    int x_ceil = (x+1);
    int y_floor = y;
    int y_ceil = (y+1);

    /*we do not handle the border, attention*/
//    if(x<0 || y<0 || (y+1) > (nHeight-1) || (x+1) > (nWidth-1) )
//        return INVALDE_DEM_VALUE;
    unsigned char *Value = new unsigned char [color];
    for(int i=0;i<color;i++){
        double r1 = (x_ceil - x)*(double)(int)pdfValue[y_ceil][x_floor][i]
                    + (x- x_floor)*(double)(int)pdfValue[y_ceil][x_ceil][i];

        double r2 = (x_ceil - x)*(double)(int)pdfValue[y_floor][x_floor][i]
                    + (x- x_floor)*(double)(int)pdfValue[y_floor][x_ceil][i];

        Value[i] = (y-y_floor)*r1 + (y_ceil - y)*r2;
    }
    return Value; // 所有波段都是无效值才返回false
}

void writeBmp(){
    char writeFileName[] = "..\\lena_C_cut.bmp";
    BITMAPFILEHEADER fileheader_cut = fileheader;
    BITMAPINFOHEADER infoHead_cut = infoHead;
    double times = 2;
    int newbmpheight = bmpheight*times;
    int newbmpwidth = bmpwidth*times;
    fileheader_cut.bfSize = (int)(double)(fileheader_cut.bfSize - sizeof(infoHead)- sizeof(fileheader))*times*times;
    infoHead_cut.biHeight = (int)(double)infoHead_cut.biHeight*times;
    infoHead_cut.biWidth = (int)(double)infoHead_cut.biWidth*times;
    infoHead_cut.biSizeImage = (int)(double)infoHead_cut.biSizeImage*times;
    FILE *fp;
    fp=fopen("..\\lena_C_cut.bmp","wb");
    fwrite(&fileheader_cut,sizeof(fileheader_cut),1,fp);
    fwrite(&infoHead_cut,sizeof(infoHead_cut),1,fp);

    cout<<bmpheight<<linebyte;
    if((sizeof(infoHead)+ sizeof(fileheader))!=fileheader.bfOffBits){
        fwrite(pColorBuf, sizeof(char),colornum,fp);
    }
    cout<<linebyte<<"sxa"<<bmpheight<<endl;

    newBmpBuf = new unsigned char**[newbmpheight];
    for(int i=0;i<newbmpheight;i++){
        newBmpBuf[i] = new unsigned char* [newbmpwidth];
    }


    for(int i=0;i<newbmpheight;i++){
        for(int j=0;j<newbmpwidth;j++){
            double y = (double)(bmpheight-1)/(double)newbmpheight*(double)i;
            double x = (double)(bmpwidth-1)/(double)newbmpwidth*(double)j;
//            cout<<endl<<x<<"  "<<y;
            newBmpBuf[i][j] = ResampleBilinear(x,y,pBmpBuf,bmpwidth,bmpheight,infoHead.biBitCount/8);
        }
    }
    for(int i=0;i<newbmpheight;i++){
        for(int j=0;j<newbmpwidth;j++){
            fwrite(newBmpBuf[i][j],sizeof(char),infoHead.biBitCount/8,fp);
        }
    }

    cout<<(int)pBmpBuf[0];
    fclose(fp);
}
void solve()
{
    char readFileName[] = "..\\lena.bmp";
    if(FALSE == readBmp(readFileName))
        cout<<"readfile error!"<<endl;
    writeBmp();
}

int main()
{
    solve();

    return 0;
}
