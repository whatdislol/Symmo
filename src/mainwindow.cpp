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

    // CONNECT UI SIGNALS TO METHODS
    // audio control
    connect(ui->slider_SongVolume, &QSlider::sliderMoved, m_audioControl, &AudioControl::setVolume);
    connect(ui->slider_SongProgress, &QSlider::sliderMoved, m_audioControl, &AudioControl::setPosition);
    connect(ui->toggleButton_PlayPause, &QPushButton::clicked, m_audioControl, &AudioControl::togglePlayPause);
    connect(ui->toggleButton_Mute, &QPushButton::clicked, m_audioControl, &AudioControl::toggleMute);
    // playlist manager
    connect(ui->pushButton_ViewAllSongs, &QPushButton::clicked, m_playlistManager, &PlaylistManager::updateDefaultPlaylist);
    connect(ui->listWidget_SongsInPlaylist, &QListWidget::itemClicked, this, &MainWindow::loadSelectedSong);
    // playlist
    connect(ui->pushButton_Skip, &QPushButton::clicked, this, &MainWindow::onSkipButtonClicked);
    connect(ui->pushButton_Back, &QPushButton::clicked, this, &MainWindow::onPreviousButtonClicked);

    // CONNECT LOGIC SIGNALS TO METHODS
    // audio control
    connect(m_audioControl->getMediaPlayer(), &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(m_audioControl->getMediaPlayer(), &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    connect(m_audioControl, &AudioControl::muteStateChanged, this, &MainWindow::updateMuteIcon);
    connect(m_audioControl, &AudioControl::playPauseStateChanged, this, &MainWindow::updatePlayPauseIcon);
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

void MainWindow::durationChanged(qint64 duration)
{
    qint64 totalDuration = duration / 1000;
    m_audioControl->setTotalDuration(totalDuration);
    ui->slider_SongProgress->setMaximum(totalDuration);
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

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia) {
        QMediaPlayer* M_Player = m_audioControl->getMediaPlayer();
        qint64 totalDuration = M_Player->duration() / 1000;

        if (M_Player->mediaStatus() != QMediaPlayer::NoMedia) {
            // Media loaded successfully
            QUrl mediaUrl = M_Player->source();
            QFileInfo fileInfo(mediaUrl.toLocalFile());
            QString fileNameWithoutExtension = fileInfo.fileName();
            fileNameWithoutExtension = fileNameWithoutExtension.left(fileNameWithoutExtension.lastIndexOf('.')); // Remove the file extension
            ui->label_fileName->setText(fileNameWithoutExtension);
            m_audioControl->setTotalDuration(totalDuration);
            updateDuration(0);
            disconnect(M_Player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);
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
        connect(M_Player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);
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

void MainWindow::loadSelectedSong(QListWidgetItem* song)
{
    Playlist* currentPlaylist = m_playlistManager->getCurrentPlaylist();
    currentPlaylist->selectSong(song, m_audioControl);
    QMediaPlayer* M_Player = m_audioControl->getMediaPlayer();
    connect(M_Player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);
    ui->toggleButton_PlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
}

void MainWindow::onSkipButtonClicked()
{
    Playlist* currentPlaylist = m_playlistManager->getCurrentPlaylist();
    if (currentPlaylist) {
        int currentIndex = ui->listWidget_SongsInPlaylist->currentRow();
        currentPlaylist->toNextSong(currentIndex, m_audioControl);
    }
}

void MainWindow::onPreviousButtonClicked()
{
    Playlist* currentPlaylist = m_playlistManager->getCurrentPlaylist();
    if (currentPlaylist) {
        int currentIndex = ui->listWidget_SongsInPlaylist->currentRow();
        currentPlaylist->toPreviousSong(currentIndex, m_audioControl);
    }
}
