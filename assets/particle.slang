struct VSInput
{
    float3 Pos;
    float2 UV;
};

Sampler2D textures[];

struct ShaderData
{
    float4x4 projection;
    float4x4 model[300];
    float2 offset[300];
    float4 color[300];
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float2 UV;
    float4 color;
};

[shader("vertex")]
VSOutput main(VSInput input, uniform ShaderData *shaderData, uint instanceIndex : SV_VulkanInstanceID)
{
    VSOutput output;
    float4x4 modelMat = shaderData->model[instanceIndex];
    float3 offset = float3(shaderData->offset[instanceIndex], 0.0);
    output.UV = input.UV;
    output.UV.y = 1 - output.UV.y;
    output.color = shaderData->color[instanceIndex];
    output.Pos = mul(shaderData->projection,
                     mul(modelMat, float4(input.Pos + offset, 1.0)));
    return output;
}

[shader("fragment")]
float4 main(VSOutput input)
{
    return input.color;
}
