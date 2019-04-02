#include <stdlib.h >
#include "encode.h"
#include "delay.h"
//#include "led.h"
#include "./led/bsp_led.h"   
//#include "sd.h"
//#include "bsp_sdio_sdcard.h"
//#include "fatfs.h"
#include "./usart/bsp_usart.h"
#include "./ov7725/bsp_ov7725.h"
#include "./bmp/bsp_bmp.h"
	
	JQUANT_TBL JQUANT_TBL_2[2];
	JHUFF_TBL  JHUFF_TBL_4[4];
	u8 dcttab[3][512];
	static union buffer_tag buffer;
	static int bits_in_buffer;
FIL *fileW;
u8 r,g,b;

extern u32 lcdid;
_Bool JPEG_encode(char *filename)//����������
{

	u8 res;
	u16 count,i,color;
	jpeg_compress_info* cinfo;
	JSAMPLE *inbuf_buf;

	
	
//	#if JPG_USE_MALLOC == 1	//ʹ��malloc	
	cinfo=(jpeg_compress_info*)malloc(sizeof(jpeg_compress_info));	
	if(cinfo==NULL)
	{
		printf("malloc cinfo failed");
	}
		inbuf_buf=(u8*)malloc(camera_WIDTH*48*sizeof(u8));		//����readlen�ֽڵ��ڴ�����
		if(inbuf_buf==NULL)
		{
			printf("malloc buf faild");
		}
	//memset((void*)(inbuf_buf),0,sizeof(inbuf_buf));//����Ԫ������
//	if(inbuf_buf==NULL)return PIC_MEM_ERR;	//�ڴ�����ʧ��.
	fileW=(FIL *)malloc(sizeof(FIL));	//����FIL�ֽڵ��ڴ����� 
	if(fileW==NULL||cinfo==NULL||inbuf_buf==NULL)							//�ڴ�����ʧ��.
	{
		printf("	JPEG_encode//�ڴ�����ʧ��.");///////////////////////////////////////////
		free(cinfo);
		free(inbuf_buf);
		free(fileW);
		return 1;
	} 	 

	
	//res=f_mount(0, &fatfs);
	//while(res){;}//LCD_ShowString(20,20,"SD mount failed!");
	//res=f_open(&fileW,"0:/DCMI/ph1.jpg",FA_WRITE|FA_CREATE_ALWAYS);//��������
//res=f_open(f_bmp,(const TCHAR*)filename,FA_WRITE|FA_CREATE_NEW);
	res = f_open( fileW , (char*)filename, FA_CREATE_ALWAYS | FA_WRITE );
	
	/* �½��ļ�֮��Ҫ�ȹر��ٴ򿪲���д�� */
	f_close(fileW);
		
	res = f_open( fileW , (char*)filename,  FA_OPEN_EXISTING | FA_WRITE);


	if(res==FR_OK)
	{
		
	
		
//	while(res){;}//LCD_ShowString(20,20,"file create failed!");
  	// printf("	JPEG_encode//��������");///////////////////////////////////////////
	cinfo->state=JC_CREATING;  
	memset((void*)(cinfo),0,sizeof(jpeg_compress_info));//����Ԫ������
//	cinfo=jpeg_create_compress();//����JPEGѹ���ļ�
	cinfo->image_width=camera_WIDTH;
  	cinfo->image_height=camera_HEIGHT;
  	cinfo->output=0;//���������NULL;
	
	cinfo->fileW=fileW;//�ļ�/////////////�ļ�///////////�ļ�///////////�ļ�/////////////�ļ�///////////�ļ�/////////
	
  	jpeg_set_default(cinfo,inbuf_buf);//����Ĭ�ϲ���
	
	  	 //printf("	JPEG_encode//����Ĭ�ϲ���");///////////////////////////////////////////
	
	jpeg_start_compress(cinfo);//��ʼѹ��,дѹ���ļ�ͷ��Ϣ
	
		  	 //printf("	JPEG_encode//��ʼѹ��,дѹ���ļ�ͷ��Ϣ");///////////////////////////////////////////
	
	FIFO_PREPARE;  			/*FIFO׼��*/					
  count=0;
	while(cinfo->next_line<cinfo->image_height)
	{
		for(i=0;i<camera_WIDTH;i++)//��ȡһ��
		{
			READ_FIFO_PIXEL(color);
		 cinfo->inbuf[count++]=(u8)((color&0xf800)>>8);
			cinfo->inbuf[count++]=(u8)((color&0x07e0)>>3);
			cinfo->inbuf[count++]=(u8)((color&0x001f)<<3);						
		}
		
		cinfo->next_line++; 
	  	//����������ʱѹ�����������(����16��)
	  	if(cinfo->next_line%cinfo->inbuf_height==0)
		{
			count=0;
	    	jint_process_rows(cinfo);//������ѹ��
      	memset((void*)(cinfo->inbuf),0,cinfo->inbuf_size);//������뻺����
			
	  	}
	}	
	jpeg_finish_compress(cinfo);
	printf("	JPEG_encode//jpeg_finish_compress(cinfo)");///////////////////////////////////////////
	f_close(fileW);
	free(cinfo);
	free(inbuf_buf);
	free(fileW);
	return 0;
}
	else
	{
		printf("open faild");
		free(cinfo);
		free(inbuf_buf);
		free(fileW);
		return 1;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////
//����ѹ���ļ���Ϣ
/*jpeg_compress_info *jpeg_create_compress(void) 
{
  	jpeg_compress_info *cinfo=&info1;    
	memset((void*)(cinfo),0,sizeof(jpeg_compress_info));//����Ԫ������
	cinfo->state=JC_CREATING;
  	return cinfo;
}*/
//����Ĭ�ϲ���
void jpeg_set_default(jpeg_compress_info *cinfo,u8 *inbuf_buf) 
{
  	cinfo->precision=8;//��������8  
  	cinfo->in_color_space=JCS_RGB;//ɫ�ʿռ�
  	//cinfo->quality=0.2f;//ͼ������,ֵԽС����Խ�� 
	
	cinfo->quality=1.1f;//ͼ������,ֵԽС����Խ�� 
	
  	//��ʼ��Ԫ����ɫΪYCbCr��ɫ  
  	cinfo->num_comp=3;//Ԫ������Ϊ3
  	SET_COMP(cinfo->comp[0],1,2,2,0,0,0);//
  	SET_COMP(cinfo->comp[1],2,1,1,1,1,1);//
  	SET_COMP(cinfo->comp[2],3,1,1,1,1,1);//  	
  	jint_std_quant_tables(cinfo);//���ñ�׼������  	
  	jint_std_huff_tables(cinfo);//���ñ�׼huffman��  
  	jpeg_calc_value(cinfo,inbuf_buf);//��������
}
//���ñ�׼������
void jint_std_quant_tables(jpeg_compress_info *cinfo)
{
  	jint_add_quant_table(cinfo,0,std_luminance_quant_tbl,cinfo->quality);
  	jint_add_quant_table(cinfo,1,std_chrominance_quant_tbl,cinfo->quality);
}
//���������
void jint_add_quant_table(jpeg_compress_info *cinfo,u16 which_tbl,const u16 *base_tbl,float quality) 
{
  	int i;
  	DCTVAL *qtblptr;
  	cinfo->quant_table[which_tbl]=&JQUANT_TBL_2[which_tbl];
  	qtblptr=cinfo->quant_table[which_tbl]->quantval;
  	for(i=0;i<DCTSIZE2;i++)qtblptr[i]=(short)(base_tbl[i]*quality+0.5);
  	jutl_zigzag_sort(qtblptr);
}
//Z������
void jutl_zigzag_sort(DCTBLOCK dptr) 
{
  	int i;

  	DCTBLOCK dtmp;
  	memcpy(&dtmp,dptr,sizeof(DCTBLOCK));
  	for(i=0;i<DCTSIZE2;i++)dptr[zz_order[i]]=dtmp[i];
}
//���ñ�׼huffman��
void jint_std_huff_tables (jpeg_compress_info *cinfo) 
{ 
  	jint_add_huff_table(cinfo,0,DC_HUFF_TBL,bits_dc_luminance,val_dc_luminance);
  	jint_add_huff_table(cinfo,0,AC_HUFF_TBL,bits_ac_luminance,val_ac_luminance);
  	jint_add_huff_table(cinfo,1,DC_HUFF_TBL,bits_dc_chrominance,val_dc_chrominance);
  	jint_add_huff_table(cinfo,1,AC_HUFF_TBL,bits_ac_chrominance,val_ac_chrominance);
}
//���huffman��
void jint_add_huff_table(jpeg_compress_info *cinfo,u16 which_tbl,int what_tbl,const u8 *bits,const u8 *value) 
{
  	JHUFF_TBL **hpp;
  	int nsymbols,len; 
  	if(which_tbl>NUM_HUFF_TBLS)return;
  	if(what_tbl==0)//ֱ��������
    hpp=cinfo->dc_huff_table;
  	else if(what_tbl==1)hpp=cinfo->ac_huff_table;//����������
  	else return ;
	hpp[which_tbl]=&JHUFF_TBL_4[which_tbl*2+what_tbl];
	memcpy(hpp[which_tbl]->bits,bits,sizeof(hpp[which_tbl]->bits));	  
  	nsymbols=0;//����huffval�б�־������
	for(len=1;len<=16;len++)nsymbols+=bits[len];
  	memcpy(hpp[which_tbl]->huffval,value,nsymbols*sizeof(u8));//set huffval  
  	//���㲢����huffman��&��
  	memset(hpp[which_tbl]->ecode,0,sizeof(hpp[which_tbl]->ecode));
  	memset(hpp[which_tbl]->esize,0,sizeof(hpp[which_tbl]->esize));
  	jint_calc_huff_tbl(hpp[which_tbl]);
}
//����huffman��
static u16 gtmp_hsize[256];
static u16 gtmp_hcode[256];
void jint_calc_huff_tbl(JHUFF_TBL *htblptr)
{
  	int i,j,k,lastk,code,si;    	
  	k=0;//����EHUFFSI��
  	for(i=1;i<=16;i++)
	{
    	for (j=1; j<=htblptr->bits[i];j++)
		{
      		gtmp_hsize[k]=i;
      		k++;
    	}
  	}
  	gtmp_hsize[k]=0;
  	lastk=k;    	
  	k=0;//����EHUFFCO��
  	code=0;
  	si=gtmp_hsize[0];
  	while(1)
	{
    	while(1)
		{
      		gtmp_hcode[k]=code;
      		code++;
      		k++;
      		if(gtmp_hsize[k]!=si)
        	break;
    	}
    	if(gtmp_hsize[k]==0)
      	break;
    	while(1)
		{
      		code=code<<1;
      		si++;
      		if(gtmp_hsize[k]==si)
        	break;
    	}
  	}
   	k=0;//ָ�����
  	do{
    	i=htblptr->huffval[k];
    	htblptr->ecode[i]=gtmp_hcode[k];
    	htblptr->esize[i]=gtmp_hsize[k];
    	k++;
  	}while(k<lastk);
}
//�����һЩ���õ�ֵ�����䵽��������
void jpeg_calc_value(jpeg_compress_info *cinfo,u8 *inbuf_buf) 
{
  	int i,h,v,bn;
  	int size;
  	jpeg_component_info *pcomp;  
	cinfo->state=JC_SETTING;
	bn=0;
	cinfo->max_h_factor=0;
	cinfo->max_v_factor=0;	
	for(i=0;i<cinfo->num_comp;i++)//ѭ������ÿ��Ԫ������Ϣ
	{
		pcomp=&cinfo->comp[i];
		h=pcomp->h_factor;
		v=pcomp->v_factor;	
		if(cinfo->max_h_factor<h)cinfo->max_h_factor=h;
		if(cinfo->max_v_factor<v)cinfo->max_v_factor=v;
		bn+=(h*v);	
		//�����������ڴ�ռ�,��Ҫ��jpeg_destory_compress()�����ͷ���,�����û����������ѹ��ʱ������
		pcomp->num_dct_table=h*v;
		size=sizeof(DCTBLOCK)*pcomp->num_dct_table;
		pcomp->dct_table=(DCTBLOCK*)(dcttab[i]);//malloc(size);
		memset((void*)(pcomp->dct_table),0,size);
	}	
	//if(bn>MAX_BLOCK_IN_MCU)exit(1);	
	cinfo->block_in_mcu=bn;
	cinfo->mcu_width=cinfo->max_h_factor*DCTSIZE;
	cinfo->mcu_height=cinfo->max_v_factor*DCTSIZE;
	cinfo->mcu_blocks=cinfo->mcu_width*cinfo->mcu_height;	
	//���ú�MCU�йص�ֵ 
	cinfo->mcu_per_row=cinfo->image_width/cinfo->mcu_width;
	if(cinfo->image_width%cinfo->mcu_width!=0)
	cinfo->mcu_per_row++;
	cinfo->mcu_rows=cinfo->image_height/cinfo->mcu_height;
	if(cinfo->image_height%cinfo->mcu_height!=0)
	cinfo->mcu_rows++;	
	//�������뻺����
	cinfo->inbuf_width=cinfo->mcu_per_row*cinfo->mcu_width;
	cinfo->inbuf_height=cinfo->mcu_height;
	cinfo->inbuf_size=cinfo->inbuf_width*cinfo->inbuf_height*cinfo->num_comp*sizeof(JSAMPLE);
	cinfo->inbuf=(JSAMPLE*)inbuf_buf;//malloc(cinfo->inbuf_size);
 	cinfo->next_line=0;//��״ֵ̬
}
///////////////////////////////////////////////////////////////////////////////////////////
u8  write_buf[512];
u16 wr_pt=0;
void jutl_write_byte(u8 c,char *f,jpeg_compress_info *cinfo)
{
	UINT WRBytes;

	write_buf[wr_pt]=c;
	wr_pt++;
  	if(wr_pt==512||cinfo->state==JC_FINISH)//512�ֽ�дһ��
	{
		f_write(cinfo->fileW,write_buf,wr_pt,&WRBytes);
		wr_pt=0;
	}
}
//��ʼѹ������,д��ͷ��Ϣ
void jpeg_start_compress(jpeg_compress_info *cinfo) 
{
  	int i;

  	jmkr_write_start(cinfo);//дͷ��Ϣ,�����ļ�ͷ,֡ͷ��ɨ�迪ʼ  
  	cinfo->state=JC_START;//���浱ǰ״̬(��ʼ)
  	cinfo->next_line=0;//��ʼ����ʼ����
  	for(i=0;i<cinfo->num_comp;i++)cinfo->comp[i].preval=0;
}
//дͷ��Ϣ
void jmkr_write_start(jpeg_compress_info *cinfo) 
{
  	jmkr_write_image_start(cinfo);//�ļ���ʼ��־
  	jmkr_write_jfif(cinfo);//APP0��Ϣ
  	jmkr_write_quant_table(cinfo);//quant��
  	jmkr_write_frame_head(cinfo);//֡ͷ
  	jmkr_write_huff_table(cinfo);//��������
  	jmkr_write_scan_head(cinfo);//ɨ�迪ʼ
}
//д���ļ���ʼ��־ 
void jmkr_write_image_start(jpeg_compress_info *cinfo)
{
  	jutl_write_byte(0xFF,cinfo->output,cinfo);
  	jutl_write_byte(SOI,cinfo->output,cinfo);
}
//д��APP0Ӧ�����ݶ�
void jmkr_write_jfif(jpeg_compress_info *cinfo)
{
  	int i;
  	for (i=0;i<18;i++)jutl_write_byte(jfcode[i], cinfo->output,cinfo);
}
//д��������
void jmkr_write_quant_table(jpeg_compress_info *cinfo)
{
  	int i, j;
  	for (i=0;i<NUM_QUANT_TBLS;i++) 
  	{
	    if (!cinfo->quant_table[i])continue;	      
	    jutl_write_byte(0xFF,cinfo->output,cinfo);
	    jutl_write_byte(DQT,cinfo->output,cinfo);
	    jutl_write_byte(0x00,cinfo->output,cinfo);
	    jutl_write_byte(0x43,cinfo->output,cinfo );//����:�̶�Ϊ43h
	    jutl_write_byte(i&0xFF,cinfo->output,cinfo );//ouput Pq & Tq
	    for (j=0;j<DCTSIZE2;j++) 
	    {
	      	u8 Q=cinfo->quant_table[i]->quantval[j];
	      	jutl_write_byte(Q,cinfo->output,cinfo  );
	    }
  	}
}
//д��֡ͷ
void jmkr_write_frame_head(jpeg_compress_info *cinfo)
{
  	int i;
  	u16 Lf,X,Y;
  	u8  P,Nf,C,H,V,Tq;
	jutl_write_byte(0xFF,cinfo->output,cinfo  );
	jutl_write_byte(SOF0,cinfo->output,cinfo  );
	Nf=cinfo->num_comp;
	P=cinfo->precision;
	X=cinfo->image_width;
	Y=cinfo->image_height;
	Lf=8+3*Nf;
	jutl_write_byte(Lf>>8,cinfo->output,cinfo  );
	jutl_write_byte(Lf&0xFF,cinfo->output,cinfo  );
	jutl_write_byte(P,cinfo->output,cinfo  );
	jutl_write_byte(Y>>8,cinfo->output,cinfo  );
	jutl_write_byte(Y&0xFF,cinfo->output,cinfo  );
	jutl_write_byte(X>>8,cinfo->output,cinfo  );
	jutl_write_byte(X&0xFF,cinfo->output,cinfo  );
	jutl_write_byte(Nf,cinfo->output,cinfo  );
	for(i=0;i<Nf;i++)
	{
		C=cinfo->comp[i].comp_id;
		H=cinfo->comp[i].h_factor;
		V=cinfo->comp[i].v_factor;
		Tq=cinfo->comp[i].quant_tbl_no;
		jutl_write_byte(C,cinfo->output,cinfo  );
		jutl_write_byte(((H<<4)|V),cinfo->output,cinfo  );
		jutl_write_byte(Tq,cinfo->output,cinfo  );
	}
}
//д���������
void jmkr_write_huff_table(jpeg_compress_info *cinfo)
{
  	int i,j,num_val;
  	u16 Lh;
	for(i=0;i<NUM_HUFF_TBLS;i++)//���ֱ��HUFFMAN��
	{
		num_val=0;
		if(!cinfo->dc_huff_table[i])continue;
		jutl_write_byte(0xFF,cinfo->output,cinfo  );
		jutl_write_byte(DHT,cinfo->output,cinfo  );
		for(j=1;j<=16;j++)num_val+=cinfo->dc_huff_table[i]->bits[j];
		Lh=3+16+num_val;//����,�̶�Ϊ43h
		jutl_write_byte(Lh>>8,cinfo->output,cinfo  );
		jutl_write_byte(Lh&0xFF,cinfo->output,cinfo  );
		jutl_write_byte(i,cinfo->output,cinfo  );//���ID
		for(j=1;j<=16;j++)jutl_write_byte(cinfo->dc_huff_table[i]->bits[j],cinfo->output,cinfo  );//���bits
		for(j=0;j<num_val;j++)jutl_write_byte(cinfo->dc_huff_table[i]->huffval[j],cinfo->output,cinfo  );
	}
	for(i=0;i<NUM_HUFF_TBLS;i++)//�������HUFFMAN��
	{
		num_val=0;
		if(!cinfo->ac_huff_table[i])continue;
		jutl_write_byte(0xFF,cinfo->output,cinfo  );
		jutl_write_byte(DHT,cinfo->output,cinfo  );
		for(j=1;j<=16;j++)num_val+=cinfo->ac_huff_table[i]->bits[j];
		Lh=3+16+num_val;//����,�̶�Ϊ43h
		jutl_write_byte(Lh>>8,cinfo->output,cinfo  );
		jutl_write_byte(Lh&0xFF,cinfo->output,cinfo  );
		jutl_write_byte((0x10|i),cinfo->output,cinfo  );//���ID
		for(j=1;j<=16;j++)jutl_write_byte(cinfo->ac_huff_table[i]->bits[j],cinfo->output,cinfo  );//���bits
		for(j=0;j<num_val;j++)jutl_write_byte(cinfo->ac_huff_table[i]->huffval[j],cinfo->output,cinfo  );
	}
}
//д��ɨ�迪ʼ��־
void jmkr_write_scan_head(jpeg_compress_info *cinfo) 
{
  	int i;
  	u16 Ls;
  	u8 Ns,Ss,Se,Ah,Al,Cs,Td,Ta;
	jutl_write_byte(0xFF,cinfo->output,cinfo  );
	jutl_write_byte(SOS,cinfo->output,cinfo  );
	Ns=cinfo->num_comp;
	Ls=6+2*Ns;
	Ss=0;//��ֵ!���ı�
	Se=63;
	Ah=Al=0;
	jutl_write_byte(Ls>>8,cinfo->output,cinfo  );
	jutl_write_byte(Ls&0xFF,cinfo->output,cinfo  );
	jutl_write_byte(Ns,cinfo->output,cinfo  );
	for(i=0;i<Ns;i++)
	{
		Cs=cinfo->comp[i].comp_id;
		Td=cinfo->comp[i].dc_tbl_no;
		Ta=cinfo->comp[i].ac_tbl_no;
		jutl_write_byte(Cs,cinfo->output,cinfo  );
		jutl_write_byte((Td<<4|Ta),cinfo->output,cinfo  );
	}
	jutl_write_byte(Ss,cinfo->output,cinfo  );
	jutl_write_byte(Se,cinfo->output,cinfo  );
	jutl_write_byte(((Ah<<4)|Al),cinfo->output,cinfo  );
}
//д�ļ�������־
void jmkr_write_end(jpeg_compress_info *cinfo) 
{
  	jutl_write_byte(0xFF,cinfo->output,cinfo  );
	cinfo->state =JC_FINISH;//״̬Ϊ���
  	jutl_write_byte(EOI,cinfo->output,cinfo  );	
}
///////////////////////////////////////////////////////////////////////////////////////////
//��ѹ������
//�ú������ڲ����ú���,�������Ҫ������,���뱣֤
//cinfo->inbuf������Ч���� ,���򽫲��ܵõ���ȷ��ѹ������
void jint_process_rows(jpeg_compress_info *cinfo) 
{  
  	int mn; //��ǰ�������MCU
  	JSAMPLE *mcu_start;  
	int mcu_delta; 
  	JSAMPLE *pdata; 
	//��ɫת��ѡ�����(����ֻ��RGB��YCrCb��ת��)
	if(cinfo->in_color_space==JCS_RGB)
	jutl_cc_rgb2ycc(cinfo->inbuf,cinfo->inbuf_width*cinfo->inbuf_height);	
	//����ÿ��MCU����,�ָ�inbuf�����ݵ�ÿ��Ԫ����dct����,������ÿ��Ԫ���Ĳ�����
	mcu_start=&cinfo->inbuf[0];
	mcu_delta=cinfo->mcu_width*cinfo->num_comp*sizeof(JSAMPLE);
	for(mn=0;mn<cinfo->mcu_per_row;mn++)
	{
		//����ÿ��Ԫ��,���Ͳ����ʲ����浽����Ԫ����
		int cn;//Ԫ��������
		for(cn=0;cn<cinfo->num_comp;cn++)
		{
			JSAMPLE *comp_start=(JSAMPLE*)(mcu_start+cn);
			//��ʼ���������(sampledelta)����ָ��
			int h_samp_delta=cinfo->max_h_factor/cinfo->comp[cn].h_factor*cinfo->num_comp*sizeof(JSAMPLE);
			int v_samp_delta=cinfo->max_v_factor/cinfo->comp[cn].v_factor*cinfo->num_comp*sizeof(JSAMPLE)*cinfo->inbuf_width;			
			//����ÿ����(block)
			int bn;//�������
			int pn;//����ָ��
			for(bn=0;bn<cinfo->comp[cn].num_dct_table;bn++)
			{
				//��ʼ���鿪ʼ��ַ
				JSAMPLE *block_start=(JSAMPLE*)(comp_start+(bn%cinfo->comp[cn].h_factor)*DCTSIZE*h_samp_delta+(bn/cinfo->comp[cn].v_factor)*DCTSIZE*v_samp_delta);				
				DCTVAL *dct=(DCTVAL*)&cinfo->comp[cn].dct_table[bn];//dct��Ԫ��
				for(pn=0;pn<DCTSIZE2;pn++)//����ÿ��Ԫ��
				{				
					if(pn%DCTSIZE==0)pdata=(JSAMPLE*)(pn/DCTSIZE*v_samp_delta+block_start);
					dct[pn]=*pdata;
					pdata+=h_samp_delta;
				}
			}
		}//��ɫԪ������ѭ������
		mcu_start+=mcu_delta;		
		jint_process_mcu(cinfo);//����ǰMCU����
	}
}
//RGBתYUV
void jutl_cc_rgb2ycc(JSAMPLE *data,int num) 
{
  	int i;
  	float a,b,c;
  	struct three_component_color{
		JSAMPLE a,b,c;
  	} *pcolor;
	pcolor=(struct three_component_color*)data;
	for(i=0;i<num;i++)
	{
		a=pcolor[i].a;
		b=pcolor[i].b;
		c=pcolor[i].c;
		pcolor[i].a=(0.29900f*a+0.58700f*b+0.11400f*c);//Y color
		pcolor[i].b=(-0.16874f*a-0.33126f*b+0.50000f*c+128);//Cb color
		pcolor[i].c=(0.50000f*a-0.41869f*b-0.08131f*c+128);//Cr color 
  	}
}
//��������Ԫ���е�MCU����
extern volatile int temp_dct1[64];
void jint_process_mcu(jpeg_compress_info *cinfo) 
{
  //����ÿ����ɫԪ��
  	int cn,tn;
  	for(cn=0;cn<cinfo->num_comp;cn++) 
  	{
    //Ԫ����ʹ�õ��ı���
    	DCTBLOCK   *dct_table=cinfo->comp[cn].dct_table;
    	JQUANT_TBL *quant_table=cinfo->quant_table[cinfo->comp[cn].quant_tbl_no];
    	JHUFF_TBL  *dc_table=cinfo->dc_huff_table[cinfo->comp[cn].dc_tbl_no];
    	JHUFF_TBL  *ac_table=cinfo->ac_huff_table[cinfo->comp[cn].ac_tbl_no];    
    	//����ÿ��DCT��--'Block'    
    	for(tn=0;tn<cinfo->comp[cn].num_dct_table;tn++) 
    	{
      		DCTVAL tmp=0;
      		DCTVAL *val=dct_table[tn];
      		int i;         		
      		for(i=0;i<DCTSIZE2;i++)val[i]-=128;//Forward-DCT������ƫ��       	      
      		for(i=0;i<64;i++)temp_dct1[i]=val[i];//Forward-DCT����     
      		dsp_fdct_8x8((short *)val,1);     		
      		jutl_zigzag_sort((short*)val);//Z�任
      		//������ 
      		for(i=0;i<DCTSIZE2;i++)val[i]=(DCTVAL)((float)val[i]/(float)quant_table->quantval[i]+0.5);     
      		//��ͬ����ļ���
      		tmp=val[0];
      		val[0]-=cinfo->comp[cn].preval;
      		cinfo->comp[cn].preval=tmp;           		
      		jhuff_encode_one_block(cinfo, val, dc_table, ac_table);//Huffman
    	}
  	}
}
//������ɢ���ұ任
volatile int temp_dct1[64];
void dsp_fdct_8x8(short *dct_data, unsigned num_fdcts)
{
  	const u16 c1=0x2C62,c3=0x25A0;
  	const u16 c5=0x1924,c7=0x08D4;
  	const u16 c0=0xB505,c2=0x29CF;
  	const u16 c6=0x1151;
  	short f0,f1,f2,f3,f4,f5,f6,f7;		
  	int   g0,g1,h0,h1,p0,p1;
  	short r0,r1;
  	int P00,P11,R00,R11;
  	short g2,g3,h2,h3;
  	short q0a,s0a,q0,q1,s0,s1;
  	short Q0,Q1,S0,S1;
  	int   F0,F1,F2,F3,F4,F5,F6,F7;
  	int   F0r,F1r,F2r,F3r,F4r,F5r,F6r,F7r;
  	unsigned i,j;
  	short *dct_io_ptr;
	dct_io_ptr=dct_data;
	for(i=0;i<64;i++)temp_dct1[i]=dct_data[i];
	for(i=0;i<num_fdcts;i++)
	{
		for(j=0;j<8;j++)
		{
			f0=dct_io_ptr[0];
			f1=dct_io_ptr[8];
			f2=dct_io_ptr[16];
			f3=dct_io_ptr[24];
			f4=dct_io_ptr[32];
			f5=dct_io_ptr[40];
			f6=dct_io_ptr[48];
			f7=dct_io_ptr[56];
			g0=f0+f7;h2=f0-f7;
			g1=f1+f6;h3=f1-f6;
			h1=f2+f5;g3=f2-f5;
			h0=f3+f4;g2=f3-f4;
			p0=g0+h0;r0=g0-h0;
			p1=g1+h1;r1=g1-h1;
			q1=g2;s1=h2;
			s0a=h3+g3;q0a=h3-g3;
			s0=(s0a*c0+0x7FFF)>>16;
			q0=(q0a*c0+0x7FFF)>>16;
			P00=p0+p1;P11=p0-p1;
			R11=c6*r1+c2*r0;R00=c6*r0-c2*r1;
			Q1=q1+q0;Q0=q1-q0;
			S1=s1+s0;S0=s1-s0;
			F0=P00;F4=P11;
			F2=R11;F6=R00;
			F1=c7*Q1+c1*S1;F7=c7*S1-c1*Q1;
			F5=c3*Q0+c5*S0;F3=c3*S0-c5*Q0;
			dct_io_ptr[0]=F0;
			dct_io_ptr[8]=F1>>13;
			dct_io_ptr[16]=F2>>13;
			dct_io_ptr[24]=F3>>13;
			dct_io_ptr[32]=F4;
			dct_io_ptr[40]=F5>>13;
			dct_io_ptr[48]=F6>>13;
			dct_io_ptr[56]=F7>>13;
			dct_io_ptr++;
		}
		dct_io_ptr+=56;
	}
	dct_io_ptr=dct_data;
	for(i=0;i<8*num_fdcts;i++)
	{
		f0=dct_io_ptr[0];
		f1=dct_io_ptr[1];
		f2=dct_io_ptr[2];
		f3=dct_io_ptr[3];
		f4=dct_io_ptr[4];
		f5=dct_io_ptr[5];
		f6=dct_io_ptr[6];
		f7=dct_io_ptr[7];
		g0=f0+f7;h2=f0-f7;
		g1=f1+f6;h3=f1-f6;
		h1=f2+f5;g3=f2-f5;
		h0=f3+f4;g2=f3-f4;
		p0=g0+h0;r0=g0-h0;
		p1=g1+h1;r1=g1-h1;
		q1=g2;s1=h2;
		s0a=h3+g3;q0a=h3-g3;
		q0=(q0a*c0+0x7FFF)>>16;
		s0=(s0a*c0+0x7FFF)>>16;
		P00=p0+p1;P11=p0-p1;
		R11=c6*r1+c2*r0;R00=c6*r0-c2*r1;
		Q1=q1+q0;Q0=q1-q0;
		S1=s1+s0;S0=s1-s0;
		F0=P00;F4=P11;
		F2=R11;F6=R00;
		F1=c7*Q1+c1*S1;F7=c7*S1-c1*Q1;
		F5=c3*Q0+c5*S0;F3=c3*S0-c5*Q0;
		F0r=(F0+0x0006)>>3;
		F1r=(F1+0x7FFF)>>16;
		F2r=(F2+0x7FFF)>>16;
		F3r=(F3+0x7FFF)>>16;
		F4r=(F4+0x0004)>>3;
		F5r=(F5+0x7FFF)>>16;
		F6r=(F6+0x7FFF)>>16;
		F7r=(F7+0x7FFF)>>16;
		dct_io_ptr[0]=F0r;
		dct_io_ptr[1]=F1r;
		dct_io_ptr[2]=F2r;
		dct_io_ptr[3]=F3r;
		dct_io_ptr[4]=F4r;
		dct_io_ptr[5]=F5r;
		dct_io_ptr[6]=F6r;
		dct_io_ptr[7]=F7r;
		dct_io_ptr+=8;
	}
	for(i=0;i<64;i++)temp_dct1[i]=dct_data[i];
	return;
}


/*
//������ɢ���ұ任
volatile int temp_dct1[64];
void dsp_fdct_8x8(short *dct_data, unsigned num_fdcts)
{
  	const u16 c1=0x2C62,c3=0x25A0;
  	const u16 c5=0x1924,c7=0x08D4;
  	const u16 c0=0xB505,c2=0x29CF;
  	const u16 c6=0x1151;
  	short f0,f1,f2,f3,f4,f5,f6,f7;		
  	int   g0,g1,h0,h1,p0,p1;
  	short r0,r1;
  	int P0,P1,R0,R1;
  	short g2,g3,h2,h3;
  	short q0a,s0a,q0,q1,s0,s1;
  	short Q0,Q1,S0,S1;
  	int   F0,F1,F2,F3,F4,F5,F6,F7;
  	int   F0r,F1r,F2r,F3r,F4r,F5r,F6r,F7r;
  	unsigned i,j;
  	short *dct_io_ptr;
	dct_io_ptr=dct_data;
	for(i=0;i<64;i++)temp_dct1[i]=dct_data[i];
	for(i=0;i<num_fdcts;i++)
	{
		for(j=0;j<8;j++)
		{
			f0=dct_io_ptr[0];
			f1=dct_io_ptr[8];
			f2=dct_io_ptr[16];
			f3=dct_io_ptr[24];
			f4=dct_io_ptr[32];
			f5=dct_io_ptr[40];
			f6=dct_io_ptr[48];
			f7=dct_io_ptr[56];
			g0=f0+f7;h2=f0-f7;
			g1=f1+f6;h3=f1-f6;
			h1=f2+f5;g3=f2-f5;
			h0=f3+f4;g2=f3-f4;
			p0=g0+h0;r0=g0-h0;
			p1=g1+h1;r1=g1-h1;
			q1=g2;s1=h2;
			s0a=h3+g3;q0a=h3-g3;
			s0=(s0a*c0+0x7FFF)>>16;
			q0=(q0a*c0+0x7FFF)>>16;
			P0=p0+p1;P1=p0-p1;
			R1=c6*r1+c2*r0;R0=c6*r0-c2*r1;
			Q1=q1+q0;Q0=q1-q0;
			S1=s1+s0;S0=s1-s0;
			F0=P0;F4=P1;
			F2=R1;F6=R0;
			F1=c7*Q1+c1*S1;F7=c7*S1-c1*Q1;
			F5=c3*Q0+c5*S0;F3=c3*S0-c5*Q0;
			dct_io_ptr[0]=F0;
			dct_io_ptr[8]=F1>>13;
			dct_io_ptr[16]=F2>>13;
			dct_io_ptr[24]=F3>>13;
			dct_io_ptr[32]=F4;
			dct_io_ptr[40]=F5>>13;
			dct_io_ptr[48]=F6>>13;
			dct_io_ptr[56]=F7>>13;
			dct_io_ptr++;
		}
		dct_io_ptr+=56;
	}
	dct_io_ptr=dct_data;
	for(i=0;i<8*num_fdcts;i++)
	{
		f0=dct_io_ptr[0];
		f1=dct_io_ptr[1];
		f2=dct_io_ptr[2];
		f3=dct_io_ptr[3];
		f4=dct_io_ptr[4];
		f5=dct_io_ptr[5];
		f6=dct_io_ptr[6];
		f7=dct_io_ptr[7];
		g0=f0+f7;h2=f0-f7;
		g1=f1+f6;h3=f1-f6;
		h1=f2+f5;g3=f2-f5;
		h0=f3+f4;g2=f3-f4;
		p0=g0+h0;r0=g0-h0;
		p1=g1+h1;r1=g1-h1;
		q1=g2;s1=h2;
		s0a=h3+g3;q0a=h3-g3;
		q0=(q0a*c0+0x7FFF)>>16;
		s0=(s0a*c0+0x7FFF)>>16;
		P0=p0+p1;P1=p0-p1;
		R1=c6*r1+c2*r0;R0=c6*r0-c2*r1;
		Q1=q1+q0;Q0=q1-q0;
		S1=s1+s0;S0=s1-s0;
		F0=P0;F4=P1;
		F2=R1;F6=R0;
		F1=c7*Q1+c1*S1;F7=c7*S1-c1*Q1;
		F5=c3*Q0+c5*S0;F3=c3*S0-c5*Q0;
		F0r=(F0+0x0006)>>3;
		F1r=(F1+0x7FFF)>>16;
		F2r=(F2+0x7FFF)>>16;
		F3r=(F3+0x7FFF)>>16;
		F4r=(F4+0x0004)>>3;
		F5r=(F5+0x7FFF)>>16;
		F6r=(F6+0x7FFF)>>16;
		F7r=(F7+0x7FFF)>>16;
		dct_io_ptr[0]=F0r;
		dct_io_ptr[1]=F1r;
		dct_io_ptr[2]=F2r;
		dct_io_ptr[3]=F3r;
		dct_io_ptr[4]=F4r;
		dct_io_ptr[5]=F5r;
		dct_io_ptr[6]=F6r;
		dct_io_ptr[7]=F7r;
		dct_io_ptr+=8;
	}
	for(i=0;i<64;i++)temp_dct1[i]=dct_data[i];
	return;
}
*/


//huffman����
void jhuff_encode_one_block(jpeg_compress_info *cinfo,DCTBLOCK src,JHUFF_TBL *dc_tbl,JHUFF_TBL *ac_tbl) 
{
  	int diff,ssss,rrrr,code;
  	int rs,k;  
  	//ֱ����������
  	diff=src[0];
  	ssss=bit_num(diff);
  	jhuff_write_bits(cinfo,dc_tbl->ecode[ssss],dc_tbl->esize[ssss]);
  	if (diff<0)diff-=1;
  	jhuff_write_bits(cinfo,diff,ssss);  
  	//������������
  	rrrr=0;
  	for(k=1;k<DCTSIZE2;k++)
  	{
    	if(src[k]==0)
		{
      		rrrr++;
      		continue;
    	}
		else
		{
      		while(rrrr>15)//write EHUFSI(0xF0) bits of EHUFCO(0xF0)
			{        		 
        		jhuff_write_bits(cinfo, ac_tbl->ecode[0xF0], ac_tbl->esize[0xF0]);
        		rrrr-=16;
      		}
			ssss=bit_num(src[k]);
			rs=((rrrr<<4)|ssss);			
			jhuff_write_bits(cinfo,ac_tbl->ecode[rs],ac_tbl->esize[rs]);//write EHUFSI(rs) bits of EHUFCO(rs)
			code=src[k];
			if(code<0)code-=1;			
			jhuff_write_bits(cinfo,code,ssss);//write ssss bits of rs
			rrrr=0;
		}
	}
  	if(rrrr!=0)jhuff_write_bits(cinfo,ac_tbl->ecode[0x00],ac_tbl->esize[0x00]);//write EOB
}
//��ɻ�������ʣ�����ݵ�ѹ������,��д�ļ�β������ļ�
void jpeg_finish_compress(jpeg_compress_info *cinfo) 
{
  	int i,startline,rowlen;
  	JSAMPLE *data;
  	//if(cinfo->state!=JC_START)exit(1);
  	//���cinfo->inbuf�����ݵ�ѹ��
  	if(cinfo->next_line%cinfo->inbuf_height!=0)
	{
		rowlen=cinfo->inbuf_width*cinfo->num_comp*sizeof(JSAMPLE);
		startline=cinfo->next_line%cinfo->inbuf_height;
		data=&cinfo->inbuf[(startline-1)*rowlen];
		if(startline!=0)
			for(i=startline;i<cinfo->inbuf_height;i++)
				memcpy(&cinfo->inbuf[i*rowlen],data,rowlen);
    	jint_process_rows(cinfo);  
  	}
  	jhuff_flush_buffer(cinfo); 
  	jmkr_write_end(cinfo);//���JPEG�ļ�β  	
}
//���/�ͷŻ�����
void jhuff_flush_buffer(jpeg_compress_info *cinfo) 
{
  	int i=3;  
  	while(bits_in_buffer>8)
	{
		jutl_write_byte(buffer.c[i],cinfo->output,cinfo  );
		if(buffer.c[i]==0xFF)jutl_write_byte(0x00,cinfo->output,cinfo  );		
		i--;
		bits_in_buffer-=8;
	}
	if(bits_in_buffer>0)
	{
		buffer.c[i]|=((u8)(0xFF)>>bits_in_buffer);
		jutl_write_byte(buffer.c[i],cinfo->output,cinfo  );
		if(buffer.c[i]==0xFF)jutl_write_byte(0x00,cinfo->output,cinfo  );		
	}
	buffer.all=0;
	bits_in_buffer=0;
}

int bit_num(int a) 
{
  	int bit=0;
  	if(a==0)return 0;
  	a=a>0?a:-a;
  	while((1<<bit)<=a)bit++;
  	return bit;
}
//д����
void jhuff_write_bits(jpeg_compress_info *cinfo,u32 data,int len) 
{
  	u32 tdata; 
	while(len>0)
	{
		tdata=(data<<(32-len));
		buffer.all|=((tdata>>bits_in_buffer));
		bits_in_buffer+=len;		
		if(bits_in_buffer>=32)//�������
		{
			int i;			
			for(i=3;i>=0;i--)//����Ϊ���ģʽ
			{
				jutl_write_byte(buffer.c[i],cinfo->output,cinfo  );
				if(buffer.c[i]==0xFF)
				jutl_write_byte(0x00,cinfo->output,cinfo  );
			}
			buffer.all=0;
			len=bits_in_buffer-32;
			bits_in_buffer=0;
		}
		else len=0;
	}
}














