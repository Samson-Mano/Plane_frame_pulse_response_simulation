#version 330 core

uniform mat4 modelMatrix;
uniform mat4 panTranslation;
uniform float zoomscale;
uniform float deflscale;
uniform float transparency = 1.0f;
uniform float geom_scale;

layout(location = 0) in vec2 node_position;
layout(location = 1) in vec2 node_defl;
layout(location = 2) in vec3 vertexColor;
layout(location = 3) in float is_offset;

out vec2 v_textureCoord;
out vec4 v_textureColor;

void main()
{
	// apply zoom scaling and Rotation to model matrix
	mat4 scalingMatrix = mat4(1.0)*zoomscale;
	scalingMatrix[3][3] = 1.0f;
	mat4 scaledModelMatrix = scalingMatrix * modelMatrix;
	
	// Declare variable to store final node center
	vec4 finalPosition;

	if(is_offset == 0.0f)
	{
		// apply Translation to the final position 
		finalPosition = scaledModelMatrix * vec4(node_position,0.0f,1.0f) * panTranslation;
	}
	else
	{
		// Scale based on model
		float node_circe_radii = 0.005f;
		float defl_ratio = deflscale * (node_circe_radii/ geom_scale);

		// Scale the deflection point
		vec2 defl_position = vec2(node_position.x + (node_defl.x * defl_ratio), node_position.y - (node_defl.y * defl_ratio));

		// apply Translation to the node position
		finalPosition = scaledModelMatrix * vec4(defl_position,0.0f,1.0f) * panTranslation;
	}

	v_textureColor = vec4(vertexColor,transparency);

	// Final position passed to fragment shader
	gl_Position = finalPosition;
}