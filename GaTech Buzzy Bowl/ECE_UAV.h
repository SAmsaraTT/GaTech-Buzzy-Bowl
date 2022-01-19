#ifndef __ECE__
#define __ECE__
#include<string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include<iostream>
#include<atomic>
#include<thread>
#include<mutex>
#include<chrono>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <random>


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>


// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>



class ECE_UAV
{
public:
    void start();
    void stop();
    friend void threadFunction(ECE_UAV* pUAV);
    vec3 m_Position;
    vec3 m_Velocity;
    double St;
    ECE_UAV(double posx, double posy, double posz);
    double LastTime;
    double ELTime;
    void getPositon();

private:
    std::atomic<bool> m_stop = false;
    double m_mass = 1.0;
    vec3 goal;
    vec3 goal1;
    int time;
    double K1;
    double K2;
    vec3 m_Position1;
    vec3 m_Accerleration;
    double Distance;
    std::thread m_Kthread;

};

ECE_UAV::ECE_UAV(double posx, double posy, double posz)
{   
    m_Position.x = posx;
    m_Position.y = posy;
    m_Position.z = posz;
    goal = { 0.0f,40.0f,0.0f };
    goal1 = { 0.0f,50.0f,0.0f };
    K1 = 0.01;
    K2 = 15;
    Distance = 0.0;
    m_Velocity.x = 0.0f;
    m_Velocity.y = 0.0f;
    m_Velocity.z = 0.0f;
    time = 0;
    St = 0;
    LastTime= glfwGetTime();
    
}
void ECE_UAV::getPositon()
{
    // when UAV touch the surface Let it have a random velocity
    std::random_device rd;
    std::default_random_engine generator{ rd() };
    std::uniform_int_distribution<int> Mode(0,4);
    std::uniform_int_distribution<int> Vx1(3, 5);
    std::uniform_int_distribution<int> Vz1(-5,-2);
    std::uniform_int_distribution<int> Vx2(-4,-2);
    std::uniform_int_distribution<int> Vz2(2, 5);
    std::uniform_int_distribution<int> Vx3(4, 8);
    std::uniform_int_distribution<int> Vz3(-5,-3);
    std::uniform_int_distribution<int> Vx4(3, 6);
    std::uniform_int_distribution<int> Vz4(-2, 4);
    std::uniform_int_distribution<int> Vx5(5, 8);
    std::uniform_int_distribution<int> Vz5(4, 8);
    int mode = Mode(generator);
    float vx1 = Vx1(generator);
    float vz1 = Vz1(generator);
    float vx2 = Vx2(generator);
    float vz2 = Vz2(generator);
    float vx3 = Vx3(generator);
    float vz3 = Vz3(generator);
    float vx4 = Vx4(generator);
    float vz4 = Vz4(generator);
    float vx5 = Vx5(generator);
    float vz5 = Vz5(generator);
    


   // the UAV UAVs then launch from the ground and go towards the (0,0,50)
    if (St == 0)
   {
        vec3 force = goal - m_Position;
        Distance = glm::distance(goal, m_Position);
        double F = K1 * (Distance - 10);
        vec3 force1 = normalize(force);
        vec3 Anormal = vec3(F, F, F);
        

        double Currenttime = glfwGetTime();
        double T = Currenttime - LastTime;
      
    
        m_Position.x = m_Position.x + (m_Velocity.x * T) + (0.5 * m_Accerleration.x * pow(T, 2));
        m_Position.y = m_Position.y + (m_Velocity.y * T) + (0.5 * m_Accerleration.y * pow(T, 2));
        m_Position.z = m_Position.z + (m_Velocity.z * T) + (0.5 * m_Accerleration.z * pow(T, 2));

        m_Accerleration = force1 * Anormal;

        m_Velocity.x = m_Velocity.x + m_Accerleration.x * T;
        m_Velocity.y = m_Velocity.y + m_Accerleration.y * T;
        m_Velocity.z = m_Velocity.z + m_Accerleration.z * T;
        LastTime = Currenttime;
    }
    //the UAVs start to o fly in random paths along the virtual sphere
    if((abs(Distance)< 0.3 && St==0))
    {
     St=1;
     switch (mode)
     {
     case 0:
         m_Velocity.x = vx1;
         m_Velocity.y = 0.0f;
         m_Velocity.z = vz1;
         break;
     case 1:
         m_Velocity.x = vx2;
         m_Velocity.y = 0.0f;
         m_Velocity.z = vz2;
         break;
     case 2:
         m_Velocity.x = vx3;
         m_Velocity.y = 0.0f;
         m_Velocity.z = vz3;
         break;
     case 3:
         m_Velocity.x = vx4;
         m_Velocity.y = 0.0f;
         m_Velocity.z = vz4;
         break;
     case 4:
         m_Velocity.x = vx5;
         m_Velocity.y = 0.0f;
         m_Velocity.z = vz5;
         break;
     }
    }
   if (St == 1)
   {  
       vec3 force = goal1 - m_Position;
       Distance = glm::distance(goal1, m_Position);
       double F = K2 * (Distance - 10);
       vec3 force1 = normalize(force);
       vec3 Anormal = vec3(F, F, F);
       m_Accerleration = force1 * Anormal;

       double Currenttime = glfwGetTime();
       double T = Currenttime - LastTime;
    m_Velocity.x = m_Velocity.x + m_Accerleration.x * T;
    m_Velocity.y = m_Velocity.y + m_Accerleration.y * T;
    m_Velocity.z = m_Velocity.z + m_Accerleration.z * T;
    m_Position.x = m_Position.x + (m_Velocity.x * T) + (0.5 * m_Accerleration.x * pow(T, 2));
    m_Position.y = m_Position.y + (m_Velocity.y * T) + (0.5 * m_Accerleration.y * pow(T, 2));
    m_Position.z = m_Position.z + (m_Velocity.z * T) + (0.5 * m_Accerleration.z * pow(T, 2));
    LastTime = Currenttime;
    }
  
}
// define the thread function
void threadFunction(ECE_UAV* pUAV)
{   
    std::this_thread::sleep_for(std::chrono::seconds(5));
    do
    {
        pUAV->time += 1;
        pUAV->getPositon();
     std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } while (!pUAV->m_stop&&pUAV->time<=4000);
}
//start the threads
void ECE_UAV::start()
{
    
    m_Kthread = std::thread(threadFunction, this);
    
}  
//stop the threads
void ECE_UAV::stop()
{
    m_stop = true;
    if (m_Kthread.joinable())
    {
        m_Kthread.join();
   }

}
#endif