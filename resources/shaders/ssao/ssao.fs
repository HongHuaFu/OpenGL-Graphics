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
        // get sample position
        vec3 sample = TBN * samples[i]; // from tangent to view-space
        sample = fragPos + sample * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(sample, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;
}