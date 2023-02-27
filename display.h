#ifndef DISPLAY_H
#define DISPLAY_H

#include <QWidget>
#include <QThread>
#include <QLabel>
#include <QKeyEvent>

#include "renderer.h"

class RenderWorker;

class RenderingCanvas : public QLabel {
    Q_OBJECT
public:
    RenderingCanvas(RenderWorker *worker);
    void keyPressEvent(QKeyEvent *event);
signals:
    void keyPressed(int key);
};

class RenderWorker : public QObject {
    Q_OBJECT

private:
    Renderer renderer;

public:
    RenderWorker(Renderer a_renderer) : renderer(a_renderer) {};
public slots:
    void render();
    void keyReceived(int key);
signals:
    void renderDone(const QImage& renderedImage);
};

class Display : public QObject {
    Q_OBJECT
    QThread workerThread;
private:
    RenderingCanvas* canvas;
    bool shown = false;
public:
    Display();
    ~Display();
    void queryNextFrame();

public slots:
    void handleResults(const QImage& renderedImage);

signals:
    void operate();
};

#endif