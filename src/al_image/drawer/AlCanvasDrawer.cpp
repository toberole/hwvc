/*
* Copyright (c) 2018-present, aliminabc@gmail.com.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/

#include "AlCanvasDrawer.h"
#include "Logcat.h"
#include "AlVec4.h"

#define TAG "AlCanvasDrawer"

AlCanvasDrawer::AlCanvasDrawer() : HwAbsFilter() {
    _resetUV();
}

AlCanvasDrawer::~AlCanvasDrawer() {

}

HwProgram *AlCanvasDrawer::createProgram() {
    string vertex("attribute vec4 aPosition;\n"
                  "attribute vec4 aTextureCoord;\n"
                  "uniform mat4 uTextureMatrix;\n"
                  "varying vec2 vTextureCoord;\n"
                  "void main() {\n"
                  "    gl_Position = uTextureMatrix * aPosition;\n"
                  "    vTextureCoord = aTextureCoord.xy;\n"
                  "}");
    string fragment("precision mediump float;\n"
                    "varying mediump vec2 vTextureCoord;\n"
                    "uniform sampler2D uTexture;\n"
                    "void main() {\n"
                    "    vec4 color = texture2D(uTexture, vTextureCoord);\n"
                    "    gl_FragColor = color;\n"
                    "}");
    return HwProgram::create(&vertex, &fragment);
}

void AlCanvasDrawer::drawStart(HwProgram *program, HwAbsTexture *src, HwAbsTexture *dest) {
    HwAbsFilter::drawStart(program, src, dest);
    program->updateMatrix(&matrix);
    program->updateLocation(uv, vertex);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
}

void AlCanvasDrawer::drawEnd(HwProgram *program, HwAbsTexture *src, HwAbsTexture *dest) {
    HwAbsFilter::drawEnd(program, src, dest);
    glDisable(GL_BLEND);
}

void AlCanvasDrawer::setAlpha(float alpha) {
    this->alpha = alpha;
}

void AlCanvasDrawer::setMatrix(AlMatrix &matrix) {
    this->matrix = matrix;
}

void AlCanvasDrawer::setVertexRectF(AlRectF &rectF) {
    this->vertex[0] = rectF.left;
    this->vertex[1] = rectF.bottom;
    this->vertex[2] = rectF.right;
    this->vertex[3] = rectF.bottom;
    this->vertex[4] = rectF.left;
    this->vertex[5] = rectF.top;
    this->vertex[6] = rectF.right;
    this->vertex[7] = rectF.top;
}

void AlCanvasDrawer::setPositionQuad(AlQuad &quad) {
    if (!quad.isZero()) {
        this->uv[0] = quad.leftBottom().x;
        this->uv[1] = quad.leftBottom().y;
        this->uv[2] = quad.rightBottom().x;
        this->uv[3] = quad.rightBottom().y;
        this->uv[4] = quad.leftTop().x;
        this->uv[5] = quad.leftTop().y;
        this->uv[6] = quad.rightTop().x;
        this->uv[7] = quad.rightTop().y;
    } else {
        _resetUV();
    }
}

void AlCanvasDrawer::_resetUV() {
    this->uv[0] = 0.0f;
    this->uv[1] = 0.0f;
    this->uv[2] = 1.0f;
    this->uv[3] = 0.0f;
    this->uv[4] = 0.0f;
    this->uv[5] = 1.0f;
    this->uv[6] = 1.0f;
    this->uv[7] = 1.0f;
}