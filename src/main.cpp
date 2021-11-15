 /*
* Copyright(C) 2011,Hikvision Digital Technology Co., Ltd 
* 
* File   name£ºmain.cpp
* Discription£ºdemo for muti thread get stream
* Version    £º1.0
* Author     £ºluoyuhua
* Create Date£º2011-12-10
* Modification History£º
*/

/*
*  Updated by Eku Jiang 2021/11/15
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "HCNetSDK.h"
#include "PlayM4.h"
#include "iniFile.h"
#include "opencv2/opencv.hpp"
#include "cv.h"
#include "highgui.h"

FILE *g_pFile = NULL;
FILE *VideoYUVfile=NULL;
FILE *AudioPCMfile=NULL;

int iUserID = -1; //设备登录句柄
int nPort=-1; //解码的播放库句柄

//取流相关信息，用于线程传递
typedef struct tagREAL_PLAY_INFO
{
	char szIP[16];
	int iUserID;
	int iChannel;
}REAL_PLAY_INFO, *LPREAL_PLAY_INFO;

// update time: 2021/11/15
// function: YV12_TO_YUV444
void yv12toYUV(char *outYuv, char *inYv12, int width, int height,int widthStep)
{
   int col,row;
   unsigned int Y,U,V;
   int tmp;
   int idx;
 
  //printf("widthStep=%d.\n",widthStep);
 
   for (row=0; row<height; row++)
   {
      idx=row * widthStep;
      int rowptr=row*width;
 
      for (col=0; col<width; col++)
      {
         //int colhalf=col>>1;
         tmp = (row/2)*(width/2)+(col/2);
//         if((row==1)&&( col>=1400 &&col<=1600))
//         { 
//          printf("col=%d,row=%d,width=%d,tmp=%d.\n",col,row,width,tmp);
//          printf("row*width+col=%d,width*height+width*height/4+tmp=%d,width*height+tmp=%d.\n",row*width+col,width*height+width*height/4+tmp,width*height+tmp);
//         } 
         Y=(unsigned int) inYv12[row*width+col];
         U=(unsigned int) inYv12[width*height+width*height/4+tmp];
         V=(unsigned int) inYv12[width*height+tmp];
				 
         if((idx+col*3+2)> (1200 * widthStep))
         {
          //printf("row * widthStep=%d,idx+col*3+2=%d.\n",1200 * widthStep,idx+col*3+2);
         } 
         outYuv[idx+col*3]   = Y;
         outYuv[idx+col*3+1] = U;
         outYuv[idx+col*3+2] = V;
      }
   }
   //printf("col=%d,row=%d.\n",col,row);
}


//////////////////////////////////////////////////////////////////////////
////解码回调 视频为YUV数据(YV12)，音频为PCM数据
void CALLBACK DecCBFun(int nPort, char* pBuf, int nSize, FRAME_INFO * pFrameInfo, void* nUser, int nReserved2)
{
        char filename[100];
 	int lFrameType = pFrameInfo->nType;	
	if (lFrameType == T_AUDIO16)
	{
		printf("Audio nStamp:%d\n",pFrameInfo->nStamp);
		printf("test_DecCb_Write Audio16 \n");
		if (AudioPCMfile==NULL)
		{
			sprintf(filename,"./record/AudionPCM.pcm");
			AudioPCMfile = fopen(filename,"wb");
		}
		fwrite(pBuf,nSize,1,AudioPCMfile);
	}

	else if(lFrameType == T_YV12)
	{		
	    printf("当前视频时间戳 :%\n",pFrameInfo->nStamp);
		printf("test_DecCb_Write YUV \n");
		printf("当前视频帧长 %d \t 当前视频帧宽 %d \t 当前视频帧当前视频帧总大小 %d %d\n", 
		pFrameInfo->nWidth, pFrameInfo->nHeight, nSize);
		// if (VideoYUVfile==NULL)
		// {
		// 	sprintf(filename,"./record/VideoYV12.yuv");
		// 	VideoYUVfile = fopen(filename,"wb");
		// }
		// fwrite(pBuf,nSize,1,VideoYUVfile);

		cv::Mat YUVImg(pFrameInfo->nHeight + pFrameInfo->nHeight / 2, pFrameInfo->nWidth, CV_8UC1, pBuf);
    cv::Mat BGRImg;
    cv::cvtColor(YUVImg, BGRImg, cv::COLOR_YUV2BGR_YV12);  // 随着图片像素增加，cup占用率也增加

		cv::imshow("相机读取界面", BGRImg);
		cv::waitKey(1);
	}
	else
	{

	}
}

void PsDataCallBack(LONG lRealHandle,DWORD dwDataType,BYTE *pBuffer,DWORD dwBufSize,void *pUser)
{
        DWORD dRet = 0;
	BOOL inData = FALSE;

	if (dwDataType  == NET_DVR_SYSHEAD)
	{	
        /////////////////////////////////////////////////////
		//保存回调码流数据，保存录像文件
		//写入头数据
		g_pFile = fopen("./record/ps.dat", "wb");
		
		if (g_pFile == NULL)
		{
			printf("CreateFileHead fail\n");
			return;
		}

		//写入头数据
		fwrite(pBuffer, sizeof(unsigned char), dwBufSize, g_pFile);
		printf("write head len=%d\n", dwBufSize);
		
		/////////////////////////////////////////////////////
		//播放库解码
		if (!PlayM4_GetPort(&nPort))
		{
			return;
		}
		
		if (!PlayM4_OpenStream(nPort,pBuffer,dwBufSize,1024*1024))
		{
			dRet=PlayM4_GetLastError(nPort);
			printf("PlayM4_OpenStream failed, error code=%d\n", dRet);
			return;
		}

		//设置解码回调函数 只解码不显示
		if (!PlayM4_SetDecCallBackMend(nPort,DecCBFun, NULL))
 		{
 			dRet=PlayM4_GetLastError(nPort);
			printf("PlayM4_SetDecCallBack failed, error code=%d\n", dRet);
 			return;
 		}	

		//打开视频解码
		if (!PlayM4_Play(nPort, NULL))
		{
			dRet=PlayM4_GetLastError(nPort);
			printf("PlayM4_Play failed, error code=%d\n", dRet);
			return;
		}

		//打开音频解码, 需要码流是复合流
		if (!PlayM4_PlaySound(nPort))
		{
			dRet=PlayM4_GetLastError(nPort);
			printf("PlayM4_PlaySound failed, error code=%d\n", dRet);
		}
	}
	else
	{
		/////////////////////////////////////////////////////
		//保存回调码流数据，保存录像文件
		if(g_pFile != NULL)
		{
			fwrite(pBuffer, sizeof(unsigned char), dwBufSize, g_pFile);
			printf("write data len=%d\n", dwBufSize);
		}
		
		/////////////////////////////////////////////////////
		//播放库解码
		inData=PlayM4_InputData(nPort,pBuffer,dwBufSize);
		while (!inData)
		{
			printf("PlayM4_InputData failed, error code=%d\n", dRet);
			//缓冲期满，等待一会儿重新送入解码
			if(PlayM4_GetLastError(nPort) == 11)
			{
				usleep(5000);
				inData=PlayM4_InputData(nPort,pBuffer,dwBufSize);	
			}
			else
			{
				break;
			}			
		}
	}	

}



void GetStream()
{
	// 从配置文件读取设备信息 
	IniFile ini("Device.ini");
	unsigned int dwSize = 0;
	char sSection[16] = "DEVICE";

	
	char *sIP = ini.readstring(sSection, "ip", "error", dwSize);
	int iPort = ini.readinteger(sSection, "port", 0);
	char *sUserName = ini.readstring(sSection, "username", "error", dwSize); 
	char *sPassword = ini.readstring(sSection, "password", "error", dwSize);
	int iChannel = ini.readinteger(sSection, "channel", 0);
		
	NET_DVR_DEVICEINFO_V30 struDeviceInfo;
	iUserID = NET_DVR_Login_V30(sIP, iPort, sUserName, sPassword, &struDeviceInfo);
	if(iUserID >= 0)
	{

    		//NET_DVR_CLIENTINFO ClientInfo = {0};
    		//ClientInfo.lChannel     = iChannel;  //channel NO.
    		//ClientInfo.lLinkMode    = 0;
    		//ClientInfo.sMultiCastIP = NULL;
    		//int iRealPlayHandle = NET_DVR_RealPlay_V30(iUserID, &ClientInfo, PsDataCallBack, NULL, 0);
		NET_DVR_PREVIEWINFO struPreviewInfo = {0};
		struPreviewInfo.lChannel =iChannel;
		struPreviewInfo.dwStreamType = 0;
		struPreviewInfo.dwLinkMode = 0;
		struPreviewInfo.bBlocked = 1;
		struPreviewInfo.bPassbackRecord  = 1;
		int iRealPlayHandle = NET_DVR_RealPlay_V40(iUserID, &struPreviewInfo, PsDataCallBack, NULL);
		printf("[GetStream] iRealPlayHandle %d \n", iRealPlayHandle);
		if(iRealPlayHandle >= 0)
		{
			printf("[GetStream]---RealPlay %s:%d success, \n", sIP, iChannel, NET_DVR_GetLastError());
			//int iRet = NET_DVR_SaveRealData(iRealPlayHandle, "./record/realplay.dat");
			//NET_DVR_SetStandardDataCallBack(iRealPlayHandle, StandardDataCallBack, 0);

		}
		else
		{
			printf("[GetStream]---RealPlay %s:%d failed, error = %d\n", sIP, iChannel, NET_DVR_GetLastError());
		}
	}
	else
	{
		printf("[GetStream]---Login %s failed, error = %d\n", sIP, NET_DVR_GetLastError());
	}
}


int main()
{
    NET_DVR_Init();
    NET_DVR_SetLogToFile(3, "./record/");

    GetStream();
	
    char c = 0;
    while('q' != c)
    {
		printf("input 'q' to quit\n");
		printf("input: ");
		printf("helllllllllllllllllllllllll\n");
		scanf("%c", &c);
    }



    //释放播放库资源
    if(nPort >-1)
    {
        PlayM4_Stop(nPort);
        PlayM4_CloseStream(nPort);
        PlayM4_FreePort(nPort);
    }

    if(iUserID > -1)
    {
        NET_DVR_Logout(iUserID);
    }    

    NET_DVR_Cleanup();

    //Close Files
    if (AudioPCMfile!=NULL)
    {
        fclose(AudioPCMfile);
        AudioPCMfile=NULL;
    }
    if (VideoYUVfile!=NULL)
    {
        fclose(VideoYUVfile);
        VideoYUVfile=NULL;
    }	
    if (g_pFile!=NULL)
    {
        fclose(g_pFile);
        g_pFile=NULL;
    }	

    return 0;
}


