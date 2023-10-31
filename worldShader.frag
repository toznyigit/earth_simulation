#version 430

in Data
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
} data;
in vec3 LightVector;
in vec3 CameraVector;

uniform vec3 lightPosition;
uniform sampler2D TexColor;
uniform sampler2D MoonTexColor;
uniform sampler2D TexGrey;
uniform float textureOffset;

out vec4 FragColor;

void main()
{
    // Calculate texture coordinate based on data.TexCoord
    vec2 textureCoordinate = data.TexCoord;
    vec4 texColor = texture(TexColor, textureCoordinate);
    vec4 amb_ref = vec4(0.5f,0.5f,0.5f,1.0f);
    vec4 amb_light_color = vec4(0.6f,0.6f,0.6f,1.0f);
    vec4 spec_ref = vec4(1.0f,1.0f,1.0f,1.0f);
    vec4 spec_light_color = vec4(1.0f,1.0f,1.0f,1.0f);
    vec4 diff_ref = vec4(texColor.xyz,1.0f);
    vec4 diff_light_color = vec4(1.0f,1.0f,1.0f,1.0f);
    int phong_exp = 10;
    vec3 h = (LightVector+CameraVector)/length(LightVector+CameraVector);
    float cos_t = max(dot(LightVector,data.Normal),0);
    float cos_a = max(dot(data.Normal,h),0);

    vec3 ambient = vec3(amb_light_color*amb_ref);    
    vec3 diffuse = vec3(diff_ref*cos_t*diff_light_color);
    vec3 spec = vec3(spec_ref*pow(cos_a,phong_exp)*spec_light_color);

    FragColor = vec4(clamp( texColor.xyz * vec3(ambient + diffuse + spec), 0.0, 1.0), 1.0);

}
