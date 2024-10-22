#version 330 core
layout(location=0)in vec3 aPos;
layout(location=1)in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int alightPower;

out vec2 TexCoord;
out int lightPower;

void main()
{
   gl_Position=projection*view*model*vec4(aPos,1.);
   TexCoord=vec2(aTexCoord.x,aTexCoord.y);
   lightPower=alightPower;
}