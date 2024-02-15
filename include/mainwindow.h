// mainwindow.h

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QStyle>
#include <QSlider>
#include "audiocontrol.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void updateDuration(qint64 duration);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void updateMuteIcon(bool isMuted);
    void updatePlayPauseIcon(bool isPaused);
    void on_actionAdd_File_triggered();
    void setupIcons();

private:
    Ui::MainWindow* ui;
    AudioControl* m_audioControl;
};

#endif // MAINWINDOW_H
