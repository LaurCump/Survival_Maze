#version 330

// Input
// Get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color_attrib;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 Color;
uniform float ElapsedTime;
uniform bool MustDeform;

// Output values to fragment shader
out vec3 frag_normal;
out vec3 frag_color;

float bookOfShaders(float x)
{
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float y = sin(x * frequency);
    float t = 0.01f * (ElapsedTime * -130.0f);

    y += sin(x * frequency * 2.1f + t) * 4.5f;
    y += sin(x * frequency * 1.72f + t * 1.121f) * 4.0f;
    y += sin(x * frequency * 2.221f + t * 0.437f) * 5.0f;
    y += sin(x * frequency * 3.1122f + t * 4.269f) * 2.5f;
    y *= amplitude * 0.06f;

    return abs(y);
}

void main()
{
    // Send output to fragment shader
    frag_color = Color;
    frag_normal = v_normal;
    
    vec3 new_v_position = vec3(v_position);

    if (MustDeform)
    {
        float distance = length(v_position);
        new_v_position.x = v_normal.x * (distance + bookOfShaders(v_position.x));
        new_v_position.y = v_normal.y * (distance + bookOfShaders(v_position.y));
        new_v_position.z = v_normal.z * (distance + bookOfShaders(v_position.z));
    }

    // Compute gl_Position
    gl_Position = Projection * View * Model * vec4(new_v_position, 1.0f);
}
