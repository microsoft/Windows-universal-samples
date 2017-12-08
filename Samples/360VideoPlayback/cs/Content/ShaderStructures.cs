using System.Numerics;

namespace _360VideoPlayback.Content
{
    /// <summary>
    /// Constant buffer used to send hologram position transform to the shader pipeline.
    /// </summary>
    internal struct ModelConstantBuffer
    {
        public Matrix4x4 model;
    }

    /// <summary>
    /// Used to send per-vertex data to the vertex shader.
    /// </summary>
    internal struct VertexPositionColor
    {
        public VertexPositionColor(Vector3 pos, Vector3 color)
        {
            this.pos   = pos;
            this.color = color;
        }

        public Vector3 pos;
        public Vector3 color;
    };

    internal struct VertexPositionTexture
    {
        public VertexPositionTexture(Vector3 pos, Vector2 texture)
        {
            this.pos = pos;
            this.texture = texture;
        }

        public Vector3 pos;
        public Vector2 texture;
    };
}
