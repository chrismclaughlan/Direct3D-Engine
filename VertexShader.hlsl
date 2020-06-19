struct VSOut
{
	float3 colour : Colour;
	float4 pos : SV_Position;
};

cbuffer CBuff
{
	matrix transform;
};

VSOut main( float3 pos : Position, float3 colour: Colour )
{
	VSOut vso;
	vso.pos = mul(float4(pos, 1.0f), transform);
	vso.colour = colour;
	return vso;
}