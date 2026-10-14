#include "Simulator.h"

const double Simulator::_timeScale = 0.016;
const int Simulator::_tickTime = 16;
const double Simulator::_g = 9.81;
Simulator::Simulator(AirplaneState* airplane)
{
    this->_airplane = airplane;
    this->_simRunning = false;
}

Simulator::~Simulator()
{
    if (this->_simThread.joinable())
    {
        this->_simThread.join();
    }
}

void Simulator::simLoop()
{
    while (_simRunning)
        {
            Vec3 airplaneForward = this->_airplane->getForward();
            Vec3 airplaneRight = this->_airplane->getRight();
            Vec3 airplaneVelocity = this->_airplane->getVelocity();
            double rho = computeAirDensity(this->_airplane->getAltitude());
            double airplaneM = this->_airplane->getMasse();

            this->_airplane->computeAOA(airplaneForward, airplaneVelocity, airplaneRight);
            this->_airplane->computeIAS(airplaneVelocity.length());

            Vec3 force = computeThrust(airplaneForward) + computeDrag(airplaneVelocity, rho)
                       + computeLift(airplaneVelocity, airplaneRight, rho) + computeWeight(airplaneM);

            airplaneVelocity += force / airplaneM * this->_timeScale;          // a → v
            this->_airplane->setVelocity(airplaneVelocity);

            this->_airplane->setXPos(this->_airplane->getXPos() + airplaneVelocity.getX() * this->_timeScale);   // v → p
            this->_airplane->setYPos(this->_airplane->getYPos() + airplaneVelocity.getY() * this->_timeScale);
            this->_airplane->setAltitude(this->_airplane->getAltitude() + airplaneVelocity.getZ() * this->_timeScale);

            std::this_thread::sleep_for(std::chrono::milliseconds(this->_tickTime));
        }
}
void Simulator::startSim()
{
    if (this->_simThread.joinable())
    {
        this->_simRunning = false;
        this->_simThread.join();
    }
    this->_simRunning = true;
    this->_simThread = std::thread(&Simulator::simLoop, this);
}

void Simulator::stopSim()
{
    this->_simRunning = false;
    if (this->_simThread.joinable())
    {
        this->_simThread.join();
    }
}
double Simulator::computeAirDensity(double altitude)
{
    double RHO = 1.225 * pow((1 - 0.0065 * altitude / 288.15), 4.256);
    return RHO;
}
bool Simulator::getSimRunning() { return this->_simRunning; }

int Simulator::getTickTime() { return this->_tickTime; }
Vec3 Simulator::computeThrust(const Vec3& forward)
{
    return forward * this->_airplane->getTotalThrust();
}

Vec3 Simulator::computeDrag(const Vec3& v, double rho)
{
    double speed = v.length();
    if (speed < 1e-6) return Vec3();
    double mag = 0.5 * rho * speed * speed * this->_airplane->getDragCoef() * this->_airplane->getSurface();
    return v.normalize() * -mag;                 // opposée à la vitesse
}

Vec3 Simulator::computeLift(const Vec3& v, const Vec3& right, double rho)
{
    double speed = v.length();
    if (speed < 1e-6) return Vec3();
    Vec3 dir = right.cross(v).normalize();       // vecteur vitesse, côté "haut" de l'avion
    double cl = _airplane->getLiftCoef() * _airplane->getAOA();
    return dir * (0.5 * rho * speed * speed * cl * _airplane->getSurface());
}

Vec3 Simulator::computeWeight(double m)
{
    return Vec3(0, 0, -m * _g);
}
