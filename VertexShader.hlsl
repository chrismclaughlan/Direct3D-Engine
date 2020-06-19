struct VSOut
{
	float3 colour : Colour;
	float4 pos : SV_Position;
};

VSOut main( float2 pos : Position, float3 colour: Colour )
{
	VSOut vso;
	vso.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
	vso.colour = colour;
	return vso;
}