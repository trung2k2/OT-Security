#include "arp.h"

extern const uint8_t macaddr[6];
extern uint8_t ip[4];
extern char debug_string[60];
extern int led;
uint8_t macfake[6]={0x01,0x02,0x03,0x04,0x05,0x06};
struct
{
   uint8_t ip[4];
   uint8_t mac[6];
}ARP_table_defend[5];
extern struct
{
   uint8_t ip[4];
   uint8_t mac[6];
}ARP_table_static[5];
uint8_t ARP_table_index_defend=1;
int check =0;
int a[4]={10,10,10,10};
int k =1;
uint32_t ARP_clear_time;

void ARP_clear_table(void)
{
  uint8_t *end;
  uint8_t *start;
     start = (uint8_t *)&ARP_table_defend;
     end =  start + sizeof(ARP_table_defend);
     for(;start<end;start++)
        *start=0;
}
int8_t ARP_table_checkIP_defend(uint8_t *ip_check)  //kiem tra xem co ton tai ip trong bang chua
{
 int i;
 for(i=0;i<8;i++)
 {
   if(ip_check[0] == ARP_table_defend[i].ip[0] && ip_check[1] == ARP_table_defend[i].ip[1] && ip_check[2] == ARP_table_defend[i].ip[2] && ip_check[3] == ARP_table_defend[i].ip[3] )
      return i+1; //tra ve vi tri cua ip trong table
 }
 return -1;
}
void ARP_table_setIP_defend(uint8_t *ip_set)
{
  if(ARP_table_checkIP_defend(ip_set) == -1 ) //neu chua ton tai IP trong table
  {
    memcpy(ARP_table_defend[ARP_table_index_defend].ip,ip_set,4);

//		UART_putString("Check truoc khi luu vao bang\r\n");
//			sprintf(debug_string,"---Dia chi thiet bi Ip %i.%i.%i.%i = Mac %02X:%02X:%02X:%02X:%02X:%02X\r\n",ARP_table_defend[ARP_table_index_defend].ip[0],
//		ARP_table_defend[ARP_table_index_defend].ip[1],
//			ARP_table_defend[ARP_table_index_defend].ip[2],	ARP_table_defend[ARP_table_index_defend].ip[3],ARP_table_defend[ARP_table_index_defend].mac[0],ARP_table_defend[ARP_table_index_defend].mac[1],
//		ARP_table_defend[ARP_table_index_defend].mac[2],
//			ARP_table_defend[ARP_table_index_defend].mac[3],ARP_table_defend[ARP_table_index_defend].mac[4],	ARP_table_defend[ARP_table_index_defend].mac[5]);
//			UART_putString(debug_string);
    ARP_table_index_defend++;if(ARP_table_index_defend==8)ARP_table_index_defend=0; //neu so luong ip vuot qua muc cho phep thi quay lai
  }
  //else
  //  UART_putString("IP da ton tai trong bang\r\n");
}


void ARP_read_packet(uint8_t *ARP_Buff,uint16_t len)
{
	check =0;
//  ARP_struct ARP_reponse;
  //tao 2 array de luu IP và MAC cua nguon
  uint8_t MAC_source[6];
	uint8_t MAC_target[6];
  uint8_t IP_source[4];
  uint8_t IP_target[4];
  memcpy(MAC_source,(uint8_t *)&ARP_Buff[6],6);  //lay MAC cua nguon gui packet
	memcpy(MAC_target,(uint8_t *)&ARP_Buff[0],6);  //lay MAC cua nguon nhan packet
  memcpy(IP_source ,(uint8_t *)&ARP_Buff[28],4); //lay IP cua nguon gui packet
  memcpy(IP_target ,(uint8_t *)&ARP_Buff[38],4); //lay IP cua nguon nhan packet
//for(int i = 0;i<4;i++)
//	{
//		if(memcmp(IP_source,ARP_table[i].ip,4)==0)
//		{
//		}
//	}
  //kiem tra xem co trung voi IP cua minh khong
//	sprintf(debug_string,"\r\nDia chi thiet bi 1 Ip %i.%i.%i.%i = Mac %02X:%02X:%02X:%02X:%02X:%02X\r\n",IP_source[0],IP_source[1],
//	IP_source[2],	IP_source[3],MAC_source[0],MAC_source[1],MAC_source[2],MAC_source[3],
//	MAC_source[4],MAC_source[5]);
//	UART_putString(debug_string);
//	sprintf(debug_string,"Dia chi thiet bi 2 Ip %i.%i.%i.%i = Mac %02X:%02X:%02X:%02X:%02X:%02X\r\n",IP_target[0],IP_target[1],
//	IP_target[2],	IP_target[3],MAC_target[0],MAC_target[1],MAC_target[2],MAC_target[3],
//	MAC_target[4],MAC_target[5]);
//	UART_putString(debug_string);
		for(int i =0; i<5;i++)
		{ 
			if(memcmp(IP_target,ARP_table_static[i].ip,4)==0)
			{
				for(int j=0; j<5;j++)
				{if(memcmp(MAC_source,ARP_table_static[j].mac,6)==0)
				{check=1;
//					UART_putString("Trung dia chi dich, dia chi nguon\r\n");

				}}
			if (check == 0){ //khong ton tai nguon trong bang
		UART_putString("\r\nCo thiet bi la xam nhap vao he thong\r\n");
				ARP_table_setIP_defend(IP_target);
		led = 0;
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_SET);
		sprintf(debug_string,"Dia chi thiet bi la Ip %i.%i.%i.%i = Mac %02X:%02X:%02X:%02X:%02X:%02X\r\n",IP_source[0],IP_source[1],
		IP_source[2],	IP_source[3],MAC_source[0],MAC_source[1],MAC_source[2],MAC_source[3],
		MAC_source[4],MAC_source[5]);
		UART_putString(debug_string);	
			
			sprintf(debug_string,"So thiet bi bi tan cong la: %i\r\n", ARP_table_index_defend-1);
						UART_putString(debug_string);
					for(int k=1;k<ARP_table_index_defend;k++)
		{
			sprintf(debug_string,"Dia chi thiet bi %i Ip %i.%i.%i.%i\r\n",k,ARP_table_defend[k].ip[0],ARP_table_defend[k].ip[1],
			ARP_table_defend[k].ip[2],	ARP_table_defend[k].ip[3]);
			UART_putString(debug_string);}
}
		}
			}
		}



//--------------------------------------------------------------------------
//End file

