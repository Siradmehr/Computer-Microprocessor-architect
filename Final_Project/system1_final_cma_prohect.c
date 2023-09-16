#include <reg51.h>
#include <string.h>
#include <absacc.h>

char Temp;           

char xdata PortA _at_ 0x2000;  //Port A of 8255                  
char xdata PortB _at_ 0x2001;  //Port B of 8255
char xdata PortC _at_ 0x2002;  //Port C of 8255
char xdata CWR _at_ 0x2003;    //CWR of 8255

int ramLocation = 0x1000;          //External RAM space from 0x2000 to 0x3000 
sbit RD_AD = P1^0;           //Read Pin of ADC 0804
sbit WR_AD = P1^1;           //Write Pin of ADC 0804
sbit INTR_AD = P1^7;         //INTR Pin of ADC 0804
sbit WR_SYS1 = P3^6;          //Write select System 1
sbit RD_SYS1 = P3^7;          //Read select System 1
// sbit ALE_1 = ALE;

sbit A0 = P0^0;               //Using of A[0:1] for Port Select 8255
sbit A1 = P0^1;

void serial_init()     
{
	TMOD = 0x20; //use timer1
	
	PCON = 0x80; //SCON = 1; Baud Rate Doubler
	TH1 = 0xFD;  //baud-rate = 19200;
	
	SCON = 0x50; 
	TR1 = 1;
}

void serial_transmit(char val) 
{
	SBUF = val;
	while (TI == 0);
	TI = 0;
}

void delay(int k)         
{
	int i,j;
	for (i = 0; i <= k; i++)
		for (j = 0; j < 255; j++);
}

void save2RAM(char Val)
{
	if (ramLocation >= 0x3000)
		ramLocation = 0x2000;    //reset the RAM location
	
	XBYTE[ramLocation] = Val;    //save the Temperature into External RAM
	ramLocation = ramLocation + 1;    //Increase the RAM location
}

void Set8255 ()
{
	A0 = 1;  A1 = 1;
	RD_SYS1 = 1;
	WR_SYS1 = 0;
	CWR = 0x9B; //Port A as input Port from ADC
	
	/*A0 = 0;
	A1 = 0;
	*/
	delay(30);
	//So we have Temperature in Port A of 8255
	
	
	/*
	A0 = 1;
	A1 = 1;
	RD_SYS1 = 1;
	WR_SYS1 = 0;
	CWR = 0x8B;  //Port A as output Port to Data Bus
	*/
	
	//Set the Value of Port A on Data Bus 
	A0 = 0;  A1 = 0;
	RD_SYS1 = 0;
	WR_SYS1 = 1;
	delay(30);
	//So we have Port A on Data Bus
	
}

char adc()
{
	char TempValue;
	//Applying appropirate settings for ADC to start and convert the analog signal
	WR_AD = 0;
	RD_AD = 1;
	WR_AD = 1;
	while (INTR_AD == 1);      //Wait till conversion is complete
	RD_AD = 0;                 //Read from ADC 
	
	Set8255();         //Apply the 8255 settings
	
	TempValue = PortA;          //Read the Temperature value
	//TempValue = P0;
	
	return TempValue;	
}



void main()
{
	serial_init();                       //Initialize the Serial communicatin
	while (1)
	{
		Temp = adc();         //Read Analog to Digital converted Temperature
		delay(100);                      //Short Delay for relaxation!
		save2RAM(Temp);          //Save the Temperature into External RAM
		delay(100);                      //Short Delay for relaxation!
		serial_transmit(Temp);        //Send the Temperature serially to System 2
	}
	
}
