#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor; // Može da se koristi za default boju, ali æemo je premostiti uniformom

out vec4 vertColor;

uniform mat4 uM; // Model matrica
uniform mat4 uV; // View matrica
uniform mat4 uP; // Projection matrica

void main()
{
    gl_Position = uP * uV * uM * vec4(aPos, 1.0);
    vertColor = aColor; // Prosleðujemo boju dalje, iako je neæemo koristiti direktno za objectColor
}