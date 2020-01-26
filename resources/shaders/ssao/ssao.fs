#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise; //采样向量旋转噪声

uniform vec3 samples[64];//采样向量组


int kernelSize = 64;
uniform float radius = 0.5f;
uniform float bias = 0.025f;

uniform vec2 WindowSize;
uniform mat4 projection;

void main()
{
    //平铺采样
    vec2 noiseScale = vec2(WindowSize.x/4.0f, WindowSize.y/4.0f); 
    vec3 fragPos = texture(gPosition, TexCoords).xyz; //观察空间坐标
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);

    //4*4
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

    //施密特正交化
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal); //TBN


    // 累加遮蔽因子
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        vec3 sample = TBN * samples[i];
        sample = fragPos + sample * radius; 
        
        vec4 offset = vec4(sample, 1.0);
        offset = projection * offset; // 转为裁剪坐标系下
        offset.xyz /= offset.w; // 透视除法
        offset.xyz = offset.xyz * 0.5 + 0.5; 
        
     
        float sampleDepth = texture(gPosition, offset.xy).z; // 获取采样深度值
        
        //边缘检测
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;
}