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
    void on_pushButton_Volume_clicked();
    void on_pushButton_Play_clicked();
    void on_actionAdd_File_triggered();
    void on_horizontalSlider_song_duration_sliderMoved(int value);
    void on_horizontalSlider_volume_sliderMoved(int value);
    void updateIcons(); // Added slot to update icons

private:
    Ui::MainWindow* ui;
    AudioControl* m_audioControl;
};

#endif // MAINWINDOW_H
