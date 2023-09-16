#include <reg51.h>
#include <string.h>

char Temp;              

char xdata PortA _at_ 0x2000;  //Port A of 8255                  
char xdata PortB _at_ 0x2001;  //Port B of 8255
char xdata PortC _at_ 0x2002;  //Port C of 8255
char xdata CWR _at_ 0x2003;    //CWR of 8255

char TemperatureVal[100];             //Interal 100 Byte RAM space of 8051 of System 2
int ramLocation = 0;                       //Internal RAM space location counter (100 Byte usage)

void serial_init()      
{
	TMOD = 0x20; //use timer1
	
	PCON = 0x80;
	TH1 = 0xFD;  //baud-rate = 19200;
	
	SCON = 0x50; 
	TR1 = 1;
}

char serial_receive()    
{
	/*
	This function receives a char (val) serially from System 1 by applying appropirate setting of RxD 
	*/
	unsigned char val;
	while(RI == 0);
	val = SBUF;
	RI = 0;
	return val;	
}

void delay(int k)         
{
	/*
	This function generates a delay (bigger values of input corresponds to longer delay)
	*/
	int i,j;
	for (i = 0; i <= k; i++)
		for (j = 0; j < 255; j++);
}

void Lcd_CmdWrite(char Command_Val)
{ 
   PortA = Command_Val;     //Command on Port A

   //Command Control Signals
   
   //Port B = 0100B  = 0x04 => Enable: 1, RW:0, RS:0  (Command Mode)
   PortB  = 0x04;
   delay(100);        
   
   //Port B = 0000B  = 0x00 => Enable: 0, RW:0, RS:0  (Command Mode)
   PortB  = 0x00;
   delay(100);
}

void Lcd_DataWrite(char Data_Val) 
{ 
   PortA = Data_Val;
 
   //Data Control Signals
   
   //Port B = 0101B  = 0x05 => Enable: 1, RW:0, RS:1  (Data Mode)
   PortB  = 0x05;
   delay(100);
 
   //Port B = 0001B  = 0x01 => Enable: 0, RW:0, RS:1  (Data Mode)
   PortB  = 0x01;
   delay(100);   
}

void save2RAM(char val)
{
	if (ramLocation >= 100)
		ramLocation = 0;
	
	TemperatureVal[ramLocation] = val;           //Save the Temperature value into Interal RAM space 
	ramLocation++;
}

void main()
{
	char START_phrase[] = "START:";  //Start phrase to be sent
	int i = 0;
	char j,a[]={"The temputure is :"};
	serial_init();            //Initialize the Serial communicatin
	while(1)
	{
		Temp = serial_receive();             //Receive Temperature from System1
		save2RAM(Temp);             //Save the Temperature value into Internal RAM
		
		Lcd_CmdWrite(0x38);   //Initialize LCD IN 2x16 MATRIX
		Lcd_CmdWrite(0x0e);   //Display ON, Cursor ON
		Lcd_CmdWrite(0x01);   //Clear LCD
		Lcd_CmdWrite(0x80);   //Sets line & Position 
		
		//Display "START:" on LCD
		for (i = 0; i < 6; i++)   
	    { 
		Lcd_DataWrite(START_phrase[i]);  
	    }
		Lcd_CmdWrite(0xC0);       //Sets the second line  
		for(j=0;a[i]!=0;j++)
		{
        Lcd_DataWrite(a[i]);
		}
		Lcd_DataWrite(Temp);   //Display Temperature Value on LCD
	}
}
