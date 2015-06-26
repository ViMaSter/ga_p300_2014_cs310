SamplerState TextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

Texture2D Texture;


struct VS_IN {
	int3 Position : POSITION;
	float4 Color : COLOR0;
	float2 TexCoord : TEXCOORD;
};

struct VS_TO_PS {
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
	float2 TexCoord : TEXCOORD;
};

/*
SV_ == SystemValue (Converts position to position in ScreenSpace)
*/

cbuffer ScreenSizeBuffer : register(cb0) {
	float4 ScreenSize;	
	float4x4 transformationMatrix;
};

VS_TO_PS VS_main(VS_IN input)
{
	VS_TO_PS output;
	output.Color = input.Color;
	output.TexCoord = input.TexCoord;
	output.Position.xy = float2(input.Position.x, input.Position.y);
	output.Position.z = 0;
	output.Position.w = 1;

	output.Position.xy /= ScreenSize.xy;
	output.Position.x *= 2.0f;
	output.Position.x -= 1.0f;
	output.Position.y *= -2.0f;
	output.Position.y += 1.0f;

	output.Position = mul(output.Position, transformationMatrix);

	return output;
}

float4 PS_main(VS_TO_PS input) : SV_TARGET
{
	return Texture.Sample(TextureSampler, input.TexCoord);
}