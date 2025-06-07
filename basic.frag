#version 330 core
out vec4 FragColor;

in vec4 vertColor; // Boja koja dolazi iz Vertex Shader-a
uniform vec4 objectColor; // Uniformna promenljiva za boju objekta

void main()
{
    FragColor = objectColor; // Koristimo boju poslatu putem uniforme
}