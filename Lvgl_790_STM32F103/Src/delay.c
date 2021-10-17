#include "delay.h"
#include "cmsis_os.h"

static unsigned char  fac_us=0;							//us延时倍乘数
static unsigned short fac_ms=0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数

void delay_init()
{
    fac_us=SystemCoreClock/1000000;
    fac_ms=1000/configTICK_RATE_HZ;
}

//延时nus
//nus为要延时的us数.
void delay_us(unsigned long nus) 
{
	unsigned long ticks;
	unsigned long told,tnow,tcnt=0;
	unsigned long reload=SysTick->LOAD;			//LOAD的值
	ticks=nus*fac_us; 							//需要的节拍数
	tcnt=0;
	told=SysTick->VAL;        					//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;
		if(tnow!=told)
		{
			if(tnow<told)tcnt+=told-tnow;		//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;
			told=tnow;
			if(tcnt>=ticks)break;				//时间超过/等于要延迟的时间,则退出.
		}
	};
}
//延时nms
//nms:要延时的ms数
void delay_ms(unsigned short nms)
{
	if(osKernelRunning())	//如果OS已经在跑了,并且不是在中断里面(中断里面不能任务调度)
	{
		if(nms>=fac_ms)							//延时的时间大于OS的最少时间周期
		{
   			vTaskDelay(nms/fac_ms);		//OS延时
		}
		nms%=fac_ms;							//OS已经无法提供这么小的延时了,采用普通方式延时
	}
	delay_us((unsigned long)(nms*1000));					//普通方式延时
}

//不引起任务调度
void delay_xms(unsigned short nms)
{
    unsigned short i;
    for(i=nms; i>0; i--)
    {
        delay_us(1000);
    }
}


