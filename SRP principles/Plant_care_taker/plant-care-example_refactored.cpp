#include <cassert>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <optional>
#include <random>
#include <thread>
#include <stdexcept>
#include <string>
#include <set>



/* Example where SRP is violated.
 * This code modeles some smart device that takes care of home plants.
 * Here some other SOLID principles are also violated
 * but we will discuss those in other sections.
 */


/* This class represents the part of the device that can pour or sprinkle water to moisturize soil
 * or air if it’s too dry, close or open window blinds if there is too much or too little sunlight.
 * Sensors detect such changes in conditions and talk to this class through its interface (methods).  
 */
class SensorLight {
private:    
    using TimePoint = decltype(std::chrono::system_clock::now());

    std::optional<TimePoint> sunlightOnFrom;
    std::optional<TimePoint> sunlightOffFrom;

    bool sensorOn = true;
    const int upperLightThreshold = 3;
    const int lowerlightThreshold = 2;

    bool isSunlight()
    {
        /* We simulate changes of sunlight by generating random numbers that represent
         * a probability of change in sunlight conditions: if the sun shines or not.
         */
        static bool sunlightOn = true;
        static std::mt19937 generator;

        auto prob = std::uniform_int_distribution<int>(1, 100)(generator);
        if (prob >= 90) {
            sunlightOn = !sunlightOn;
        }

        const auto currentTime = std::chrono::system_clock::now();
        if (sunlightOn and sensorOn) {
            sunlightOnFrom = sunlightOnFrom ? *sunlightOnFrom : currentTime;
            sunlightOffFrom = std::nullopt;
        } else if (not sunlightOn) {
            sunlightOffFrom = sunlightOffFrom ? *sunlightOffFrom : currentTime;
            sunlightOnFrom = std::nullopt;
        }

        return sunlightOn;
    }
public:
    SensorLight()
    {
    }


    int getSunlightValue ()
    {
        bool sunlight = this->isSunlight();
        auto timeNow = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsedSeconds;
        if (sunlight) {
            elapsedSeconds = timeNow - (*sunlightOnFrom);
            }  
        else {
            elapsedSeconds = timeNow - (*sunlightOffFrom);
            }
         std::cout<<elapsedSeconds.count()<<std::endl;
         return elapsedSeconds.count();   
    }
};

class SensorSoil {
    bool sensorOn = true;
    const int min = 0;
    const int max = 10;
public:
    sensorSoil()
    {
    }
    int getSoilMoistureValue()
    {   
        static std::mt19937 generator;
        return std::uniform_int_distribution<int>(min, max)(generator);
    }
};

class SensorAir {
    bool sensorOn = true;
    const int min = 0;
    const int max = 10;
public:
    sensorAir()
    {
    }
    int getAirMoistureValue()
    {
        static std::mt19937 generator;
        return std::uniform_int_distribution<int>(min, max)(generator);
    }
};


class PlantCaretaker
{
    bool windowBlindsOpen = true;
    const int upperLightThreshold = 3;
    const int lowerlightThreshold = 2;

    const int lowerSoilThreshold = 3;
    const int lowerAirThreshold = 3;
public:
    void pourWater() { std::cout << "Pouring water" << std::endl; }

    void sprinkleWater() { std::cout << "Sprinkling water" << std::endl; }

    void openWindowBlinds() {
        if (not windowBlindsOpen) {
            windowBlindsOpen = true;
            std::cout << "Opened window blinds" << std::endl;
        }
    }

    void closeWindowBlinds() {
        if (windowBlindsOpen) {
            windowBlindsOpen = false;
            std::cout << "Closed window blinds" << std::endl;
        }
    }

    void operator()()
    {
        auto sensorLight = SensorLight();
        auto sensorSoil = SensorSoil();
        auto sensorAir = SensorAir();

        int sunlightValue;
        int airMoistureValue;

        for(;;)
        {
            //light
            //std::cout<<"Light value: "<<sensorLight.getSunlightValue()<<std::endl;
            sunlightValue = sensorLight.getSunlightValue();
            if (sunlightValue>upperLightThreshold) 
                closeWindowBlinds();
            else if (sunlightValue< lowerlightThreshold)
                openWindowBlinds();

            //soil
            if (sensorSoil.getSoilMoistureValue()<lowerSoilThreshold)
                pourWater();

            //air
            if (sensorAir.getAirMoistureValue()<lowerAirThreshold)
                sprinkleWater();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

};



void run() {
    auto plantCareTaker = PlantCaretaker();
    plantCareTaker();
}


int main()
{
    run();
    return 0;
}