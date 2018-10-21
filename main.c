/*
 * Dosya    : main.c
 * Yazar    : Burak Yoruk
 * Uygulama : SHT11 Sensor Uygulamasi
 * Surum    : v1.0
 * Tarih    : 04.04.2018
 * MCU      : STM32F10x
 * Kristal  : 8Mhz (MCU FOSC 72Mhz)
 * Kit      : EXS03
 * Aciklama : SHT1 sensorlerini okuyarak LCD ekranda sicaklik ve nem
 *            bilgisi gosterir.
 * Uyarı    : Keils uVision v5.25 ile derlenmiştir.
 *            EXS03 Kullanıcıları için;
 *            - Sensörü geliştirme kitine takarken besleme voltajının doğru
 *              olduğundan emin olunuz. Aksi halde sensör zarar görebilir.
 *						- SWDIP1 üzerindeki tüm anahtarların kapalı olduğundan emin olun.
 *					-2's complement değer okumakta
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stmio.h"	//gpio, delay, lcd
#include "stdio.h"


//Data pins in the bus are labeled DQ and the strobe pin is labeled DQS
//#define DQ_PIN			9	
//#define DQ_PIN_WRITE(x) ((x) ? (GPIO_SetBits(DQ_GPIO,(1<<DQ_PIN)))  : (GPIO_ResetBits(DQ_GPIO,(1<<DQ_PIN))) );
//#define DQ_PIN_READ 	((GPIO_ReadInputDataBit(DQ_GPIO,(1<<DQ_PIN)))&0x01)
/*	ROM Komutları
Convert T (44h) 68		:Sıcaklık ölçme işlemini başlatan komuttur. Tek ölçüm yapılır bit çözünürlüğüne göre 
								azami 750 ms içinde ölçüm tamamlanır.
Write Scratchpad(4Eh)78	: Bu komut ile DS18B20'nin hazfızasına yazmak için izin alınır. 
								DS18B20 içerisinde 3 byte'a yazma yapılır bunlar TH, TL ve Konfigürasyon kaydedicileridir. Veriler LSB ilk gönderilmelidir.
Read Scratchpad (BEh)190: Bu komut ile DS18B20 hafızasından okuma için izin alınır. Bu komut gönderildikten 
								sonra DSB18B20 8byte(Hafızası) + CRC(Hata denetim verisi) olmak üzere toplam 9 byte veri okunur.
Copy Scratchpad (48h)72 : Bu komut ile geçici hafızada bulunan TH, TL ve Konfigürasyon kaydedicileri
								kalıcı olan eeprom hafızaya yazılır.
Recall E2 (B8h)	184	: Bu komut ile eeprom hafızada saklı olan TH, TL ve Konfigürasyon kaydedicileri 
									tekrardan geçici hafızadaki yerlerine yazılır.
Read Power Suply (B4h)180: Bu komut ile master cihaz hattaki DS18B20'lerin hat üzerinden 
mi yoksa harici olarak mı beslendiklerini kontrol eder
	
*/

#define	in  1
#define out 0


void onewire_reset(void);
void onewire_write(char data);
unsigned char onewire_read( void );
float ds18b20_read(void);
void TRIS_PIN(char x);

float sicaklik;
char sicaklik_bilgisi[16]="";
static vu32 TimingDelay;

int main(){
	initGPIO();
	delay_init();
	lcd_init();
	lcd_yaz("    DS18B20     ");
   lcd_gotoxy(2,1);
   delay_ms(1500);
   lcd_gotoxy(2,1);
   lcd_yaz("   One Wire     ");
    while (1)
    {
        sicaklik=ds18b20_read();
        if(sicaklik>4095) {sprintf(sicaklik_bilgisi,"   Sensor Yok   ");}
        else { sprintf(sicaklik_bilgisi,"  Temp: %.1f%cC", sicaklik,223);}
        lcd_gotoxy(2,1);
        lcd_yaz(sicaklik_bilgisi);
    
		  
	 }
}

float ds18b20_read(void)
{
    int busy;
    int sayi1;
    int sayi2;
    float result;
	
	 onewire_reset();
    onewire_write(0xCC);	//1100 1100	- 204	//skip Rom muhtemelen
    onewire_write(0x44);	//0100 0100	- 68	
	
    while(busy==0)
        busy=onewire_read();
    
	 onewire_reset();
    onewire_write(0xCC);	//1100 1100	- 204
    onewire_write(0xBE);	//1011 1110	- 190
	 
    sayi1 = onewire_read();
    sayi2 = onewire_read();
	 
    result=sayi2*256+sayi1;
    result=result/16.0;
    delay_ms(250);
    return result;
}

void onewire_reset(void)
{
    TRIS_PIN(out);
    GPIOB->BRR = 1<<9;    //GPIO_ResetBits(GPIOB,(1<<9))
	 delay_us(480);
    TRIS_PIN(in);
    delay_us(400);
    TRIS_PIN(out);
}

void onewire_write(char data)
{
    unsigned char i, bitshifter;
    bitshifter = 1;
    for (i=0; i<8; i++)
    {
        if (data & bitshifter)
        {
            TRIS_PIN(out);
            GPIOB->BRR = 1<<9; 
            delay_us(20);	//çakışmayı engelliyor
            TRIS_PIN(in);
            delay_us(60);
        }
        else
        {
            TRIS_PIN(out);
            GPIOB->BRR = 1<<9; 
            delay_us(60);
            TRIS_PIN(in);
        }
        bitshifter = bitshifter<<1;
    }
}

unsigned char onewire_read( void )
{
    unsigned char i;
    unsigned char data, bitshifter;
    data = 0;
    bitshifter = 1;
    for (i=0; i<8; i++)
    {
        TRIS_PIN(out);
        GPIOB->BRR = 1<<9;
        delay_us(6);
        TRIS_PIN(in);
        delay_us(4);
        if ((GPIO_ReadInputDataBit(GPIOB,(1<<9))) && 0x01)
            data |= bitshifter;
        delay_us(50);
        bitshifter = bitshifter<<1;
    }
    return data;
}


void TRIS_PIN(char x)	//in-out
{	
//    GPIO_InitStructure.GPIO_Pin = (1<<9);;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    if(!x)
    {
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 		//Output GPIOB->CRH = 0x070;
		 GPIOB->CRH = (GPIOB->CRH & CONFMASKH(9)) | //9.pini sıfırlanıyor
						GPIOPINCONFH(9, GPIOCONF(GPIO_MODE_OUTPUT50MHz, GPIO_CNF_OUTPUT_OPENDRAIN));
    }
    else
    {
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	GPIOB->CRH = 0x040;
		 GPIOB->CRH = (GPIOB->CRH & CONFMASKH(9)) | 
					GPIOPINCONFH(9, GPIOCONF(GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOATING));
    }
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
}



