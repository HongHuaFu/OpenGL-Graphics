#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{		
    //半球采样方向
    vec3 N = normalize(WorldPos);

    vec3 irradiance = vec3(0.0);   
    
    // 切线空间
    // 施密特正交化
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, N);
    up = cross(N, right);
       
    
    float sampleDelta = 0.025;
    float nrSamples = 0.0f;

    // phi 航向角
    // theta 倾斜角
    // 使用phi和theta代替立体角
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        // 因为采样半球，航向角范围是0-2pi，倾斜角是 0.5pi
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // 切线空间下
            // 球形坐标到笛卡尔空间
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));

            // 切线空间换为世界空间
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

 
            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    
    FragColor = vec4(irradiance, 1.0);
}
