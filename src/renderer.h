class Renderer
{
public:
    void init();
    //void render(std::vector<Sprite>& sprites);
    void terminate();
    void clear();
private:
    void initMesh();
    //void renderSprite(Sprite& sprite);

    unsigned int VBO, EBO, VAO;

    static constexpr float m_vertices[16] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };

    static constexpr unsigned int m_indices[6] = {
        0, 1, 2, 2, 3, 0
    };
};
