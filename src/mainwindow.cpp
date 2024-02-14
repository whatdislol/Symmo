// mainwindow.cpp

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_audioControl(new AudioControl(this))
{
    ui->setupUi(this);

    // Connect UI signals to playback controller slots
    connect(ui->slider_SongVolume, &QSlider::sliderMoved, m_audioControl, &AudioControl::setVolume);
    connect(ui->toggleButton_PlayPause, &QPushButton::clicked, m_audioControl, &AudioControl::togglePlayPause);
    connect(ui->slider_SongProgress, &QSlider::sliderMoved, m_audioControl, &AudioControl::setPosition);

    // Connect playback controller signals to update UI
    connect(m_audioControl, &AudioControl::durationChanged, this, &MainWindow::durationChanged);
    connect(m_audioControl, &AudioControl::positionChanged, this, &MainWindow::positionChanged);

    // Connect playback controller signal to update icons
    connect(m_audioControl, &AudioControl::positionChanged, this, &MainWindow::updateIcons);
    connect(m_audioControl, &AudioControl::togglePlayPause, this, &MainWindow::updateIcons);
    connect(m_audioControl, &AudioControl::toggleMute, this, &MainWindow::updateIcons);

    // Initial setup of icons
    updateIcons();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_audioControl;
}

void MainWindow::durationChanged(qint64 duration)
{
    ui->slider_SongProgress->setMaximum(duration / 1000);
}

void MainWindow::positionChanged(qint64 progress)
{
    if (!ui->slider_SongProgress->isSliderDown()) {
        ui->slider_SongProgress->setValue(progress / 1000);
    }
}

void MainWindow::on_actionAdd_File_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open a file", "", "Audio File (*.mp3)");
    QMediaPlayer* M_Player = m_audioControl->getMediaPlayer();

    if (!file_name.isEmpty()) {
        M_Player->setSource(QUrl::fromLocalFile(file_name));

        if (M_Player->mediaStatus() != QMediaPlayer::NoMedia) {
            // Media loaded successfully
            QFileInfo fileInfo(file_name);
            QString fileNameWithoutExtension = fileInfo.fileName();
            fileNameWithoutExtension = fileNameWithoutExtension.left(fileNameWithoutExtension.lastIndexOf('.')); // Remove the file extension
            ui->label_fileName->setText(fileNameWithoutExtension);
        }
        else {
            // Handle error loading media
            qDebug() << "Error loading media file:" << M_Player->errorString();
        }
    }
}

void MainWindow::on_horizontalSlider_volume_sliderMoved(int value)
{
    m_audioControl->setVolume(value);
}

void MainWindow::on_horizontalSlider_song_duration_sliderMoved(int value)
{
    m_audioControl->setPosition(value * 1000);
}

void MainWindow::on_pushButton_Volume_clicked()
{
    m_audioControl->toggleMute();
}

void MainWindow::on_pushButton_Play_clicked()
{
    m_audioControl->togglePlayPause();
}

void MainWindow::updateIcons()
{
    // Update play/pause button icon
    QIcon playPauseIcon = m_audioControl->isPaused() ? style()->standardIcon(QStyle::SP_MediaPlay) : style()->standardIcon(QStyle::SP_MediaPause);
    ui->toggleButton_PlayPause->setIcon(playPauseIcon);

    // Update volume button icon
    QIcon volumeIcon = m_audioControl->isMuted() ? style()->standardIcon(QStyle::SP_MediaVolumeMuted) : style()->standardIcon(QStyle::SP_MediaVolume);
    ui->toggleButton_Mute->setIcon(volumeIcon);
}
