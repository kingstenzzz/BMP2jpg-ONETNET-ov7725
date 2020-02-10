/*************************************************************************
*��Ŀ����������ƣ� ����STM32+0V7670+NRF24L01��������Ƶ����С��
*�������ڣ�         2011.4.9
*������:            ��־ǿ
*ģ�鹦�ܣ�         MAIN2.c
*�޸����ڣ�
*�����ĵ���          IJG JPEG LIBRARY
*�ر�˵��:   	     ��ѹ���㷨��Դ��(JPEG Group's software)IJG JPEG LIBRARY��
                    www.ouravr.com��VERYVERY���Ѷ����������ֲ���Ҷ��������
                    �Ż����޸ģ�
                                       
*************************************************************************/
#include "jinclude.h"
#include "jcapi.h"
#include "test.h"
/*************************************************************************
*����������ȫ�ֱ�����λ����
*������ڣ�
*��������:
*����˵��:   
*************************************************************************/
unsigned char JPG_enc_buf[20000];//jpeg ���
volatile unsigned int pt_buf;//������ָ��
__no_init unsigned char inbuf_buf[230400] @ 0x68000000;//���뻺�壬���ⲿSRAM
//ע�⣺ԭʼRGB565���ݺ����뻺����ͬһ������RAM�����������ǽ�ʡ�ռ�!!
jpeg_compress_info info1;
JQUANT_TBL  JQUANT_TBL_2[2];
JHUFF_TBL   JHUFF_TBL_4[4];
unsigned char dcttab[3][512];
//ѹ��
void Compression(int width,int height,float quality) 
{
  jpeg_compress_info *cinfo;

  pt_buf=0;
  cinfo=jpeg_create_compress();
  cinfo->image_width=width;//�趨ͼƬ��С
  cinfo->image_height=height;
  cinfo->output=JPG_enc_buf;//�趨�������λ��
  jpeg_set_default(cinfo,quality);  
  jpeg_start_compress(cinfo);
  while(cinfo->next_line<cinfo->image_height)//һ��һ�е�ɨ������뻺���� 
  {
    jpeg_write_scanline(cinfo,&inbuf_buf[(cinfo->next_line*cinfo->image_width*3)]);
  }
  jpeg_finish_compress(cinfo);
}


