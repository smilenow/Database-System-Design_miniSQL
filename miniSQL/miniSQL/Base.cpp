//
//  Base.cpp
//  miniSQL
//
//  Created by SmilENow on 11/2/14.
//  Copyright (c) 2014 Jiaquan Yin. All rights reserved.
//

#include "Base.h"

std::string Value::getKey(){
    switch(type){
        case _string_type:return charKey;
        case _int_type:{
            std::stringstream ss;
            ss << intKey;
            std::string tmp;
            ss >> tmp;
            return tmp;
        }
        case _float_type:{
            std::stringstream ss;
            ss << floatKey;
            std::string tmp;
            ss >> tmp;
            return tmp;
        }
        default: return "";
    }
}

void Value::resetKey(){
    Valid = true;
    switch (type) {
        case _string_type:
            charKey="";
            break;
        case _int_type:
            intKey=0;
            break;
        case _float_type:
            floatKey=0;
            break;
        default:
            break;
    }
}