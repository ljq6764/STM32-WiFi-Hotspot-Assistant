#include "PACK.h"
#include "touch.h"




static int flag1=1;					//Ò³Î»±ê¼Ç

TOUCH_TypeDef texttouch;


void one()
{
	while(1)
	{
		if(texttouch.xval<240)
		{
			flag1=2;
		}
		if(texttouch.xval>240)
		{
			flag1=3;
		}
		if(flag1 !=1)
		{
			break;
		}
	}
	return;
}

void two(){}

void three()
{
	while(1)
	{
		if(texttouch.xval<240&texttouch.yval<160)
		{
			flag1=4;
		}
		if(texttouch.xval>240&texttouch.yval<160)
		{
			flag1=5;
		}
		if(texttouch.xval<240&texttouch.yval>160)
		{
			flag1=6;
		}
		if(texttouch.xval>240&texttouch.yval>160)
		{
			flag1=7;
		}
		if(flag1!=3)
		{
			break;
		}
	}
	return;
}

void four(){}
void five(){}
void six(){}
void seven(){}

void zhuye()
		{
			while(1)
			{
				if(0 == Touch_Scan(&texttouch,0))
					switch(flag1)
					{
						case 1:one();break;
						case 2:two();break;
						case 3:three();break;
						case 4:four();break;
						case 5:five();break;
						case 6:six();break;
						case 7:seven();break;
					}
			}	
		}


