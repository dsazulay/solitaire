struct VSInput
{
    float3 Pos;
    float2 UV;
};

Sampler2D textures[];

struct ShaderData
{
    float4x4 projection;
    float4x4 view;
    float4x4 model[3];
    float4 lightPos;
    uint32_t selected;
};

struct VSOutput {
    float4 Pos : SV_POSITION;
    float2 UV;
};

[shader("vertex")]
VSOutput main(VSInput input, uniform ShaderData *shaderData, uint instanceIndex : SV_VulkanInstanceID) {
    VSOutput output;
    float4x4 modelMat = shaderData->model[instanceIndex];
    output.UV = input.UV;
    output.Pos = mul(shaderData->projection,  mul(modelMat, float4(input.Pos.xy, 0.0, 1.0)));
    return output;
}

[shader("fragment")]
float4 main(VSOutput input) {
    return float4(input.UV, 0.0, 1.0);
}
