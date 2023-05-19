
//#include <GL3/gl3.h>
//#include <GL3/gl3w.h>
#include <glad/gl.h>
#include <cstring>
// #include <GLFW/glfw3.h>
//#include <red_book/LoadShaders.h>
// #include "STB/stb_image.h"

#include "cs4722/buffer_utilities.h"



GLuint cs4722::init_buffers(GLuint program, std::vector<cs4722::artifact *> &part_list, const char *position_var,
                            const char *color_var, const char *texture_var, const char *normal_var,
                            const char *tangent_var) {

    std::vector<glm::vec4> *vBuf;
    std::vector<cs4722::color> *cBuf;
    std::vector<glm::vec2> *tBuf;
    std::vector<glm::vec4> *nBuf;
    std::vector<glm::vec4> *tgBuf;

    int number_of_buffers = 0;

    if(strlen(position_var) > 0) {
        vBuf = new std::vector<glm::vec4>();
        number_of_buffers++;
    }
    if(strlen(color_var) > 0) {
        cBuf = new std::vector<cs4722::color>();
        number_of_buffers++;
    }
    if(strlen(texture_var) > 0) {
        tBuf = new std::vector<glm::vec2>();
        number_of_buffers++;
    }
    if(strlen(normal_var) > 0) {
        nBuf = new std::vector<glm::vec4>();
        number_of_buffers++;
    }
    if(strlen(tangent_var) > 0) {
        tgBuf = new std::vector<glm::vec4>();
        number_of_buffers++;
    }



    for (auto & widget : part_list) {

//            auto* current_object = *iterator;
        auto* current_shape = widget->the_shape;

        if(current_shape->buffer_size == 0) {

            current_shape->buffer_size = (current_shape->get_size());
            current_shape->buffer_start = (vBuf->size());

            if(strlen(position_var) > 0) {
                auto *vertices = current_shape->positions();
                vBuf->insert(vBuf->end(), vertices->begin(), vertices->end());
                delete vertices;
            }

            if(strlen(color_var) > 0) {
                auto *colors = current_shape->colors();
                cBuf->insert(cBuf->end(), colors->begin(), colors->end());
                delete colors;
            }

            if(strlen(texture_var) > 0) {
                auto *texture_coordinates_list = current_shape->texture_coordinates();
                tBuf->insert(tBuf->end(),
                             texture_coordinates_list->begin(),
                             texture_coordinates_list->end());
                delete texture_coordinates_list;
            }

            if(strlen(normal_var) > 0) {
                auto *normals = current_shape->normals();
                nBuf->insert(nBuf->end(), normals->begin(), normals->end());
                delete normals;
            }
            if(strlen(tangent_var) > 0) {
                auto *tangents = current_shape->tangents();
                tgBuf->insert(tgBuf->end(), tangents->begin(), tangents->end());
                delete tangents;
            }
        }

    }

//    std::cerr << "vertex lists from shapes" << std::endl;

    glUseProgram(program);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
//    vertex_array = vao;


//    const auto number_of_buffers = 2;
    GLuint buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);
    // std::cout << "buffers " << buffers[0] << "  " << buffers[1] << std::endl;
    // std::cout << "bCOlor location " << bColor << std::endl;
    // std::cout << "bPosition location " << bPosition << std::endl;

    int buffer_num = 0;

    if(strlen(position_var) > 0) {
//        std::cerr << "position 1" << std::endl;
        auto bPosition = glGetAttribLocation(program, position_var);
//        std::cerr << "position 2" << std::endl;
        glBindBuffer(GL_ARRAY_BUFFER, buffers[buffer_num]);
//        std::cerr << "position 3" << std::endl;
        glBufferData(GL_ARRAY_BUFFER, 4 * 4 * vBuf->size(), vBuf->data(),  GL_STATIC_DRAW);
//        std::cerr << "position 4" << std::endl;
        glVertexAttribPointer(bPosition, 4, GL_FLOAT,
                              GL_FALSE, 0, 0);
//        std::cerr << "position 5" << std::endl;
        glEnableVertexAttribArray(bPosition);
//        std::cerr << "position 6" << std::endl;
        delete vBuf;
        buffer_num++;
//        std::cerr << "position buffer setup" << std::endl;
    }


    if(strlen(color_var) > 0) {
        auto bColor = glGetAttribLocation(program, color_var);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[buffer_num]);
        glBufferData(GL_ARRAY_BUFFER, 4 * cBuf->size(), cBuf->data(), GL_STATIC_DRAW);
        glVertexAttribPointer(bColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
        glEnableVertexAttribArray(bColor);
        delete cBuf;
        buffer_num++;
//        std::cerr << "color buffer setup" << std::endl;
    }


    if(strlen(texture_var) > 0) {
//        std::cerr << "texture coord 1" << std::endl;
        auto bTextureCoord = glGetAttribLocation(program, texture_var);
//        std::cerr << "texture coord 2" << std::endl;
        glBindBuffer(GL_ARRAY_BUFFER, buffers[buffer_num]);
//        std::cerr << "texture coord 3" << std::endl;
        glBufferData(GL_ARRAY_BUFFER, 4 * 2 * tBuf->size(), tBuf->data(),  GL_STATIC_DRAW);
//        std::cerr << "texture coord 4" << std::endl;
        glVertexAttribPointer(bTextureCoord, 2, GL_FLOAT,
                              GL_FALSE, 0, 0);
//        std::cerr << "texture coord 5" << std::endl;
        glEnableVertexAttribArray(bTextureCoord);
//        std::cerr << "texture coord 6" << std::endl;
        delete tBuf;
        buffer_num++;
//        std::cerr << "texture coord buffer setup" << std::endl;
    }

//    std::cerr << "init buffers done" << std::endl;


    if(strlen(normal_var) > 0) {
        auto bNormal = glGetAttribLocation(program, normal_var);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[buffer_num]);
        glBufferData(GL_ARRAY_BUFFER, 4 * 4 * nBuf->size(), nBuf->data(),  GL_STATIC_DRAW);
        glVertexAttribPointer(bNormal, 4, GL_FLOAT,
                              GL_FALSE, 0, 0);
        glEnableVertexAttribArray(bNormal);
        delete nBuf;
        buffer_num++;
    }


    if(strlen(tangent_var) > 0) {
        auto bTangent = glGetAttribLocation(program, tangent_var);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[buffer_num]);
        glBufferData(GL_ARRAY_BUFFER, 4 * 4 * tgBuf->size(), tgBuf->data(), GL_STATIC_DRAW);
        glVertexAttribPointer(bTangent, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(bTangent);
        delete tgBuf;
        buffer_num++;
    }




    return vao;

}

