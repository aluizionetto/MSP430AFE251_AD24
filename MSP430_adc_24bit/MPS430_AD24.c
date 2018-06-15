
/**
 * Testes com Conversor AD24 Bits para MSP 430AFE251
 *
 * Autor: Aluizio d'Affonseca Netto
 *
 * Descri��o: Le valor do conversor AD para tens�es entre 0 e 600.0 mV, usando refer�ncia interna do microncontrolador (1.2V)
 * Os valores convetidos s�o enviados pela serial.
 *
 */

#include <msp430.h>
volatile unsigned int i;        // volatile to prevent optimization
volatile unsigned char i2;
unsigned long int ad_conv;

void SendTx(unsigned char data)
{
    //while (!(U0TCTL & TXEPT));
    while (!(IFG1 & UTXIFG0));                //TX buffer ready?
    U0TXBUF = data;
}

void SendTxString(unsigned char *s)
{
    while (*s != '\0')
        SendTx(*s++);
}

void SendTxHex(unsigned char d)
{
    const char table_hex[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    SendTx(table_hex[(d >> 4) & 0x0F]);
    SendTx(table_hex[d & 0x0F]);
}

void SendTxInt(long int num)
{
    unsigned char buf_tx[16];
    char tmp_char;
    unsigned int id = 0;
    unsigned int idr = 0;
    long int tmp;

    if(num < 0)
    {
        num *= (-1);
        SendTx('-');
    }

    do
    {
        tmp = num;
        num = num /10;
        buf_tx[id++] = (tmp - num*10) + '0';
    }while(num);

    buf_tx[id--] = '\0';

    while(idr < id) {
        tmp_char = buf_tx[id];
        buf_tx[id--]= buf_tx[idr];
        buf_tx[idr++] = tmp_char;
    }
    SendTxString(buf_tx);
}

#define SERIAL_CONF 9600
void IniSerial(void)
{
    //GPIO para serial
    P1SEL = BIT3 + BIT4;                      // P1.4 = RXD, P1.3=TXD
    P1SEL2 &= ~(BIT3 + BIT4);

    //configura porta serial 9600
    U0CTL |= SWRST;
    U0CTL |= CHAR;                     // 8bits
    U0TCTL |= SSEL1;           //SMCLK como fonte

    //registradores para baud rate - 9600
#if SERIAL_CONF == 9600
    U0BR0 = 0x41;
    U0BR1 = 0x03;
    U0MCTL = 0x00;  //sem modulador
#elif SERIAL_CONF == 1200

    //registradores para baud rate - 115200
    U0BR0 = 0x0A;
    U0BR1 = 0x1A;
    U0MCTL = 0x00;
#elif SERIAL_CONF == 115200

    //registradores para baud rate - 115200
    U0BR0 = 0x45;
    U0BR1 = 0x00;
    U0MCTL = 0xAA;
#else
#error "BaudRate ERROR"
#endif


    U0ME |= UTXE0;   //ativa modulo TX
    U0CTL &= ~SWRST;   //limpa reset da serial

    IE1 &= ~(UTXIE0 + URXIE0);
}


void InitAD24()
{
    //configuracoes do AD24 bits
    SD24CTL |= (SD24DIV0 + SD24DIV1) + SD24SSEL0 + SD24REFON;
    SD24CCTL0 |= SD24UNI + SD24SNGL + SD24LSBTOG; // muda de MSB para LSB automaticamente
    SD24INCTL0 = 0x00; //mantem estado de reset
}

long int ReadAD24 (void)
{
    long int d_lsb, d_msb, d_r;

    SD24CCTL0 |= SD24SC; //inicia conversao
    while (!(SD24CCTL0 & SD24IFG)) ; // aguarda fim da conversao

    d_msb = SD24MEM0;
    d_lsb = SD24MEM0;

    d_r = ((d_msb << 8) & 0xFFFF00) + (d_lsb & 0xFF);
    return d_r;

}

const float v_const = 3.57627889e-4;
long int tensao = 0;
void main(void)
{
    //configura clock para valor calibrado em 8 MHz
    BCSCTL1 = CALBC1_8MHZ;
    DCOCTL = CALDCO_8MHZ;

    //configura watchdog - desativado
    WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer

    IniSerial();
    InitAD24();

    P1DIR |= 0x01;					// configure P1.0 as output

    while(1)
    {
        P1OUT ^= 0x01;			// toggle P1.0

        for(i=100; i>0; i--)     // delay
            __delay_cycles(8000);

        SendTxString("AD > ");
        ad_conv = ReadAD24();
        tensao = (long int)(v_const*(float)(ad_conv));
        SendTxString(" ");
        SendTxInt(tensao/10);
        SendTxString(".");
        SendTxInt(tensao%10);
        SendTxString(" ");
        SendTxHex((ad_conv >> 16) & 0xFF);
        SendTxHex((ad_conv >> 8) & 0xFF);
        SendTxHex(ad_conv & 0xFF);
        SendTxString("\r\n");
    }
}
