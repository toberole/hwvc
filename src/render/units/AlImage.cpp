/*
* Copyright (c) 2018-present, aliminabc@gmail.com.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/

#include "AlImage.h"
#include "HwBitmapFactory.h"
#include "HwTexture.h"
#include "ObjectBox.h"

AlImage::AlImage(string alias) : Unit(alias) {
    registerEvent(EVENT_COMMON_INVALIDATE, reinterpret_cast<EventFunc>(&AlImage::onInvalidate));
    registerEvent(EVENT_AIMAGE_UPDATE_LAYER, reinterpret_cast<EventFunc>(&AlImage::onUpdateLayer));
    registerEvent(EVENT_AIMAGE_UPDATE_CANVAS,
                  reinterpret_cast<EventFunc>(&AlImage::onUpdateCanvas));
    registerEvent(EVENT_AIMAGE_SAVE, reinterpret_cast<EventFunc>(&AlImage::onSave));
}

AlImage::~AlImage() {
    this->onSaveListener = nullptr;
}

bool AlImage::onCreate(Message *msg) {
    texAllocator = new TextureAllocator();
    return true;
}

bool AlImage::onDestroy(Message *msg) {
    mCanvas.release();
    mLayerManager.release();
    if (texAllocator) {
        delete texAllocator;
        texAllocator = nullptr;
    }
    return true;
}

bool AlImage::onUpdateCanvas(Message *m) {
    auto model = getCanvas();
    mCanvas.update(model->getWidth(), model->getHeight(), model->getColor(), texAllocator);
    onInvalidate(nullptr);
    return true;
}

bool AlImage::onUpdateLayer(Message *msg) {
    mLayerManager.update(getLayers(), texAllocator);
    _newDefaultCanvas();
    onInvalidate(nullptr);
    return true;
}

bool AlImage::onInvalidate(Message *m) {
    _drawAllLayer();
    Message *msg = new Message(EVENT_RENDER_FILTER, nullptr);
    auto tex = mCanvas.getOutput();
    if (nullptr == tex) {
        Logcat::e("AlImage", "%s(%d): Empty canvas", __FUNCTION__, __LINE__);
        return true;
    }
    msg->obj = HwTexture::wrap(tex->target(), tex->texId(),
                               tex->getWidth(),
                               tex->getHeight(),
                               tex->fmt());
    postEvent(msg);
    return true;
}

bool AlImage::onSave(Message *m) {
    std::string path = getString("output_path");
    if (path.empty()) {
        if (onSaveListener) {
            onSaveListener(Hw::FAILED.code, "Failed", path.c_str());
        }
        return true;
    }
    _drawAllLayer(true);
    auto output = mCanvas.getOutput();
    size_t size = static_cast<size_t>(output->getWidth() * output->getHeight() * 4);
    AlBuffer *buf = AlBuffer::alloc(size);
    mCanvas.read(buf);
    HwBitmapFactory::save(output->getWidth(), output->getHeight(), buf, path);
    delete buf;
    if (onSaveListener) {
        onSaveListener(Hw::SUCCESS.code, "Success", path.c_str());
    }
    return true;
}

/*+------------------+*/
/*|     Model        |*/
/*+------------------+*/
AlImageCanvasModel *AlImage::getCanvas() {
    return dynamic_cast<AlImageCanvasModel *>(getObject("canvas"));
}

std::vector<AlImageLayerModel *> *AlImage::getLayers() {
    auto *obj = static_cast<ObjectBox *>(getObject("layers"));
    return static_cast<vector<AlImageLayerModel *> *>(obj->ptr);
}

void AlImage::_newDefaultCanvas() {
    if (mLayerManager.empty()) {
        return;
    }
    auto model = getCanvas();
    if (model->getWidth() > 0 && model->getHeight() > 0) {
        return;
    }
    auto layer = mLayerManager.getLayer(0);
    model->set(layer->getWidth(), layer->getHeight(), 0);
    mCanvas.update(model->getWidth(), model->getHeight(), model->getColor(), texAllocator);
}

void AlImage::_drawAllLayer(bool showBg) {
    mCanvas.clear(showBg);
    if (!mLayerManager.empty()) {
        int size = mLayerManager.size();
        for (int i = 0; i < size; ++i) {
            mCanvas.draw(mLayerManager.getLayer(i));
        }
    }
}

void AlImage::setOnSaveListener(AlImage::OnSaveListener listener) {
    this->onSaveListener = listener;
}