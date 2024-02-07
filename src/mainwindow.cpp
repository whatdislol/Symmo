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
    Mduration = M_Player->duration() / 1000;

    //default volume
    ui->horizontalSlider_volume->setMinimum(0);
    ui->horizontalSlider_volume->setMaximum(100);
    ui->horizontalSlider_volume->setValue(50);
    audio_output->setVolume(0.5);

    // Connect volume to slider
    connect(ui->horizontalSlider_volume, &QSlider::sliderMoved, this, &MainWindow::on_horizontalSlider_volume_sliderMoved);

    // Duration slider
    connect(M_Player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(M_Player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);

    ui->horizontalSlider_song_duration->setRange(0, M_Player->duration() / 1000);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete M_Player;
    delete audio_output;
}

void MainWindow::durationChanged(qint64 duration)
{
	Mduration = duration / 1000;
    ui->horizontalSlider_song_duration->setMaximum(Mduration);
}

void MainWindow::positionChanged(qint64 progress)
{
    if (!ui->horizontalSlider_song_duration->isSliderDown()) {
		ui->horizontalSlider_song_duration->setValue(progress / 1000);
	}
    updateDuration(progress / 1000);
}

void MainWindow::updateDuration(qint64 duration)
{
    QString timestr;
    if (duration || Mduration) {
        QTime currentTime((duration / 3600) % 60, (duration / 60) % 60, duration % 60, (duration * 1000) % 1000);
        QTime totalTime((Mduration / 3600) % 60, (Mduration / 60) % 60, Mduration % 60, (Mduration * 1000) % 1000);
        QString format = "mm:ss";
        if (Mduration > 3600) {
			format = "hh:mm:ss";
		}
        ui->timeLabel_left->setText(currentTime.toString(format));
        ui->timeLabel->setText(totalTime.toString(format));
    }
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

void MainWindow::on_horizontalSlider_song_duration_sliderMoved(int value)
{
    M_Player->setPosition(static_cast<qint64>(value) * 1000);
}

void MainWindow::on_horizontalSlider_volume_sliderMoved(int value)
{
    audio_output->setVolume(value / 100.0);

    if (value == 0) {
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    }
    else {
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    }
}
