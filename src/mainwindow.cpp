#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QIcon>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int iconSize = 25;

    QIcon playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
    QIcon skipIcon = style()->standardIcon(QStyle::SP_MediaSeekForward);
    QIcon backIcon = style()->standardIcon(QStyle::SP_MediaSeekBackward);
    QIcon volumeIcon = style()->standardIcon((QStyle::SP_MediaVolume));

    // Get the actual size of the icon without scaling
    QSize actualSize = playIcon.actualSize(QSize(iconSize, iconSize));

    // set size for button
    ui->pushButton_Play->setIcon(playIcon);
    ui->pushButton_Play->setIconSize(actualSize);
    ui->pushButton_Skip->setIcon(skipIcon);
    ui->pushButton_Skip->setIconSize(actualSize);
    ui->pushButton_Back->setIcon(backIcon);
    ui->pushButton_Back->setIconSize(actualSize);
    ui->pushButton_Volume->setIcon(volumeIcon);
    ui->pushButton_Volume->setIconSize(actualSize);

    this->M_Player = new QMediaPlayer();
    this->audio_output = new QAudioOutput();

    M_Player->setAudioOutput(audio_output);

    //default volume
    audio_output->setVolume(50);
    ui->horizontalSlider_volume->setValue(50);

    // Connect volume to slider
    connect(ui->horizontalSlider_volume, &QSlider::valueChanged, this, &MainWindow::on_horizontalSlider_volume_valueChanged);

    // Duration slider
    connect(M_Player, &QMediaPlayer::durationChanged, ui->horizontalSlider_song_duration, &QSlider::setMaximum);
    connect(M_Player, &QMediaPlayer::positionChanged, ui->horizontalSlider_song_duration, &QSlider::setValue);

    // Time Label
    connect(M_Player, &QMediaPlayer::durationChanged, this, &MainWindow::setTimeLabel);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete M_Player;
    delete audio_output;
}

void MainWindow::on_pushButton_Volume_clicked()
{
    if (!is_muted) {
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        audio_output->setMuted(true);  // Mute the audio
        is_muted = true;
    }
    else {
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        audio_output->setMuted(false);  // Unmute the audio
        is_muted = false;
    }
}

void MainWindow::on_pushButton_Play_clicked()
{
    if (!is_paused) {
        ui->pushButton_Play->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        M_Player->play();
        is_paused = true;
    }
    else {
        ui->pushButton_Play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        M_Player->pause();
        is_paused = false;
    }
}


void MainWindow::on_actionAdd_File_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open a file", "", "Audio File (*.mp3)");

    if (!file_name.isEmpty()) {
        M_Player->setSource(QUrl::fromLocalFile(file_name));

        if (M_Player->mediaStatus() != QMediaPlayer::NoMedia) {
            // Media loaded successfully
            QFileInfo fileInfo(file_name);
            ui->label_filename->setText(fileInfo.fileName());
        }
        else {
            // Handle error loading media
            qDebug() << "Error loading media file:" << M_Player->errorString();
        }
    }
}

void MainWindow::on_horizontalSlider_song_duration_valueChanged(int value)
{
}

void MainWindow::on_horizontalSlider_volume_valueChanged(int value)
{
    audio_output->setVolume(value);

    if (value == 0) {
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    }
    else {
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    }
}

void MainWindow::setTimeLabel()
{
    // Convert duration into HH:MM format and set the label
    qint64 milli = M_Player->duration();
    qint64 min = milli / 60000;
    milli = milli - 60000 * min;
    qint64 sec = milli / 1000;

    QString timeLbl = QString("%1:%2")
        .arg(min, 2, 10, QLatin1Char('0'))  // Two digits, zero-padded
        .arg(sec, 2, 10, QLatin1Char('0')); // Two digits, zero-padded

    ui->timeLabel->setText(timeLbl);
}

