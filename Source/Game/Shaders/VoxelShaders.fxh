//--------------------------------------------------------------------------------------
// File: VoxelShaders.fx
//
// Copyright (c) Kyung Hee University.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------

#ifndef NUM_LIGHTS
#define NUM_LIGHTS 2
#endif

Texture2D aTextures[2] : register(t0);
SamplerState aSamplers[2] : register(s0);


//--------------------------------------------------------------------------------------
// Constant Buffer Variables 
//--------------------------------------------------------------------------------------

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnCameraMovement

  Summary:  Constant buffer used for view transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
cbuffer cbChangeOnCameraMovement : register(b0)
{
    matrix View;
    float4 CameraPosition;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnResize

  Summary:  Constant buffer used for projection transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
cbuffer cbChangeOnResize : register(b1)
{
    matrix Projection;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangesEveryFrame

  Summary:  Constant buffer used for world transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
cbuffer cbChangesEveryFrame : register(b2)
{
    matrix World;
    float4 OutputColor;
    bool HasNormalMap;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbLights

  Summary:  Constant buffer used for shading
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
cbuffer cbLights : register(b3)
{
    float4 LightPositions[2];
    float4 LightColors[2];
};

struct VS_INPUT
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    row_major matrix Transform : INSTANCE_TRANSFORM;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 WorldPosition : WORLDPOS;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VSVoxel(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Position = mul(input.Position, input.Transform);
    output.Position = mul(output.Position, World);
    output.WorldPosition = output.Position;
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    
    output.TexCoord = input.TexCoord;
    output.Normal = mul(float4(input.Normal, 0.0f), input.Transform).xyz;
    output.Normal = mul(float4(output.Normal, 0.0f), World).xyz;

    if (HasNormalMap)
    {
        output.Tangent = normalize(mul(float4(input.Tangent, 0), World).xyz);
        output.Bitangent = normalize(mul(float4(input.Bitangent, 0), World).xyz);
    }

    return output;
}

float4 PSVoxel(PS_INPUT input) : SV_Target
{
    float3 normal = normalize(input.Normal);

     if (HasNormalMap)
     {
         // Sample the pixel in the normal map.
         float4 bumpMap = aTextures[1].Sample(aSamplers[1], input.TexCoord);

         // Expand the range of the normal value from (0, +1) to (-1, +1).
         bumpMap = (bumpMap * 2.0f) - 1.0f;

         // Calculate the normal from the data in the normal map.
         float3 bumpNormal = (bumpMap.x * input.Tangent) + (bumpMap.y * input.Bitangent) + (bumpMap.z * normal);

         // Normalize the resulting bump normal and replace existing normal
         normal = normalize(bumpNormal);
     }

    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    float3 ambient = float3(0.0f, 0.0f, 0.0f);
    float3 lightDirection = float3(0.0f, 0.0f, 0.0f);
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    float3 reflectDirection = float3(0.0f, 0.0f, 0.0f);
    float3 viewDirection = normalize(input.WorldPosition - CameraPosition.xyz);
    for (uint i = 0; i < NUM_LIGHTS; ++i)
    {
        lightDirection = normalize(input.WorldPosition - LightPositions[i].xyz);
        reflectDirection = reflect(lightDirection, input.Normal);

        diffuse += saturate(dot(normal, -lightDirection)) * LightColors[i];  
        ambient += (0.1f, 0.1f, 0.1f) * LightColors[i].xyz;
        specular += saturate(pow(max(dot(-viewDirection, reflectDirection),0), 20.0f)) * LightColors[i];
    }

    return float4(ambient + diffuse + specular, 1.0f) * float4(aTextures[0].Sample(aSamplers[0],input.TexCoord));;

}