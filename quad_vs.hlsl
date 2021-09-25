#pragma once


struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    uint id : SV_VERTEXID;

};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

OutputType main(InputType input)
{

    OutputType output;
   
    output.tex.x = (input.id == 2) ? 2.0 : 0.0;
    output.tex.y = (input.id == 1) ? 2.0 : 0.0;

    output.position = float4(output.tex * float2(2.0, -2.0) + float2(-1.0, 1.0), 1.0, 1.0);


    return output;
}