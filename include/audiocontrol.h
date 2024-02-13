#ifndef AUDIOCONTROLS_H
#define AUDIOCONTROLS_H

#include <QObject>
#include <QtCore>
#include <QtWidgets>
#include <QtMultimedia>
#include <QStyle>
#include <QAudioOutput>
#include "mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class AudioControl : public QObject {
    Q_OBJECT
public:
    AudioControl(QWidget* parent, Ui::MainWindow* ui, QStyle* style);
    ~AudioControl();
    void setMediaPlayer(QMediaPlayer* player);

private slots:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void on_pushButton_Play_clicked();
    void on_pushButton_Volume_clicked();
    void on_horizontalSlider_SongProgress_sliderMoved(int value);
    void on_horizontalSlider_SongVolume_sliderMoved(int value);

private:
    Ui::MainWindow* ui;
    QStyle* style;
    bool isMuted = false;
    bool isPaused = false;
    QMediaPlayer* M_Player;
    QAudioOutput* audio_Output;
    qint64 M_Duration;
    void updateDuration(qint64 duration);
};

#endif // AUDIOCONTROLS_H
