#include <cdrom.h>

void cdrom_init(void){
    

}

void cdrom_irq(void){

    uint8_t * msg =0;

    msg[0] = 'C';
    msg[1] = 'D';
    msg[2] = '-';
    msg[3] = 'R';
    msg[4] = 'O';
    msg[5] = 'M';
}