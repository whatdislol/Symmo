// mainwindow.cpp

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_audioControl(new AudioControl(this)),
    m_playlistManager(new PlaylistManager(this))
{
    ui->setupUi(this);
    Playlist* currentPlaylist = m_playlistManager->getCurrentPlaylist();
    QMediaPlayer* player = m_audioControl->getMediaPlayer();

    // CONNECT UI SIGNALS TO METHODS
    // audio control
    connect(ui->slider_SongVolume, &QSlider::sliderMoved, m_audioControl, &AudioControl::setVolume);
    connect(ui->slider_SongProgress, &QSlider::sliderMoved, m_audioControl, &AudioControl::setPosition);
    connect(ui->toggleButton_PlayPause, &QPushButton::clicked, m_audioControl, &AudioControl::togglePlayPause);
    connect(ui->toggleButton_Mute, &QPushButton::clicked, m_audioControl, &AudioControl::toggleMute);
    // playlist manager
    connect(ui->pushButton_ViewAllSongs, &QPushButton::clicked, m_playlistManager, &PlaylistManager::updateDefaultPlaylist);
    connect(ui->listWidget_SongsInPlaylist, &QListWidget::itemClicked, [=](QListWidgetItem* song) {
        if (currentPlaylist && m_audioControl) {
            currentPlaylist->selectSong(song, m_audioControl);
        }
    });
    // playlist
    connect(ui->pushButton_Skip, &QPushButton::clicked, [=]() {
        if (currentPlaylist && m_audioControl) {
            currentPlaylist->toNextSong(m_audioControl);
        }
    });
    connect(ui->pushButton_Back, &QPushButton::clicked, [=]() {
        if (currentPlaylist && m_audioControl) {
            currentPlaylist->toPreviousSong(m_audioControl);
        }
    });

    // CONNECT LOGIC SIGNALS TO METHODS
    // audio control
    connect(m_audioControl->getMediaPlayer(), &QMediaPlayer::durationChanged, this, &MainWindow::setMaxDuration);
    connect(m_audioControl->getMediaPlayer(), &QMediaPlayer::positionChanged, this, &MainWindow::updateSongProgress);
    connect(m_audioControl, &AudioControl::muteStateChanged, this, &MainWindow::updateMuteIcon);
    connect(m_audioControl, &AudioControl::playPauseStateChanged, this, &MainWindow::updatePlayPauseIcon);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::updatePlaybackUI);
    connect(player, &QMediaPlayer::mediaStatusChanged, [=](QMediaPlayer::MediaStatus status) {
        if (currentPlaylist && m_audioControl) {
            currentPlaylist->skipOnSongEnd(m_audioControl, status);
        }
    });
    // playlist manager
    connect(m_playlistManager, &PlaylistManager::clearSongList, ui->listWidget_SongsInPlaylist, &QListWidget::clear);
    // playlist
    connect(currentPlaylist, &Playlist::addSongToPlaylist, this, &MainWindow::addSongToPlaylist);
    connect(currentPlaylist, &Playlist::setPlaylistName, ui->label_PlaylistName, &QLabel::setText);
    connect(currentPlaylist, &Playlist::setTrackQuantity, ui->label_TrackQuantity, &QLabel::setText);

    setupIcons();
    //m_playlistManager->updateDefaultPlaylist();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_audioControl;
    delete m_playlistManager;
}

void MainWindow::setMaxDuration(qint64 duration)
{
    qint64 totalDuration = duration / 1000;
    m_audioControl->setTotalDuration(totalDuration);
    ui->slider_SongProgress->setMaximum(totalDuration);
}

void MainWindow::updateSongProgress(qint64 progress)
{
    if (!ui->slider_SongProgress->isSliderDown()) {
        ui->slider_SongProgress->setValue(progress / 1000);
    }
    updateDuration(progress / 1000);
}

void MainWindow::updateDuration(qint64 duration)
{
    qint64 totalDuration = m_audioControl->getTotalDuration();
    if (duration || totalDuration) {
        QTime currentTime((duration / 3600) % 60, (duration / 60) % 60, duration % 60, (duration * 1000) % 1000);
        QTime totalTime((totalDuration / 3600) % 60, (totalDuration / 60) % 60, totalDuration % 60, (totalDuration * 1000) % 1000);
        QString format = "mm:ss";
        if (totalDuration > 3600) {
            format = "hh:mm:ss";
        }
        ui->label_CurrentSongDuration->setText(currentTime.toString(format));
        ui->label_TotalSongDuration->setText(totalTime.toString(format));
    }
}

void MainWindow::updatePlaybackUI(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia) {
        QMediaPlayer* M_Player = m_audioControl->getMediaPlayer();
        qint64 totalDuration = M_Player->duration() / 1000;

        if (M_Player->mediaStatus() != QMediaPlayer::NoMedia) { // Media loaded successfully
            QUrl mediaUrl = M_Player->source();
            QFileInfo fileInfo(mediaUrl.toLocalFile());
            QString fileNameWithoutExtension = fileInfo.fileName();
            fileNameWithoutExtension = fileNameWithoutExtension.left(fileNameWithoutExtension.lastIndexOf('.')); // Remove the file extension
            ui->label_fileName->setText(fileNameWithoutExtension);
            ui->toggleButton_PlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
            m_audioControl->setTotalDuration(totalDuration);
            updateDuration(0);
        }
    }
}

void MainWindow::updateMuteIcon(bool isMuted)
{
	QIcon volumeIcon = isMuted ? style()->standardIcon(QStyle::SP_MediaVolumeMuted) : style()->standardIcon(QStyle::SP_MediaVolume);
	ui->toggleButton_Mute->setIcon(volumeIcon);
}

void MainWindow::updatePlayPauseIcon(bool isPaused)
{
	QIcon playPauseIcon = isPaused ? style()->standardIcon(QStyle::SP_MediaPlay) : style()->standardIcon(QStyle::SP_MediaPause);
	ui->toggleButton_PlayPause->setIcon(playPauseIcon);
}

void MainWindow::on_actionAdd_File_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open a file", "", "Audio File (*.mp3)");
    QMediaPlayer* M_Player = m_audioControl->getMediaPlayer();

    if (!file_name.isEmpty()) {
        M_Player->setSource(QUrl::fromLocalFile(file_name));
    }
}

void MainWindow::setupIcons()
{
    ui->toggleButton_PlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->toggleButton_Mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    ui->pushButton_Skip->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
	ui->pushButton_Back->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
}

void MainWindow::addSongToPlaylist(QListWidgetItem* song)
{
	ui->listWidget_SongsInPlaylist->addItem(song);
}