// mainwindow.cpp

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_audioControl(new AudioControl(this)),
    m_playlistManager(new PlaylistManager(this)),
    m_dataPath(getProjectRootPath() + "/data.json")
{
    ui->setupUi(this);
    Playlist* selectedPlaylist = m_playlistManager->getSelectedPlaylist();
    QMediaPlayer* player = m_audioControl->getMediaPlayer();

    // CONNECT UI SIGNALS TO METHODS
    // main window
	ui->listWidget_SongsInPlaylist->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listWidget_Playlist->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->toggleButton_Shuffle->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->listWidget_SongsInPlaylist, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(ui->listWidget_Playlist, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(ui->toggleButton_Shuffle, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(ui->lineEdit_SearchBar, &QLineEdit::textChanged, this, &MainWindow::filterSearchResults);
    connect(ui->pushButton_ClearSearch, &QPushButton::clicked, ui->lineEdit_SearchBar, &QLineEdit::clear);

    // audio control
    connect(ui->slider_SongVolume, &QSlider::sliderMoved, m_audioControl, &AudioControl::setVolume);
    connect(ui->slider_SongProgress, &QSlider::sliderMoved, m_audioControl, &AudioControl::setPosition);
    connect(ui->toggleButton_PlayPause, &QPushButton::clicked, m_audioControl, &AudioControl::togglePlayPause);
    connect(ui->toggleButton_Mute, &QPushButton::clicked, m_audioControl, &AudioControl::toggleMute);

    // playlist manager
    connect(ui->pushButton_ViewAllSongs, &QPushButton::clicked, m_playlistManager, &PlaylistManager::updateDefaultPlaylist);
    connect(ui->listWidget_SongsInPlaylist, &QListWidget::itemClicked, [=](QListWidgetItem* song) {
        m_playlistManager->onSelectSong(song, m_audioControl);
    });
    connect(ui->pushButton_AddPlaylist, &QPushButton::clicked, this, &MainWindow::getNewPlaylistName);
    connect(ui->listWidget_Playlist, &QListWidget::itemClicked, m_playlistManager, &PlaylistManager::selectPlaylist);
    connect(ui->pushButton_AddSong, &QPushButton::clicked, m_playlistManager, &PlaylistManager::displaySongSelectionDialog);
    connect(ui->toggleButton_Shuffle, &QPushButton::clicked, m_playlistManager, &PlaylistManager::toggleShuffleStatus);

    // playlist
    connect(ui->pushButton_Skip, &QPushButton::clicked, [=]() {
        m_playlistManager->onToNextSong(m_audioControl);
    });
    connect(ui->pushButton_Back, &QPushButton::clicked, [=]() {
        m_playlistManager->onToPreviousSong(m_audioControl);
    });

    // CONNECT LOGIC SIGNALS TO METHODS
    // audio control
    connect(m_audioControl->getMediaPlayer(), &QMediaPlayer::durationChanged, this, &MainWindow::setMaxDuration);
    connect(m_audioControl->getMediaPlayer(), &QMediaPlayer::positionChanged, this, &MainWindow::updateSongProgress);
    connect(m_audioControl->getMediaPlayer(), &QMediaPlayer::playingChanged, this, &MainWindow::updatePlayPauseIcon);
    connect(m_audioControl->getAudioOutput(), &QAudioOutput::mutedChanged, this, &MainWindow::updateMuteIcon);
    connect(m_audioControl, &AudioControl::isZeroVolume, this, &MainWindow::updateMuteIcon);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::updatePlaybackUI);
    connect(player, &QMediaPlayer::mediaStatusChanged, [=](QMediaPlayer::MediaStatus status) {
        m_playlistManager->onSkipOnSongEnd(m_audioControl, status);
    });

    // playlist manager
    connect(m_playlistManager, &PlaylistManager::songsDisplayCleared, ui->listWidget_SongsInPlaylist, &QListWidget::clear);
    connect(this, &MainWindow::playlistAdded, m_playlistManager, &PlaylistManager::addPlaylist);
    connect(m_playlistManager, &PlaylistManager::playlistDisplayUpdated, this, &MainWindow::updatePlaylistDisplay);
    connect(m_playlistManager, &PlaylistManager::defaultPlaylistDisplayUpdated, this, &MainWindow::updatePlaylistInfo);
    connect(m_playlistManager, &PlaylistManager::songImportButtonHidden, ui->pushButton_AddSong, &QPushButton::hide);
    connect(m_playlistManager, &PlaylistManager::songImportButtonVisible, ui->pushButton_AddSong, &QPushButton::show);
    connect(m_playlistManager, &PlaylistManager::playlistRemoved, this, &MainWindow::removePlaylist);
    connect(m_playlistManager, &PlaylistManager::playlistAdded, this, &MainWindow::addPlaylistWidgetItem);
    connect(m_playlistManager, &PlaylistManager::updateShuffleStatus, this, &MainWindow::updateShuffleIcon);
    connect(m_playlistManager, &PlaylistManager::searchBarCleared, ui->lineEdit_SearchBar, &QLineEdit::clear);

    // playlist
    connect(selectedPlaylist, &Playlist::songAdded, this, &MainWindow::addSongWidgetItem);

    setupIcons();
    m_playlistManager->updateDefaultPlaylist();
    loadFromJSON(m_dataPath);
    m_playlistManager->onMusicLibraryChanged(m_playlistManager->getMusicLibraryPath());
}

MainWindow::~MainWindow()
{
    saveToJSON(m_dataPath);
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

void MainWindow::updateMuteIcon(bool muted)
{
    QIcon volumeIcon = muted ? style()->standardIcon(QStyle::SP_MediaVolumeMuted) : style()->standardIcon(QStyle::SP_MediaVolume);
    ui->toggleButton_Mute->setIcon(volumeIcon);
}

void MainWindow::updatePlayPauseIcon(bool playing)
{
    QIcon playPauseIcon = playing ? style()->standardIcon(QStyle::SP_MediaPause) : style()->standardIcon(QStyle::SP_MediaPlay);
    ui->toggleButton_PlayPause->setIcon(playPauseIcon);
}

void MainWindow::updateShuffleIcon(bool shuffled)
{
	QIcon shuffleIcon = shuffled ? style()->standardIcon(QStyle::SP_MediaSeekForward) : style()->standardIcon(QStyle::SP_MediaSeekBackward);
	ui->toggleButton_Shuffle->setIcon(shuffleIcon);
}

void MainWindow::setupIcons()
{
    ui->toggleButton_PlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->toggleButton_Mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    ui->pushButton_Skip->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui->pushButton_Back->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
}

void MainWindow::addSongWidgetItem(QListWidgetItem* song)
{
    ui->listWidget_SongsInPlaylist->addItem(song);
    qDebug() << "Song added to playlist: " << song->text();
}

void MainWindow::addPlaylistWidgetItem(QListWidgetItem* playlist)
{
	ui->listWidget_Playlist->addItem(playlist);
}

void MainWindow::getNewPlaylistName()
{
	QString newPlaylistName = QInputDialog::getText(this, tr("Add Playlist"), tr("Enter the name of the new playlist:"));

    if (!newPlaylistName.isEmpty() && newPlaylistName.length() <= 16) {
        m_playlistManager->addPlaylist(newPlaylistName);
    }
    else {
		QMessageBox::warning(this, "Invalid Playlist Name", "Playlist name must be between 1 and 16 characters.");
	}
}

void MainWindow::updateSongsDisplay()
{
    ui->listWidget_SongsInPlaylist->clear();
    Playlist* selectedPlaylist = m_playlistManager->getSelectedPlaylist();
    if (selectedPlaylist) {
        QList<QString> m_songPaths = selectedPlaylist->getSongPaths();
        for (QString songPath : m_songPaths) {
            QFileInfo fileInfo(songPath);
            QString baseName = fileInfo.baseName();
            QListWidgetItem* musicItem = new QListWidgetItem(baseName);
            ui->listWidget_SongsInPlaylist->addItem(musicItem);
        }
    }
}

void MainWindow::updatePlaylistInfo()
{
    Playlist* selectedPlaylist = m_playlistManager->getSelectedPlaylist();
    if (selectedPlaylist) {
        ui->label_PlaylistName->setText(selectedPlaylist->getName());
        ui->label_TrackQuantity->setText(selectedPlaylist->getTrackQuantity());
    }
}

void MainWindow::updatePlaylistDisplay()
{
    updateSongsDisplay();
    updatePlaylistInfo();
}

void MainWindow::removePlaylist(const int& index)
{
	QListWidgetItem* item = ui->listWidget_Playlist->takeItem(index);
	delete item;
}

void MainWindow::filterSearchResults(const QString& searchQuery)
{
    if (searchQuery.isEmpty()) {
		updatePlaylistDisplay();
		return;
	}

	Playlist* selectedPlaylist = m_playlistManager->getSelectedPlaylist();
    if (selectedPlaylist) {
		QList<QString> songNames = selectedPlaylist->getSongNames();
		ui->listWidget_SongsInPlaylist->clear();
        for (QString& songName : songNames) {
            if (songName.contains(searchQuery)) {
				QListWidgetItem* songItem = new QListWidgetItem(songName);
				ui->listWidget_SongsInPlaylist->addItem(songItem);
			}
		}
	}
}

void MainWindow::showContextMenu(const QPoint& pos)
{
	QListWidgetItem* playlistItem = ui->listWidget_Playlist->itemAt(pos);
    QListWidgetItem* songItem = ui->listWidget_SongsInPlaylist->itemAt(pos);
    QPoint globalPlaylistPos = ui->listWidget_Playlist->mapToGlobal(pos);
	QPoint globalSongsPos = ui->listWidget_SongsInPlaylist->mapToGlobal(pos);
    QPoint globalShufflePos = ui->toggleButton_Shuffle->mapToGlobal(pos);
	Playlist* selectedPlaylist = m_playlistManager->getSelectedPlaylist();


    if (ui->listWidget_Playlist->underMouse()) {
        if (!playlistItem) {
            return;
        }
        int playlistIndex = ui->listWidget_Playlist->row(playlistItem);
        QMenu playlistMenu;
		QAction* removePlaylistAction = playlistMenu.addAction("Remove Playlist");
		connect(removePlaylistAction, &QAction::triggered, [=]() {
			m_playlistManager->removePlaylist(playlistIndex);
			});
        playlistMenu.exec(globalPlaylistPos);
    } else if (ui->listWidget_SongsInPlaylist->underMouse() && m_playlistManager->getSelectedPlaylist() != m_playlistManager->getDefaultPlaylist()) {
        if (!songItem) {
            return;
        }
        int songIndex = ui->listWidget_SongsInPlaylist->row(songItem);
        QMenu songsMenu;
		QAction* removeSongAction = songsMenu.addAction("Remove from Playlist");
		connect(removeSongAction, &QAction::triggered, [=]() {
            QString songPath = m_playlistManager->getMusicLibraryPath() + songItem->text() + ".mp3";
			selectedPlaylist->removeSong(songPath);
			updatePlaylistDisplay();
			});
        songsMenu.exec(globalSongsPos);
    }
    else if (ui->toggleButton_Shuffle->underMouse()) {
        QMenu shuffleMenu;
        QAction* shuffleFisherYates = shuffleMenu.addAction("Fisher-Yates");
        connect(shuffleFisherYates, &QAction::triggered, [=]() {
            m_playlistManager->setShuffleMode(0);
            m_playlistManager->shufflePlaylist();
            // update shuffle icon for FISHER YATES shuffle
        });
        QAction* shuffleRandom = shuffleMenu.addAction("Random");
		connect(shuffleRandom, &QAction::triggered, [=]() {
			m_playlistManager->setShuffleMode(1);
			m_playlistManager->shufflePlaylist();
			// update shuffle icon for shuffle THREE
		});
        shuffleMenu.exec(globalShufflePos);
    }
}

void MainWindow::saveToJSON(const QString &filePath)
{
    QJsonArray playlistsArray;

    for (Playlist* playlist : m_playlistManager->getPlaylists()) {
        if (playlist == m_playlistManager->getDefaultPlaylist()) {
			continue;
		}
        QJsonObject playlistObject;
        playlistObject["name"] = playlist->getName();

        QJsonArray songsArray;
        for (const QString& song : playlist->getSongPaths()) {
            songsArray.append(song);
        }
        playlistObject["songs"] = songsArray;

        playlistsArray.append(playlistObject);
    }

	QDir().mkpath(QFileInfo(filePath).path());

	QJsonDocument jsonDoc(playlistsArray);
	if (jsonDoc.isNull() || jsonDoc.isEmpty()) {
		qDebug() << "Failed to serialize playlists data to JSON";
		return;
	}

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open file for writing:" << file.errorString();
        return;
    }
    file.write(QJsonDocument(playlistsArray).toJson());
    file.close();
}

void MainWindow::loadFromJSON(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file for reading:" << file.errorString();
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Failed to parse JSON:" << error.errorString();
        return;
    }

    QJsonArray playlistsArray = doc.array();
    QList<Playlist*> loadedPlaylists;
    for (const QJsonValue& playlistValue : playlistsArray) {
        QJsonObject playlistObject = playlistValue.toObject();
        QString playlistName = playlistObject["name"].toString();
        Playlist* playlist = new Playlist(this);
        playlist->setName(playlistName);

        QJsonArray songsArray = playlistObject["songs"].toArray();
        for (const QJsonValue& songValue : songsArray) {
            playlist->addSong(songValue.toString());
        }

        loadedPlaylists.append(playlist);
        addPlaylistWidgetItem(new QListWidgetItem(playlistName));
    }
    m_playlistManager->setPlaylists(loadedPlaylists);

    file.close();
}

QString MainWindow::getProjectRootPath() const
{
    QString executablePath = QCoreApplication::applicationDirPath();
    QDir currentDir(executablePath);
    while (!currentDir.exists("CMakeLists.txt") && currentDir.cdUp());

    return currentDir.absolutePath();
}
