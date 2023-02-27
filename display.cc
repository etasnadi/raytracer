#include <iostream>
#include <chrono>

#include <QKeyEvent>
#include <QWidget>

#include "display.h"
#include "renderer.h"

RenderingCanvas::RenderingCanvas(RenderWorker *worker){
    connect(this, &RenderingCanvas::keyPressed, worker, &RenderWorker::keyReceived);
}

void RenderingCanvas::keyPressEvent(QKeyEvent *event){
    emit keyPressed(event -> key());
}

QColor getCol(const glm::vec3& colVec){
    return QColor(int(colVec.x*255), int(colVec.y*255), int(colVec.z*255));
}

void RenderWorker::render(){
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::cout << "Rendering..." << std::endl;
    
    ColorBuffer<glm::vec3> colBuff = renderer.render();

    QImage im(colBuff.w, colBuff.h, QImage::Format_RGB32);

    for(int x = 0; x < colBuff.w; x++){
        for(int y = 0; y < colBuff.h; y++){
            glm::vec3 c = colBuff.getPixel(x, y);
            QColor col = getCol(c);
            im.setPixel(x, y, col.rgb());
        }
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "Done " << ms << " ms. [" << 1000./float(ms) << " fps]" << std::endl;

    emit renderDone(im);
}

void RenderWorker::keyReceived(int key){
    if (key == Qt::Key_Left){
        renderer.rotateLeft();
        this->render();
    }
    
    if (key == Qt::Key_Right){
        renderer.rotateRight();
        this->render();
    }

    if (key == Qt::Key_Down){
        renderer.rotateDown();
        this->render();;
    }
    
    if (key == Qt::Key_Up){
        renderer.rotateUp();
        this->render();
    }
}

Display::Display(){
    Renderer renderer(512, 512);
    RenderWorker *worker = new RenderWorker(renderer);
    canvas = new RenderingCanvas(worker);
    
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &Display::operate, worker, &RenderWorker::render);
    connect(worker, &RenderWorker::renderDone, this, &Display::handleResults);
    workerThread.start();
}

void Display::queryNextFrame(){
    emit operate();
}

Display::~Display() {
    workerThread.quit();
    workerThread.wait();
}

void Display::handleResults(const QImage& renderedImage){
    canvas->setPixmap(QPixmap::fromImage(renderedImage));
    if(!shown){
        canvas->show();
    }
}