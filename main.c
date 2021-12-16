#include <msp430g2553.h>

volatile long ADC_Value;
char result[100];
volatile long sample[100];

void ConfigureAdc_ldo();
void ConfigureAdc_therm();
void ConfigureAdc_pot();
long int getADC_Value(void);

void adc_out(int data);
void uart_init(void);
void ConfigClocks(void);
void strreverse(char* begin, char* end);
void itoa(int value, char* str, int base);
void port_init();

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

    port_init();

    ConfigClocks();
    uart_init();

    _delay_cycles(5);

    while(1){
        _delay_cycles(2000000);
        //send light value
        ConfigureAdc_ldo();
        ADC_Value = getADC_Value();
        int lightPercent;
        lightPercent = (100*ADC_Value)/1024;                 //make light value to percent
        adc_out(lightPercent);
        while((IFG2 & UCA0TXIFG)==0);
            UCA0TXBUF = ',';
        //send temperature value
        ConfigureAdc_therm();
        ADC_Value = getADC_Value();
        ////////////////////////////////
        int TempConv;
        TempConv = ADC_Value - 30;                          //convert raw data to F
        ///////////////////////////////
        adc_out(TempConv);
        while((IFG2 & UCA0TXIFG)==0);
            UCA0TXBUF = ',';
        //send pot value
        ConfigureAdc_pot();
        ADC_Value = getADC_Value();
        int potPercent;
        potPercent = (100*ADC_Value)/1024;                  //make pot value to percent
        adc_out(potPercent);
        while((IFG2 & UCA0TXIFG)==0);
            UCA0TXBUF = '\n';
        while((IFG2 & UCA0TXIFG)==0);
            UCA0TXBUF = '\r';

        _delay_cycles(10000000);        //slows down data for thingspeak, can only upload every 15 seconds
    }
}

void adc_out(int data) {
    itoa(data, result,10);
    int acount = 0;
    while(result[acount]!='\0') {
        while((IFG2 & UCA0TXIFG)==0);
            UCA0TXBUF = result[acount++];
    }
}

long int getADC_Value (void) {
    __delay_cycles(1000);
    ADC10CTL0 |= ENC + ADC10SC +MSC;
    while((ADC10CTL0 & ADC10IFG) == 0);
    long int ADC_Value = ADC10MEM;
    ADC10CTL0 &= ~ADC10IFG;
    return ADC_Value;
}


void ConfigureAdc_ldo (void) {
    ADC10CTL0 &= (~ ENC );
    ADC10CTL1 = INCH_0 + ADC10DIV_0;           // Pin A0
    ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON;
    __delay_cycles(5);
    ADC10CTL0 |= ENC| ADC10SC;
}

void ConfigureAdc_therm(void) {
    ADC10CTL0 &= (~ ENC );
    ADC10CTL1 = INCH_4 + ADC10DIV_0;            // Pin A4
    ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON;
    __delay_cycles(5);
    ADC10CTL0 |= ENC| ADC10SC;
}

void ConfigureAdc_pot(void) {
    ADC10CTL0 &= (~ ENC );
    ADC10CTL1 = INCH_5 + ADC10DIV_0;            // Pin A5
    ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON;
    __delay_cycles(5);
    ADC10CTL0 |= ENC| ADC10SC;
}

void uart_init(void){
    UCA0CTL1 |= UCSWRST;
    UCA0CTL1 |= UCSSEL_3;
    UCA0BR1 =0;
    UCA0BR0 = 104;
    UCA0MCTL = 0x02;
    UCA0CTL1 &= ~UCSWRST;
    IE2 |= UCA0RXIE;
}

void ConfigClocks(void) {
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    BCSCTL3 |= LFXT1S_2;
    IFG1 &= ~OFIFG;
    BCSCTL2 = 0;
 }

void strreverse(char* begin, char* end) {
    char aux;
    while(end>begin)
        aux=*end, *end--=*begin, *begin++=aux;
}

void itoa(int value, char* str, int base) {
    static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char* wstr=str;
    int sign;


    if (base<2 || base>35){
        *wstr='\0';
        return;
    }


    sign=value;
    if (sign < 0)
        value = -value;

    do
        *wstr++ = num[value%base];
    while(value/=base);

    if(sign<0)
        *wstr++='-';
    *wstr='\0';
    strreverse(str,wstr-1);
}

void port_init(){
    P1SEL |= BIT1 + BIT2;
    P1SEL2 |= BIT1 + BIT2;
}
