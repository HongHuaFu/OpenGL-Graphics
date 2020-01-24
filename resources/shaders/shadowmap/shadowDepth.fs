#version 330 core

void main()
{
    //自动写入
    gl_FragDepth = gl_FragCoord.z;
}
