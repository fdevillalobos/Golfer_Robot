//
//  Functions.c
//  Test_1
//
//  Created by Francisco de Villalobos on 9/25/13.
//  Copyright (c) 2013 Francisco de Villalobos. All rights reserved.
//

#include <stdio.h>
#include "Functions.h"
//#include "m_general.h"
#include "saast.h"



void timer_init(char timer_num, char timer_presc, char timer_mode, char interr_mode){
    
#ifndef timer_presc
#define timer_presc     1
#endif
    
#ifndef timer_mode
#define timer_mode      0
#endif
    
#ifndef interr_mode
#define interr_mode     1
#endif
    
    int a;
    
    switch(timer_num){
            case 1:
            a = 1;
            break;
            case 2:
            a=2;
            break;
    }
    
}