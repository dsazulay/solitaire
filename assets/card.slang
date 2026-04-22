struct VSInput
{
    float3 Pos;
    float2 UV;
};

Sampler2D textures[];

struct ShaderData
{
    float4x4 projection;
    float4x4 model[60];
    float2 uvOffset[60];
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float2 UV;
};

[shader("vertex")]
VSOutput main(VSInput input, uniform ShaderData *shaderData, uint instanceIndex : SV_VulkanInstanceID)
{
    VSOutput output;
    float4x4 modelMat = shaderData->model[instanceIndex];
    float2 uvOffset = shaderData->uvOffset[instanceIndex];
    output.UV = input.UV + uvOffset;
    output.UV.y = 1 - output.UV.y;
    output.Pos = mul(shaderData->projection,  mul(modelMat, float4(input.Pos, 1.0)));
    return output;
}

[shader("fragment")]
float4 main(VSOutput input)
{
    float4 color = textures[0].Sample(input.UV);
    return color;
}
