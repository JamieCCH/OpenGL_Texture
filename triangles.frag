#version 410 core

in vec3 myColor;
out vec4 frag_colour;
in vec2 texCoord;

uniform sampler2D texture0;

void main() {
    frag_colour = texture(texture0, texCoord);
}

