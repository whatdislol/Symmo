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
    ui->toggleButton_PlayPause->setIcon(playIcon);
    ui->toggleButton_PlayPause->setIconSize(actualSize);
    ui->pushButton_Skip->setIcon(skipIcon);
    ui->pushButton_Skip->setIconSize(actualSize);
    ui->pushButton_Back->setIcon(backIcon);
    ui->pushButton_Back->setIconSize(actualSize);
    ui->toggleButton_Mute->setIcon(volumeIcon);
    ui->toggleButton_Mute->setIconSize(actualSize);

    this->M_Player = new QMediaPlayer();
    this->audio_Output = new QAudioOutput();

    M_Player->setAudioOutput(audio_Output);
    M_Duration = M_Player->duration() / 1000;

    //default volume
    ui->slider_SongVolume->setMinimum(0);
    ui->slider_SongVolume->setMaximum(100);
    ui->slider_SongVolume->setValue(50);
    audio_Output->setVolume(0.5);

    // Connect volume to slider
    connect(ui->slider_SongVolume, &QSlider::sliderMoved, this, &MainWindow::on_horizontalSlider_SongVolume_sliderMoved);

    // Duration slider
    connect(M_Player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(M_Player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);

    ui->slider_SongProgress->setRange(0, M_Player->duration() / 1000);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete M_Player;
    delete audio_Output;
}

void MainWindow::durationChanged(qint64 duration)
{
	M_Duration = duration / 1000;
    ui->slider_SongProgress->setMaximum(M_Duration);
}

void MainWindow::positionChanged(qint64 progress)
{
    if (!ui->slider_SongProgress->isSliderDown()) {
		ui->slider_SongProgress->setValue(progress / 1000);
	}
    updateDuration(progress / 1000);
}

void MainWindow::updateDuration(qint64 duration)
{
    QString timestr;
    if (duration || M_Duration) {
        QTime currentTime((duration / 3600) % 60, (duration / 60) % 60, duration % 60, (duration * 1000) % 1000);
        QTime totalTime((M_Duration / 3600) % 60, (M_Duration / 60) % 60, M_Duration % 60, (M_Duration * 1000) % 1000);
        QString format = "mm:ss";
        if (M_Duration > 3600) {
			format = "hh:mm:ss";
		}
        ui->label_CurrentSongDuration->setText(currentTime.toString(format));
        ui->label_TotalSongDuration->setText(totalTime.toString(format));
    }
}

void MainWindow::on_pushButton_Volume_clicked()
{
    if (!isMuted) {
        ui->toggleButton_Mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        audio_Output->setMuted(true);  // Mute the audio
        isMuted = true;
    }
    else {
        ui->toggleButton_Mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        audio_Output->setMuted(false);  // Unmute the audio
        isMuted = false;
    }
}

void MainWindow::on_pushButton_Play_clicked()
{
    if (!isPaused) {
        ui->toggleButton_PlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        M_Player->play();
        isPaused = true;
    }
    else {
        ui->toggleButton_PlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        M_Player->pause();
        isPaused = false;
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
            ui->label_fileName->setText(fileInfo.fileName());
        }
        else {
            // Handle error loading media
            qDebug() << "Error loading media file:" << M_Player->errorString();
        }
    }
}

void MainWindow::on_horizontalSlider_SongProgress_sliderMoved(int value)
{
    M_Player->setPosition(static_cast<qint64>(value) * 1000);
}

void MainWindow::on_horizontalSlider_SongVolume_sliderMoved(int value)
{
    audio_Output->setVolume(value / 100.0);

    if (value == 0) {
        ui->toggleButton_Mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    }
    else {
        ui->toggleButton_Mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    }
}

void MainWindow::on_pushButton_AddPlaylist_clicked()
{
    // Open a dialog to get the name of the new item from the user
    QString newPlaylistName = QInputDialog::getText(this, tr("Add Playlist"), tr("Enter the name of the new playlist:"));

    // Handle Error
    if (!newPlaylistName.isEmpty()) {
        // Create a new item with the text entered by the user
        QListWidgetItem* newPlaylist = new QListWidgetItem(newPlaylistName);

        // Add item to playlist
        ui->listWidget_Playlist->addItem(newPlaylist);
    }
}
void MainWindow::on_pushButton_RemovePlaylist_clicked()
{

}

