#include <iostream>
#include "AirplaneState.h"



int main(){
    AirplaneState *airplane = new AirplaneState(2,0,0,250,300,0,0,360);
    std::cout<<"Airspeed de l'avion"<<airplane->getAirSpeed()<<endl;
    return 0;
}