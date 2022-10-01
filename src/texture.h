#pragma once

class Texture
{
public:
    void bind() const;
    void generate(unsigned int width, unsigned int height, unsigned char* data);

    unsigned int ID;
};
