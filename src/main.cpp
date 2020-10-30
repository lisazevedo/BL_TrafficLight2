#include "mbed.h"

// GLOBAL VALUES
InterruptIn button(p5);
DigitalOut red_led(p6);
DigitalOut yellow_led(p7);
DigitalOut green_led(p8);
Timeout timeout;
Ticker ticker;
enum STATE{OFF, ALERT, RED, YELLOW, GREEN, ON, START};
int curr_state;
int pre_state;
int count_m;
int state_button;

// FUNCTIONS DECLARATION
void transition_state();
void default_state();
void change_red();
void change_yellow();
void change_yellow_1();
void change_green();
void change_alert();
void change_off();
void f_add_count_m();
void end_count_m();
void start_count_m();

// MAIN
int main() {
    curr_state = START;
    pre_state = START;
    transition_state();
    
    button.rise(&start_count_m);
    button.fall(&end_count_m);
    
    while(1) {
        wait(1);
    }
}

// FUNCTIONS 
void start_count_m(){
    timeout.detach();
    count_m = 0;
    ticker.attach(f_add_count_m,1);
}
void end_count_m(){
    ticker.detach();
    state_button = ON;
    transition_state();
}
void change_alert(){
    pre_state = curr_state;
    curr_state = ALERT;
    change_yellow_1();
}
void change_off(){
    pre_state = curr_state;
    curr_state = OFF;
    default_state();
}
void f_add_count_m(){
    count_m++;
}
void transition_state(){
    default_state();
    
    if(curr_state == START && pre_state == START){
        red_led = 1;
        pre_state = RED;
        curr_state = RED;
        timeout.attach(&transition_state, 10);
    }
    else if(state_button == OFF){
        if(curr_state == RED) change_green();
        else if(curr_state == GREEN) change_yellow();
        else if(curr_state == YELLOW) change_red();
    }
    
    else if(state_button == ON){
        state_button = OFF;
        if(curr_state == RED){
            if(count_m >= 3 && count_m <= 10) change_alert();
            else if(count_m > 10) change_off();
            else change_red();
        }
        else if(curr_state == YELLOW){
            if(count_m>=3 && count_m<=10) change_alert();
            else if(count_m > 10) change_off();
            else change_yellow();
        }
        else if(curr_state == GREEN){
            if(count_m >= 3 && count_m <= 10) change_alert();
            else if(count_m == 20 || count_m <= 3) change_yellow();
            else if(count_m > 10) change_off();
            else change_green();
        }
        else if(curr_state == ALERT){
            if(count_m >= 3 && count_m <= 10){
                if(pre_state == RED) change_red();
                else if(pre_state == YELLOW) change_yellow();
                else change_green();
            }
            else if(count_m > 10) change_off();
            else{
                curr_state = pre_state;
                change_alert();
            }
        }
        else if(curr_state == OFF){
            if(count_m > 10) change_red();
            else change_off();
        }
    }
}
void default_state(){
  red_led = 0;
  yellow_led = 0;
  green_led = 0;
  timeout.detach();
  ticker.detach();
}
void change_red(){
    red_led = 1;
    pre_state = curr_state;
    curr_state = RED;
    timeout.attach(&transition_state, 10);
}
void change_yellow(){
    yellow_led = 1;
    pre_state = curr_state;
    curr_state = YELLOW;
    timeout.attach(&transition_state, 4);
}
void change_green(){
    green_led = 1;
    pre_state = curr_state;
    curr_state = GREEN;
    timeout.attach(&transition_state, 20);
}
void change_yellow_1(){
    yellow_led = !yellow_led;
    ticker.attach(change_yellow_1, 0.5);
}

