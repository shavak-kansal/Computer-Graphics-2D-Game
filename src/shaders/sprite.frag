#version 330 core
in vec2 TexCoords;
in vec4 PixelPosition;

out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;
uniform vec2 PlayerPosition;
uniform int flag;

void main()
{    
    color = vec4(spriteColor, 1.0) * texture(image, TexCoords);

    float dist = length(PixelPosition - vec4(PlayerPosition, 0.0f, 1.0f));
    
    if(flag == 1){
        if(dist > 200.0f){
            color = color * 0;
        }
    }
}  