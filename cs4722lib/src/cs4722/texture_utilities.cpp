//
// Created by Ben on 11/11/2020.
//

#include <cstring>
#include "cs4722/texture_utilities.h"
#include "STB/stb_image.h"




GLuint cs4722::init_texture_from_file(const char* path, const GLuint texture_unit)
{


    const auto internal_format = GL_RGBA8;

    GLuint texture;
    glGenTextures(1, &texture);

    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_2D, texture);

    int texture_width, texture_height, n;
    auto* texture_data = stbi_load(path, &texture_width, &texture_height, &n, 0);
    const auto external_format = (n == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D,
                 0, // level
                 internal_format,
                 texture_width, texture_height,
                 0, // border
                 external_format,
                 GL_UNSIGNED_BYTE,
                 texture_data
    );
    delete texture_data;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}


//GLuint cs4722::init_textures_from_files(std::vector<std::string> paths, const GLuint start_unit)
//{
//    // const auto base_texture_unit = GL_TEXTURE0;
//
////    auto paths = std::vector<std::string>({
////                                                  "../media/square-2703542_512x512.png",
////                                                  "../media/square-background-1456646_1024.png",
////                                                  "../media/tulips-bed-colorful-color-69776.png",
////                                          });
//
//    const auto internal_format = GL_RGBA8;
//
//    GLuint* textures = new GLuint[paths.size()];
//    glGenTextures(paths.size(), textures);
//
//    for (unsigned int face = 0; face < paths.size(); face++)
//    {
//        GLuint texture;
//        glGenTextures(1, &texture);
//
//        glActiveTexture(GL_TEXTURE0 + start_unit + face);
//        glBindTexture(GL_TEXTURE_2D, texture);
//
//        int texture_width, texture_height, n;
//        auto* texture_data = stbi_load(paths[face].c_str(),
//                                       &texture_width, &texture_height, &n, 0);
//        const auto external_format = (n == 3) ? GL_RGB : GL_RGBA;
//        glTexImage2D(GL_TEXTURE_2D,
//                     0, // level
//                     internal_format,
//                     texture_width, texture_height,
//                     0, // border
//                     external_format,
//                     GL_UNSIGNED_BYTE,
//                     texture_data
//        );
//        delete texture_data;
//
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//    }
//
//    return paths.size() + start_unit;
//}

/*   *
 * Creates a gradient pattern of color as a texture
 *
 * @param start_unit Texture unit to use for this texture
 * @return
 */
GLuint cs4722::init_texture_computed(int texture_unit, int texture_size,
                                   cs4722::color color1, cs4722::color color2) {


    auto texture_width = texture_size;
    auto texture_height = texture_size;

    auto texture_data = new std::vector<GLubyte>();
    // create data
    for (int r = 0; r < texture_height; r++) {
        for (int c = 0; c < texture_width; c++) {
            texture_data->push_back(color1.r * r / texture_height + color2.r * c / texture_width);
            texture_data->push_back(color1.g * r / texture_height + color2.g * c / texture_width);
            texture_data->push_back(color1.b * r / texture_height + color2.b * c / texture_width);
            texture_data->push_back(255);
//            texture_data->push_back(r * 255 / texture_height);
//            texture_data->push_back(c * 255 / texture_width);
//            texture_data->push_back(0);
        }
    }


    glActiveTexture(GL_TEXTURE0 + texture_unit);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    auto number_of_levels = 1;
    auto internal_format = GL_RGBA8;
    auto external_format = GL_RGBA;


//    glTexStorage2D(GL_TEXTURE_2D, number_of_levels, internal_format, texture_width, texture_height);
//    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture_width, texture_height,
//                        external_format, GL_UNSIGNED_BYTE, texture_data->data());
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, texture_width, texture_height, 0, external_format,
                    GL_UNSIGNED_BYTE, texture_data->data());

//    glBindTextureUnit(texture_unit, texture);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, cs4722::x11::aquamarine.as_float().get());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    auto mag_filter = GL_NEAREST;
    // auto mag_filter = GL_LINEAR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
    // auto wrap_type = GL_CLAMP_TO_EDGE;
    //auto wrap_type = GL_MIRRORED_REPEAT;
    auto wrap_type = GL_REPEAT;
    //auto wrap_type = GL_CLAMP_TO_BORDER;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_type);

//    return start_unit + 1;
    return texture;
}

GLuint cs4722::init_cube_texture_from_file(std::vector<std::string> &paths, int environment_unit)
{
    glActiveTexture(GL_TEXTURE0 + environment_unit);
    GLuint cube_texture;
    glGenTextures(1, &cube_texture);
    // glBindTextureUnit(GL_TEXTURE31, cube_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cube_texture);


//        auto  paths = std::vector<std::string>({
//                                                       "../media/fjaderholmarna/posx.png",
//                                                       "../media/fjaderholmarna/negx.png",
//                                                       "../media/fjaderholmarna/posy.png",
//                                                       "../media/fjaderholmarna/negy.png",
//                                                       "../media/fjaderholmarna/posz.png",
//                                                       "../media/fjaderholmarna/negz.png",
//                                               });




    const auto internal_format = GL_RGBA8;


    for (unsigned int face = 0; face < paths.size(); face++)
    {
        int texture_width, texture_height, n;
        auto path = paths.at(face);
        auto* texture_data = stbi_load(path.c_str(),
                                       &texture_width, &texture_height, &n, 0);
//            std::cout << "path " << path << "             number of components " << n << std::endl;
//            std::cout << "width and height: " << texture_width << "   " << texture_height << std::endl;
        const auto external_format = (n == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                     0, // level
                     internal_format,
                     texture_width, texture_height,
                     0, // border
                     external_format,
                     GL_UNSIGNED_BYTE,
                     texture_data
        );
        delete texture_data;
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return cube_texture;
}


GLuint cs4722::init_cube_texture_from_path(const char* base_path, int environment_unit, const char* ext) {
    std::vector<std::string> plist;
    auto blen = strlen(base_path);
    auto elen = strlen(ext);
    const char* fnames[] = {"posx", "negx", "posy", "negy", "posz", "negz"};
    for(int i = 0; i < 6; i++ ) {
        char posx[blen+elen+6];
        strcpy(posx, base_path);
        strcpy(posx + blen, "/");
        strcpy(posx+blen+1, fnames[i]);
        strcpy(posx+blen+5,".");
        strcpy(posx+blen+6,ext);
        *(posx+blen+6+elen) = '\0';
//        printf("%s\n", posx);
        plist.push_back(std::string(posx));
    }
    return cs4722::init_cube_texture_from_file(plist,environment_unit);
}

