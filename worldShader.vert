#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTex;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 NormalMatrix;
uniform mat4 MVP;

uniform sampler2D TexColor;
uniform sampler2D TexGrey;
uniform float textureOffset;

uniform float heightFactor;
uniform float imageWidth;
uniform float imageHeight;

out Data
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
} data;


out vec3 LightVector;// Vector from Vertex to Light;
out vec3 CameraVector;// Vector from Vertex to Camera;

void main()
{
    
    // Calculate texture coordinate based on data.TexCoord
    vec2 textureCoordinate = vec2(VertexTex.x+textureOffset, VertexTex.y);
    data.TexCoord = textureCoordinate;

    vec4 heightMap = texture(TexGrey, textureCoordinate);

    // get texture value, compute height
    vec3 pos = VertexPosition + VertexNormal*heightFactor*heightMap.x;

    // compute normal vector
    // data.Normal = normalize(VertexNormal);
    data.Normal = normalize(NormalMatrix*vec4(VertexNormal,0.0f)).xyz;
    LightVector = normalize(lightPosition-pos);
    CameraVector = normalize(cameraPosition-pos);

   // set gl_Position variable correctly to give the transformed vertex position
    gl_Position = MVP * vec4(pos,1.0f);
}