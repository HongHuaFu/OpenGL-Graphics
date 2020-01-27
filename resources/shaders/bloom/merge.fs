#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

uniform bool bloom;
uniform float exposure;//bloom 曝光度

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, TexCoords).rgb;//      
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;//超过阈值的光照

    if(bloom)
        hdrColor += bloomColor; 

    // hdr光照度映射
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

    // 伽马校正      
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}