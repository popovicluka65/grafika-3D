// Autor: Nedeljko Tesanovic
// Opis: Testiranje dubine, Uklanjanje lica, Transformacije, Prostori i Projekcije

#define _CRT_SECURE_NO_WARNINGS
 
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const int TARGET_FPS = 60;
const double FRAME_TIME = 1.0 / TARGET_FPS;
unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath);


// Videti prepraviti da se random stvaraju vozila na traci, da ne bude fiksno al za test neka ovako
glm::vec3 vehiclePositions[3] = {
    glm::vec3(-4.0f, 0.0f, 0.5f),  
    glm::vec3(-2.0f, 0.0f, -0.5f), 
    glm::vec3(0.0f, 0.0f, 0.0f)   
};

float vehicleSpeed = 1.0f; 
const float VEHICLE_MAX_SPEED = 5.0f;
const float VEHICLE_MIN_SPEED = 0.1f;
const float BRIDGE_LENGTH = 16.0f; 

float vehicleAcceleration = 2.0f; 
float vehicleBraking = 4.0f;       

float cubeVertices[] = {    
    //X    Y    Z       R    G    B    A
    // Prednja strana
    -0.5f, -0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
     0.5f, -0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
     0.5f,  0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
    -0.5f, -0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
     0.5f,  0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
    -0.5f,  0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,

    // Zadnja strana
    -0.5f, -0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
     0.5f,  0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
     0.5f, -0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
    -0.5f, -0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
    -0.5f,  0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
     0.5f,  0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f,

     // Leva strana
     -0.5f,  0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
     -0.5f, -0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
     -0.5f, -0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
     -0.5f,  0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
     -0.5f,  0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
     -0.5f, -0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f,

     // Desna strana
      0.5f,  0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
      0.5f, -0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
      0.5f, -0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
      0.5f,  0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
      0.5f, -0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
      0.5f,  0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f,

      // Gornja strana
      -0.5f,  0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
       0.5f,  0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
       0.5f,  0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
      -0.5f,  0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
       0.5f,  0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
      -0.5f,  0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f,

      // Donja strana
      -0.5f, -0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
       0.5f, -0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
       0.5f, -0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
      -0.5f, -0.5f,  0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
      -0.5f, -0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f,
       0.5f, -0.5f, -0.5f,    0.8f, 0.8f, 0.8f, 1.0f
};
unsigned int cubeVertexCount = 36;
unsigned int cubeStride = (3 + 4) * sizeof(float); // 3 poz, 4 rgba

float planeVertices[] = {
    
    -0.5f, 0.0f,  0.5f,    0.0f, 0.0f, 0.8f, 1.0f, // Donji levi (reka)
     0.5f, 0.0f,  0.5f,    0.0f, 0.0f, 0.8f, 1.0f, // Donji desni (reka)
     0.5f, 0.0f, -0.5f,    0.0f, 0.0f, 0.8f, 1.0f, // Gornji desni (reka)

     -0.5f, 0.0f,  0.5f,    0.0f, 0.0f, 0.8f, 1.0f, // Donji levi (reka)
      0.5f, 0.0f, -0.5f,    0.0f, 0.0f, 0.8f, 1.0f, // Gornji desni (reka)
     -0.5f, 0.0f, -0.5f,    0.0f, 0.0f, 0.8f, 1.0f  // Gornji levi (reka)
};
unsigned int planeVertexCount = 6; //isto ko i za cube
unsigned int planeStride = (3 + 4) * sizeof(float); 


//za 2D teksturu, uzeo od 2D projekta
float vertices[] = {
    -1.0f, -1.0f,     0.0f, 0.0f,
     1.0f, -1.0f,     1.0f, 0.0f,
     1.0f,  1.0f,     1.0f, 1.0f, 
    -1.0f,  1.0f,     0.0f, 1.0f 
};

unsigned int indices[] = {
    0, 1, 2, 
    0, 2, 3
};

unsigned int textureStride = (2 + 2) * sizeof(float);


int main(void)
{
    if (!glfwInit())
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 1280;
    unsigned int wHeight = 720;
    const char wTitle[] = "Most - Projekat 4";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // Fragment je bliži ako ima manju dubinu

 
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // Eliminiše zadnje strane
    glFrontFace(GL_CCW); 

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ PROMJENLJIVE I BAFERI +++++++++++++++++++++++++++++++++++++++++++++++++
    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");

    unsigned int modelLoc = glGetUniformLocation(unifiedShader, "uM");
    unsigned int viewLoc = glGetUniformLocation(unifiedShader, "uV");
    unsigned int projectionLoc = glGetUniformLocation(unifiedShader, "uP");
    unsigned int objectColorLoc = glGetUniformLocation(unifiedShader, "objectColor"); 


    //klasika ko i za 2d
    unsigned int cubeVAO, cubeVBO; //vao i vbo za kocku (stub i vozila)
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO); 
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW); 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cubeStride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, cubeStride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);            

    unsigned int planeVAO, planeVBO;  //vao i vbo za ravan (most i reku)
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);

    glBindVertexArray(planeVAO); 
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW); 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, planeStride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, planeStride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);      

    //uzeto sa 2D projekta za teksturu
    unsigned int textureVAO;
    glGenVertexArrays(1, &textureVAO);
    glBindVertexArray(textureVAO);
    unsigned int textureVBO;
    glGenBuffers(1, &textureVBO);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // da bude cetvorougao moramo spojiti 2 trougla
    unsigned int textureEBO;
    glGenBuffers(1, &textureEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textureEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, textureStride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, textureStride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    unsigned int textureShader = createShader("texture.vert", "texture.frag");

    unsigned checkerTexture = loadImageToTexture("index.png");
    if (checkerTexture == 0) {
        std::cout << "Failed to load texture!" << std::endl;
        return -1;
    }
    glBindTexture(GL_TEXTURE_2D, checkerTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);

    //glDisable(GL_DEPTH_TEST);

    glUseProgram(textureShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, checkerTexture);
    glUniform1i(glGetUniformLocation(textureShader, "uTex"), 0); 


    glBindVertexArray(textureVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ PARAMETRI KAMERE +++++++++++++++++++++++++++++++++++++++++++++++++
    glm::vec3 cameraPos = glm::vec3(0.0f, 10.0f, 5.0f);  
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // Gleda u centar mosta
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);   

    glm::mat4 projectionMatrix = glm::perspective(glm::radians(60.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ RENDER LOOP - PETLJA ZA CRTANJE +++++++++++++++++++++++++++++++++++++++++++++++++

    double lastFrameTime = glfwGetTime();

    float cameraRotationSpeed = 10.0f; 
    float cameraMoveSpeed = 2.0f;    
    glm::mat4 rotationMatrix(1.0f); 


    while (!glfwWindowShouldClose(window))
    {

        auto fpsStartTime = std::chrono::high_resolution_clock::now();
        double currentFrameTime = glfwGetTime();
        double deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
     

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            vehicleSpeed += vehicleAcceleration * (float)deltaTime;
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            vehicleSpeed -= vehicleBraking * (float)deltaTime;
        }

        if (vehicleSpeed > VEHICLE_MAX_SPEED) vehicleSpeed = VEHICLE_MAX_SPEED;
        if (vehicleSpeed < VEHICLE_MIN_SPEED) vehicleSpeed = VEHICLE_MIN_SPEED;


        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(cameraRotationSpeed * (float)deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::vec4 rotatedCameraPos = rotationMatrix * glm::vec4(cameraPos - cameraTarget, 1.0f);
            cameraPos = glm::vec3(rotatedCameraPos) + cameraTarget;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-cameraRotationSpeed * (float)deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::vec4 rotatedCameraPos = rotationMatrix * glm::vec4(cameraPos - cameraTarget, 1.0f);
            cameraPos = glm::vec3(rotatedCameraPos) + cameraTarget;
        }

        glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);
        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDirection, cameraUp));

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            cameraPos -= cameraRight * cameraMoveSpeed * (float)deltaTime;
            cameraTarget -= cameraRight * cameraMoveSpeed * (float)deltaTime; 
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            cameraPos += cameraRight * cameraMoveSpeed * (float)deltaTime;
            cameraTarget += cameraRight * cameraMoveSpeed * (float)deltaTime;
        }

        float pillarHeight = 1.2f;
        float pillarWidthDepth = 0.8f;
        float numPillars = 4;
        float bridgeZOffset = 1.5f;

        for (int i = 0; i < 3; ++i)
        {
            vehiclePositions[i].x += vehicleSpeed * deltaTime;
            // Ako vozilo pređe kraj mosta, vrati ga na početak 
            if (vehiclePositions[i].x > (BRIDGE_LENGTH - pillarWidthDepth) / 2.0f )
                vehiclePositions[i].x = ( - BRIDGE_LENGTH + pillarWidthDepth )/ 2.0f;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        glDisable(GL_DEPTH_TEST);
        glUseProgram(textureShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, checkerTexture);
        glUniform1i(glGetUniformLocation(textureShader, "uTex"), 0);

        glBindVertexArray(textureVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);
        glUseProgram(unifiedShader);

        glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        glBindVertexArray(planeVAO); //crtanje reke

        glm::mat4 riverModel = glm::mat4(1.0f);
        riverModel = glm::scale(riverModel, glm::vec3(BRIDGE_LENGTH + pillarWidthDepth, 1.0f, 10.0f));
        riverModel = glm::translate(riverModel, glm::vec3(0.0f, -0.7f, 0.0f)); 
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(riverModel));
        glDrawArrays(GL_TRIANGLES, 0, planeVertexCount);

        glBindVertexArray(cubeVAO);  // crtanje mostova
        glUniform4f(objectColorLoc, 0.6f, 0.6f, 0.6f, 1.0f); 

        for (int i = 0; i < numPillars; ++i)
        {
            float xPos = -BRIDGE_LENGTH / 2.0f + (BRIDGE_LENGTH / (numPillars - 1)) * i;

            glm::mat4 pillarModelLeft = glm::mat4(1.0f); // Levi stub
            pillarModelLeft = glm::translate(pillarModelLeft, glm::vec3(xPos, -0.7f + pillarHeight / 2.0f, bridgeZOffset));
            pillarModelLeft = glm::scale(pillarModelLeft, glm::vec3(pillarWidthDepth, pillarHeight, pillarWidthDepth));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pillarModelLeft));
            glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);

            glm::mat4 pillarModelRight = glm::mat4(1.0f);// Desni stub
            pillarModelRight = glm::translate(pillarModelRight, glm::vec3(xPos, -0.7f + pillarHeight / 2.0f, -bridgeZOffset));
            pillarModelRight = glm::scale(pillarModelRight, glm::vec3(pillarWidthDepth, pillarHeight, pillarWidthDepth));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pillarModelRight));
            glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
        }

        glBindVertexArray(planeVAO); // crtanje mosta
        glUniform4f(objectColorLoc, 0.1f, 0.1f, 0.1f, 1.0f); // crna za most

        glm::mat4 deckModel = glm::mat4(1.0f);
        deckModel = glm::translate(deckModel, glm::vec3(0.0f, -0.7f + pillarHeight, 0.0f)); 
        deckModel = glm::scale(deckModel, glm::vec3(BRIDGE_LENGTH, 0.2f, bridgeZOffset * 2.0f + 0.2f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(deckModel));
        glDrawArrays(GL_TRIANGLES, 0, planeVertexCount);

        glBindVertexArray(cubeVAO); //isti Vao kao i za stubove (kocke)
        float vehicleHeight = 0.5f;
        float vehicleWidth = 0.8f;
        float vehicleDepth = 1.0f;

        float vehicleColors[3][4] = {
            {1.0f, 0.0f, 0.0f, 1.0f}, // Crveno vozilo
            {0.0f, 1.0f, 0.0f, 1.0f}, // Zeleno vozilo
            {0.0f, 0.0f, 1.0f, 1.0f}  // Plavo vozilo
        };

        for (int i = 0; i < 3; ++i)
        {
            glUniform4f(objectColorLoc, vehicleColors[i][0], vehicleColors[i][1], vehicleColors[i][2], vehicleColors[i][3]);

            glm::mat4 vehicleModelMatrix = glm::mat4(1.0f);
            vehicleModelMatrix = glm::translate(vehicleModelMatrix, glm::vec3(
                vehiclePositions[i].x,
                -0.7f + pillarHeight + (vehicleHeight / 2.0f) + 0.05f,
                vehiclePositions[i].z
            ));
            // Scale: Dubina, visina, širina vozila
            vehicleModelMatrix = glm::scale(vehicleModelMatrix, glm::vec3(vehicleDepth, vehicleHeight, vehicleWidth));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(vehicleModelMatrix));
            glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
        }

        auto fpsEndTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = fpsEndTime - fpsStartTime;

        double sleepTime = FRAME_TIME - elapsed.count();
        if (sleepTime > 0) {
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
        }

        glfwSwapBuffers(window); 
        glfwPollEvents();    
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ POSPREMANJE +++++++++++++++++++++++++++++++++++++++++++++++++
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteProgram(unifiedShader);

    glDeleteBuffers(1, &textureVBO);
    glDeleteVertexArrays(1, &textureVAO);
    glDeleteProgram(textureShader);

    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
    std::string temp = ss.str();
    const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);

    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}

unsigned int createShader(const char* vsSource, const char* fsSource)
{
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 2: InternalFormat = GL_RG; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}