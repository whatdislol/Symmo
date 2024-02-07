#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtWidgets>
#include <QtMultimedia>
#include <QFileDialog>
#include <QStyle>
#include <QAudioOutput>

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

    void on_pushButton_Volume_clicked();

    void on_pushButton_Play_clicked();

    void on_actionAdd_File_triggered();

    void on_horizontalSlider_song_duration_sliderMoved(int value);

    void on_horizontalSlider_volume_sliderMoved(int value);

private:
    Ui::MainWindow* ui;
    bool is_muted = false;
    bool is_paused = false;
    QMediaPlayer* M_Player;
    QAudioOutput* audio_output;
    qint64 Mduration;
    void updateDuration(qint64 duration);
};
#endif // MAINWINDOW_H
