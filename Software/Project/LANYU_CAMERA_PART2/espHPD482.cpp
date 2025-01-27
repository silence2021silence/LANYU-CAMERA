/*
HPD482驱动程序v3.6 for ESP32/ESP32C3 Arduino
///////////////////////////////////////////////////////////////////////////////////////////////
作者：有立diy
淘宝店铺：有立diy
淘宝链接：https://shop587795831.taobao.com/
原创声明：本例程为热敏打印机模块HPD482配套例程，仅供参考，不得用于商业等用途，其他后果概不负责。
		  如需使用，注明出处。
///////////////////////////////////////////////////////////////////////////////////////////////
*/
#include "espHPD482.h"

//打印机微秒延时
void HPD482::pdelay_us(u16 tus)
{
	delayMicroseconds(tus);
}
//打印机毫秒延时
void HPD482::pdelay_ms(u16 tms)
{
	if(tms==0);
	else delay(tms);
}
//***********************************************************************
//步进电机正转一个序列
#define MOTOR_STEPF		switch((motor_step_i++)%4)\
						{\
							case 0:MOTOR_STEP1;break;\
							case 1:MOTOR_STEP2;break;\
							case 2:MOTOR_STEP3;break;\
							case 3:MOTOR_STEP4;break;\
						}
//步进电机反转一个序列
#define MOTOR_STEPNF 	switch((motor_step_i++)%4)\
						{\
							case 0:MOTOR_STEP4;break;\
							case 1:MOTOR_STEP3;break;\
							case 2:MOTOR_STEP2;break;\
							case 3:MOTOR_STEP1;break;\
						}

/*
步进电机驱动函数
输入参数：
	cont:旋转次数（转一次进纸0.125mm），范围：[1,255]
	dir:方向（0：正转；1：反转）
*/
void HPD482::Motor_Run(u8 cont,u8 dir)
{
	static u8 motor_dir_x=0;
	if(motor_dir_x!=dir)//方向翻转
	{
		if(motor_step_i%2==0)motor_step_i+=2;
		motor_dir_x=dir;//存储此次方向
	}
	if(dir==0)//正转
	{
		while(cont--)
		{
			MOTOR_STEPF;pdelay_us(Motorspeed+ALIGN_DELAY);
			pdelay_ms(SLOW_DELAY);
			MOTOR_STEPF;pdelay_us(Motorspeed+ALIGN_DELAY);
			pdelay_ms(SLOW_DELAY);
		}
	}
	else//反转
	{
		while(cont--)
		{
			MOTOR_STEPNF;pdelay_us(Motorspeed+ALIGN_DELAY);
			pdelay_ms(SLOW_DELAY);
			MOTOR_STEPNF;pdelay_us(Motorspeed+ALIGN_DELAY);
			pdelay_ms(SLOW_DELAY);
		}
	}
	MOTOR_STOP;//停转
}

/*
步进电机退纸
输入参数：
	dot：退纸点数,范围：[1,65025]
*/
void HPD482::Motor_Back(u16 dot)
{
	u8 mtim,ddot;
	if(dot>255)
	{
		mtim=dot/255;
		ddot=dot%255;
		while(mtim--)Motor_Run(255,1);
	}
	else ddot=dot;
	Motor_Run(ddot+7,1);
	Motor_Run(7,0);//排除行程误差
}

/*
设置打印颜色深度
输入参数：
	deepcolor：打印深度，范围为0-100的整数
注：数值越大打印颜色越深，打印速度越慢，同时越容易因高温损坏打印模块！因此数值不宜太大。
*/
void HPD482::Print_SetDeep(u8 deepcolor=0)
{
	if(deepcolor>100)deepcolor=100;
	Ptdeepcolor=deepcolor*50;
	Motorspeed=(2*(HT_minhttim+Ptdeepcolor)+Leftdeep);
	if(Motorspeed<2000)Motorspeed=2000;
}

/*
设置左半部分打印颜色深度
*/
void HPD482::Print_SetLfDeep(u8 Leftdeepcolor=0)
{
	Leftdeep=Leftdeepcolor*10;
	Motorspeed=(2*(HT_minhttim+Ptdeepcolor)+Leftdeep);
	if(Motorspeed<2000)Motorspeed=2000;
}

/*
停止打印
*/
void HPD482::Print_Stop()
{
	HEAT_STOP;
	MOTOR_STOP;
}

/*
打印机初始化
*/
HPD482::HPD482(void)
{
	pinMode(MNB_pin,OUTPUT);
	pinMode(STBB_pin,OUTPUT);
	pinMode(STBA_pin,OUTPUT);
	HEAT_STOP;
	pinMode(MB_pin,OUTPUT);
	pinMode(MNA_pin,OUTPUT);
	pinMode(MA_pin,OUTPUT);
	MOTOR_STOP;
	pinMode(LAT_pin,OUTPUT);
	pinMode(DI_pin,OUTPUT);
	pinMode(CLK_pin,OUTPUT);
	Print_SetLfDeep(10);
	Print_SetDeep(10);//设置打印深度
}

/*
发送一字节数据
输入参数：
	onebytedata:一字节数据
返回参数：脉冲个数，即要加热的点的个数
*/
u8 HPD482::Send_OneByte(u8 onebytedata)
{
	u8 i,htnum=0;
	for(i=0;i<8;i++)
	{
		CLK(LOW);
		if(onebytedata&0x80){DI(HIGH);htnum++;}
		else DI(LOW);
		CLK(HIGH);
		onebytedata<<=1;
	}
	CLK(LOW);
	return htnum;
}

/*
控制加热
输入参数：
	tim:加热时间，单位：us
	ctr:加热控制线。1：STBA加热；2：STBB加热
*/
void HPD482::Print_Control(u16 tim,u8 ctr)
{
	switch(ctr)
	{
		case 1:STBA(HIGH);pdelay_us(tim+Leftdeep+Ptdeepcolor);STBA(LOW);break;
		case 2:STBB(HIGH);pdelay_us(tim+Ptdeepcolor);STBB(LOW);break;
	}
}

/*
打印一行像素点阵
输入参数：
	pixlindat:48字节数据
*/
void HPD482::Print_OneLine(u8 *pixlindat)
{
	u8 i,k,cont;
	u16 a[2];
	for(k=0;k<2;k++)
	{
		cont=0;
		for(i=0;i<24;i++)cont+=Send_OneByte(*(pixlindat++));//查待加热的点数
		if(cont<HT_mindots)a[k]=HT_minhttim;//加热时间下限，保证每个点都能打印上
		else if(cont>HT_maxdots)a[k]=HT_maxhttim;//加热时间上限
		else a[k]=cont*HT_kdottim+HT_bdottim;//平均加热时间
	}
	LAT(HIGH);
	LAT(LOW);//拉低锁存端
	pdelay_us(3);
	LAT(HIGH);
	Print_Control(a[0],1);//加热前半部分
	Print_Control(a[1],2);//加热后半部分
	HEAT_STOP;//停止加热
}

/*
打印虚线
输入参数：
	linesnum:打印几条虚线
	dist:线间距，单位：mm，最大值31
*/
void HPD482::Print_Lines(u8 linesnum,u8 dist)
{
	u8 aa[48],i;
	for(i=0;i<48;)
	{
		aa[i++]=0xFF;
		aa[i++]=0x00;
	}
	while(linesnum--)
	{
		i=8*dist;
		MOTOR_STEPF;
		pdelay_ms(SLOW_DELAY);
		Print_OneLine(aa);
		MOTOR_STEPF;pdelay_us(Motorspeed+ALIGN_DELAY);
		pdelay_ms(SLOW_DELAY);
		while(--i)
		{
			MOTOR_STEPF;pdelay_us(Motorspeed+ALIGN_DELAY);
			pdelay_ms(SLOW_DELAY);
			MOTOR_STEPF;pdelay_us(Motorspeed+ALIGN_DELAY);
			pdelay_ms(SLOW_DELAY);
		}
	}
	Print_Stop();
}

#if IS_PRINT_PICT
/*
打印黑白单色图片
图片应为按行取模
输入参数：
	startx:打印起始位置，取值范围为0到47的整数
	picdata:纯图片数据
	widthpix:图片像素宽度，宽度应<=384，>384的部分将被舍弃
	heightpix:图片像素高度，最大65536
*/
void HPD482::Print_Picture(u8 startx,u8 *picdata,u16 widthpix,u16 heightpix)
{
	u16 iline;//列变量
	u16 pixlin;//行变量
	u8 widthbyte,ptbyte;//宽度字节/实际打印字节
	u8 onepixline[48];//存放一行像素数据

	widthbyte=widthpix/8+(widthpix%8?1:0);//计算图片宽多少字节
	if(startx>47)startx=47;
	if((startx+widthbyte)>48)ptbyte=48-startx;//如果宽度超过48字节，将图片裁剪到48字节
	else ptbyte=widthbyte;

	for(pixlin=0;pixlin<heightpix;pixlin++)//行扫描并打印一行
	{
		//走一步长
		MOTOR_STEPF;pdelay_us(Motorspeed+ALIGN_DELAY);
		pdelay_ms(SLOW_DELAY);
		MOTOR_STEPF;
		pdelay_ms(SLOW_DELAY);

		for(iline=0;iline<startx;iline++)onepixline[iline]=0x00;//存入空数据
		for(iline=0;iline<ptbyte;iline++)
		{
			onepixline[iline+startx]=*(picdata++);
		}
		if(ptbyte!=widthbyte)picdata+=(startx+widthbyte-48);//跳过裁剪掉的部分
		else for(iline=0;iline<48-startx-ptbyte;iline++)onepixline[iline+startx+ptbyte]=0x00;//存入空数据

		Print_OneLine(onepixline);
	}
}
/*
打印使用Img2Lcd软件取模的图片
Img2Lcd软件图片取模方式：输出数组+水平扫描+单色+包含图像头数据+高位在前
图像头数据格式（共6字节）：扫描方式，灰度值，图片宽（高8位），图片宽（低8位），图片高（高8位），图片高（低8位）
输入参数：
	x:打印起始位置，取值范围为0到47的整数
	pic:图片数据
*/
void HPD482::Print_Img2Lcd(u8 x,u8 *pic)
{
	u16 width,height;//图片像素宽高
	u8 *imag;

	// imag=pic+6;//跳过头数据的图片数组下标
	// width=(*(pic+2)<<8)+*(pic+3);//根据头数据计算图片宽度
	// height=(*(pic+4)<<8)+*(pic+5);//根据头数据计算图片高度
	// Print_Picture(x,imag,width,height);
  Print_Picture(x,pic,320,240);
	
	Print_Stop();
}
#endif












