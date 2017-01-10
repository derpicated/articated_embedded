// window.h

#ifndef WINDOW_H
#define WINDOW_H

#include <QBoxLayout>
#include <QGridLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QStatusBar>
#include <QTimer>

#include "augmentation_widget.hpp"

namespace Ui {
class Window;
}

class Window : public QWidget {
    Q_OBJECT

    public:
    explicit Window (QWidget* parent = 0);
    ~Window ();
    void resizeEvent (QResizeEvent* event);
    QSize minimumSizeHint () const;
    QSize sizeHint () const;

    public slots:
    /* On timer timeout, do ARticated things */
    void timeout ();
    void btn_pause_clicked ();
    void btn_reference_clicked ();

    protected:
    void keyPressEvent (QKeyEvent* event);

    private:
    void update_ui_style ();
    void set_framerate (int framerate = -1);

    QTimer _frame_timer;

    // ui elements
    QGridLayout _layout;
    QHBoxLayout _layout_back;          // background
    QHBoxLayout _layout_ui;            // foreground
    QVBoxLayout _layout_buttons;       // buttons
    QVBoxLayout _layout_status;        // status bar
    augmentation_widget _augmentation; // augmeted
    QPushButton _btn_reference;
    QPushButton _btn_pause;

    QStatusBar _statusbar;
};

#endif // WINDOW_H
