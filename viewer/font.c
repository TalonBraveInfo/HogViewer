/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

#include "main.h"
#include "font.h"

#include <IL/il.h>

/*  TAB Format Specification        */

/*
    ?? ?? ?? ?? W     H
    C0 03 C0 01 0E 00 20 00

    No differential pattern for each value from first two bytes…
    8
    10
    14
    6
    6
    10

    So far I think these are stored as four separate bytes, or two pairs because
     it’s the only way these could make any sense imo, but I can’t see a pattern here either...

    A  A0 B  B0
    00 00 00 00

    The first byte is linked in with the spacing of each character, somehow.
     If I’m correct, then the difference of these should be the space before the next character… Hrrrmm

    B changes for each row within the texture
    A0 changes twice before the next row within the texture
*/

typedef struct TABIndex {
    // C0035A00 02001E00

    uint8_t a0; uint8_t a1; // ?
    uint8_t b0; uint8_t b1; // ?

    uint16_t width;
    uint16_t height;
} TABIndex;

void DrawCharacter(PIGFont *font, int x, int y, float scale, uint8_t character) {
    if(character < 33 || character > 122) {
        return;
    }

    static PLMesh *font_mesh = NULL;
    if(!font_mesh) {
        if(!(font_mesh = plCreateMesh(
                PL_PRIMITIVE_TRIANGLES,
                PL_DRAW_IMMEDIATE,
                2,
                4
        ))) {
            PRINT_ERROR("Failed to create font mesh!\n");
        }
    }

    character -= 33;

#if defined(DEBUG_FONTS)
    glBindTexture(GL_TEXTURE_2D, font->chars[character].texture);
#endif

    plClearMesh(font_mesh);

    plSetMeshVertexPosition2f(font_mesh, 0,
                              x, (y + font->chars[character].height) * scale);
    plSetMeshVertexPosition2f(font_mesh, 1,
                              x, y);
    plSetMeshVertexPosition2f(font_mesh, 2,
                              (x + font->chars[character].width) * scale, (y + font->chars[character].height) * scale);
    plSetMeshVertexPosition2f(font_mesh, 3,
                              (x + font->chars[character].width) * scale, y);

    plSetMeshVertexColour(font_mesh, 0, plCreateColour4b(255, 255, 255, 255));
    plSetMeshVertexColour(font_mesh, 1, plCreateColour4b(255, 255, 255, 255));
    plSetMeshVertexColour(font_mesh, 2, plCreateColour4b(255, 255, 255, 255));
    plSetMeshVertexColour(font_mesh, 3, plCreateColour4b(255, 255, 255, 255));

    float offset_s = (float)(1/font->width);
    float offset_t = (float)(1/font->height);

    plSetMeshVertexST(font_mesh, 0, 1, 0);
    plSetMeshVertexST(font_mesh, 1, 1, 1);
    plSetMeshVertexST(font_mesh, 2, 0, 0);
    plSetMeshVertexST(font_mesh, 3, 0, 1);

    /*
     *
     glBegin(GL_QUADS);
     glBindTexture(GL_TEXTURE_2D, m_image);
     glTexCoord2f(m_tileOffsetX, m_tileOffsetY);
     glVertex3f(m_positionX, m_positionY, 0.0f);
     glTexCoord2f(0.025f + m_tileOffsetX, m_tileOffsetY);
     glVertex3f(m_positionX+16.0f, m_positionY, 0.0f);
     glTexCoord2f(0.025f + m_tileOffsetX, 0.025f + m_tileOffsetY);
     glVertex3f(m_positionX+16.0f, m_positionY+16.0f, 0.0f);
     glTexCoord2f(m_tileOffsetX, 0.025f + m_tileOffsetY);
     glVertex3f(m_positionX, m_positionY+16.0f, 0.0f);
     glEnd();
     */

    plUploadMesh(font_mesh);
    plDrawMesh(font_mesh);

#if defined(DEBUG_FONTS)
    glBindTexture(GL_TEXTURE_2D, 0);
#endif
}

void DrawText(PIGFont *font, int x, int y, float scale, const char *msg) {
    if(x < 0 || x > g_state.width || y < 0 || y > g_state.height) {
        return;
    }

    unsigned int num_chars = (unsigned int)strlen(msg);
    if(!num_chars) {
        return;
    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    glBlendFunc(GL_ONE, GL_ONE);

    glBindTexture(GL_TEXTURE_2D, font->texture);

    for(unsigned int i = 0; i < num_chars; i++) {
        if((i > 0) && !(msg[i - 1] < 33 || msg[i - 1] > 122)) {
            x += font->chars[msg[i - 1]].width;
        }

        DrawCharacter(font, x, y, scale, (uint8_t)msg[i]);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

PIGFont *CreateFont(const char *path, const char *tab_path) {
    PIGFont *font = (PIGFont*)malloc(sizeof(PIGFont));
    if(!font) {
        PRINT_ERROR("Failed to allocate memory for PIGFont!\n");
    }

    FILE *file = fopen(tab_path, "rb");
    if(!file) {
        PRINT_ERROR("Failed to load file %s!\n", tab_path);
    }

    fseek(file, 16, SEEK_SET);

    TABIndex indices[FONT_CHARACTERS];
    font->num_chars = (unsigned int)fread(indices, sizeof(TABIndex), FONT_CHARACTERS, file);
    if(font->num_chars == 0) {
        PRINT_ERROR("Invalid number of characters for font! (%s)\n", tab_path);
    }

    ILuint image = ilGenImage();
    ilBindImage(image);

    if(!ilLoadImage(path)) {
        PRINT_ERROR("Failed to load image! (%s)\n", path);
    }

    font->width = (unsigned int)ilGetInteger(IL_IMAGE_WIDTH);
    font->height = (unsigned int)ilGetInteger(IL_IMAGE_HEIGHT);

    if(!ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE)) {
        PRINT_ERROR("Failed to convert image!\n");
    }

    memset(font->chars, 0, sizeof(PIGChar) * FONT_CHARACTERS);

    for(unsigned int i = 0; i < font->num_chars; i++) {
        font->chars[i].character = (unsigned char)(33 + i);
        font->chars[i].width = indices[i].width;
        font->chars[i].height = indices[i].height;

        if(i < 1) {
            continue;
        }

        font->chars[i].x = (font->chars[i - 1].width + font->chars[i - 1].x);
        font->chars[i].y = font->chars[i - 1].y;
        if(font->chars[i].x > font->width) {
            font->chars[i].y += font->chars[i].height;
            font->chars[i].x = 0;
        }

#if defined(DEBUG_FONTS)
        glGenTextures(1, &font->chars[i].texture);
        glBindTexture(GL_TEXTURE_2D, font->chars[i].texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                font->chars[i].x, font->chars[i].y,
                font->chars[i].width, font->chars[i].height,
                (GLenum)ilGetInteger(IL_IMAGE_FORMAT),
                GL_UNSIGNED_BYTE,
                ilGetData()
        );
#endif
    }

    glGenTextures(1, &font->texture);
    glBindTexture(GL_TEXTURE_2D, font->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            ilGetInteger(IL_IMAGE_FORMAT),
            font->width,
            font->height,
            0,
            (GLenum)ilGetInteger(IL_IMAGE_FORMAT),
            GL_UNSIGNED_BYTE,
            ilGetData()
    );

    ilDeleteImage(image);

    fclose(file);

    return font;
}

PIGFont *fonts[MAX_FONTS];

void InitializeFonts(void) {
    if(g_state.is_psx) {
        // todo
    } else {
        fonts[FONT_BIG]         = CreateFont("./FEText/BIG.BMP", "./FEText/BIG.tab");
        fonts[FONT_BIG_CHARS]   = CreateFont("./FEText/BigChars.BMP", "./FEText/BigChars.tab");
        fonts[FONT_CHARS2]      = CreateFont("./FEText/Chars2.bmp", "./FEText/CHARS2.tab");
        fonts[FONT_CHARS3]      = CreateFont("./FEText/CHARS3.BMP", "./FEText/chars3.tab");
        fonts[FONT_GAME_CHARS]  = CreateFont("./FEText/GameChars.bmp", "./FEText/GameChars.tab");
        fonts[FONT_SMALL]       = CreateFont("./FEText/SMALL.BMP", "./FEText/SMALL.tab");
    }
}

void ShutdownFontManager(void) {

}