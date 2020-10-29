// augmentation_view.hpp

#ifndef AUGMENTATION_VIEW_HPP
#define AUGMENTATION_VIEW_HPP

#include <QRunnable>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>

#include "augmentation_renderer.hpp"
#include "shared/frame_data.hpp"

class CleanupJob : public QRunnable {
    public:
    CleanupJob (AugmentationRenderer* renderer)
    : renderer_ (renderer) {
    }
    void run () override {
        delete renderer_;
    }

    private:
    AugmentationRenderer* renderer_;
};

class AugmentationView : public QQuickItem {
    Q_OBJECT

    Q_PROPERTY (QStringList models MEMBER models_ NOTIFY modelsChanged)

    public:
    AugmentationView ();
    ~AugmentationView ();

    signals:
    void modelsChanged ();

    public slots:
    void sync ();
    void cleanup ();
    void drawFrame (FrameData frame);
    void LoadObject (const QString& path);

    private slots:
    void handleWindowChanged (QQuickWindow* win);

    private:
    void releaseResources () override;
    void readModels ();

    AugmentationRenderer* renderer_;
    QString object_path_;
    QStringList models_;
    Movement3D transform_;
    GLuint background_texture_{ 0 };
};

#endif // AUGMENTATION_VIEW_HPP