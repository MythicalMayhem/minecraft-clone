#version 330 core

uniform vec4 customColor;
uniform sampler2D customTexture;

out vec4 FragColor;
in vec4 vertexColor;
in vec2 TexCoord;


void main()
{
   FragColor = texture(customTexture,TexCoord);
   // FragColor=customColor;
};