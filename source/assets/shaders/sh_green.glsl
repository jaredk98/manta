#pragma vertex

	// Vertex Input
	in vec3 vertPosition;
	in vec2 vertUV;
	in vec4 vertColor;

	// Vertex Output
	out vec2 fragUV;
	out vec4 fragColor;

	uniform mat4x4 matrixMVP;

	// Vertex Shader
	void main()
	{
		gl_Position = vec4( ( matrixMVP * vec4( vertPosition.xy, 0.0, 1.0 ) ).xy, -1.0 + vertPosition.z * 2.0, 1.0 );
		fragUV = vertUV;
		fragColor = vertColor;
	}

#pragma fragment

	// Fragment Input
	in vec2 fragUV;
	in vec4 fragColor;

	// Fragment Output
	out vec4 outColor[1];

	// Fragment Uniforms
	uniform sampler2D texture0;

	// Fragment Shader
	void main()
	{
		vec4 base = texture( texture0, fragUV );
		if( base.a == 0.0 ) { discard; }

		outColor[0] = base * fragColor * vec4( 0.0, 1.0, 0.0, 1.0 );
	}

#pragma compute

	// Compute Shader
	void main()
	{
		// Do some stuff here
	}