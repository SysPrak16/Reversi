//
// Created by Philipp on 26.11.15.
//

#include "thinker.h"

int moveOk(){
    int ret=0;
    if(field[move[0]+1][move[1]+1]!='*'){
        ret=1;
    }
    if(field[move[0]-1][move[1]+1]=='*'){
        ret=1;
    }
    if(field[move[0]+1][move[1]-1]=='*'){
        ret=1;
    }
    if(field[move[0]-1][move[1]-1]=='*'){
        ret=1;
    }
    return ret;
}