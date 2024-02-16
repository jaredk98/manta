// Vertex Input
struct VertexInput
{
	float3 Position : POSITION;
	float2 UV       : TEXCOORD;
	float4 Color    : COLOR;
};

// Vertex Output
struct VertexOutput
{
	float4 Position : SV_POSITION;
	float2 UV       : TEXCOORD;
	float4 Color    : COLOR;
	float  Depth    : TEXCOORD1;
};

// Matrix Buffer
cbuffer MatrixBuffer : register(b0) // MVP matrix is always slot 0
{
	float4x4 matrixMVP;
};

// Vertex Shader
void vs_main( in VertexInput In, out VertexOutput Out )
{
	Out.Position = float4( mul( matrixMVP, float4( In.Position.xy, 0.0f, 1.0f ) ).xy, In.Position.z, 1.0f );
	Out.UV       = In.UV;
	Out.Color    = In.Color;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pixel Samplers
Texture2D texture0       : register(t0);
SamplerState BaseSampler : register(s0);

// Pixel Output
struct pixelOutput
{
	float4 Color0 : SV_TARGET0;
};

// Pixel Shader
pixelOutput ps_main( in VertexOutput In )
{
	float4 base = texture0.Sample( BaseSampler, In.UV );
	if( base.a == 0.0 ) { discard; }

	// Output
	pixelOutput output;
	output.Color0 = base * In.Color * float4( 0.0, 1.0, 0.0, 1.0 );
	return output;
}
