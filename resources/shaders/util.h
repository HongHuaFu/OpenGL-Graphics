
//转为线性深度
float LinearizeDepth(float depth,float near,float far)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}