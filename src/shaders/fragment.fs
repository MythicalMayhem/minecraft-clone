#version 330 core

uniform sampler2D customTexture;

out vec4 FragColor;
in vec2 TexCoord;

vec4 skyColor=vec4(1.,1.,1.,1.);

void main(){
   FragColor=texture(customTexture,TexCoord)*skyColor;
   
}