// mainwindow.cpp

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_audioControl(new AudioControl(this)),
    m_playlistManager(new PlaylistManager(this)),
    m_dataPath(FilePath::getProjectRootPath() + "/data.json"),
    m_assetPath(FilePath::getProjectRootPath() + "/asset"),
    m_gif(new QMovie(m_assetPath + "/gifs/none.gif")),
    m_timer(new QTimer(this)),
    m_appStartup(false)
{
    ui->setupUi(this);
    Playlist* selectedPlaylist = m_playlistManager->getSelectedPlaylist();
    QMediaPlayer* player = m_audioControl->getMediaPlayer();

    ui->listWidget_SongsInPlaylist->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listWidget_Playlist->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->toggleButton_Shuffle->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->slider_SongProgress->setMouseTracking(true);

    // CONNECT UI SIGNALS TO METHODS
    // main window
    connect(ui->listWidget_SongsInPlaylist, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(ui->listWidget_Playlist, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(ui->toggleButton_Shuffle, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(ui->lineEdit_SearchBar, &QLineEdit::textChanged, this, &MainWindow::filterSearchResults);
    connect(ui->pushButton_ClearSearch, &QPushButton::clicked, ui->lineEdit_SearchBar, &QLineEdit::clear);
    connect(ui->slider_SongProgress, &QSlider::sliderMoved, this, &MainWindow::updateDuration);
    connect(ui->slider_SongProgress, &QSlider::actionTriggered, this, &MainWindow::setSongProgressValue);

    // audio control
    connect(ui->slider_SongVolume, &QSlider::sliderMoved, m_audioControl, &AudioControl::setVolume);
    connect(ui->slider_SongProgress, &QSlider::sliderReleased, this, &MainWindow::onSongProgressSliderReleased);
    connect(ui->toggleButton_PlayPause, &QPushButton::clicked, m_audioControl, &AudioControl::togglePlayPause);

    connect(ui->toggleButton_Mute, &QPushButton::clicked, m_audioControl, &AudioControl::toggleMute);
    connect(ui->comboBox_AmbienceBox, &QComboBox::currentIndexChanged, m_audioControl, &AudioControl::playAmbience);
    connect(ui->slider_AmbienceVolume, &QSlider::sliderMoved, m_audioControl, &AudioControl::setAmbienceVolume);

    // playlist manager
    connect(ui->pushButton_ViewAllSongs, &QPushButton::clicked, m_playlistManager, &PlaylistManager::updateDefaultPlaylist);
    connect(ui->listWidget_SongsInPlaylist, &QListWidget::itemClicked, [=](QListWidgetItem* song) { m_playlistManager->onSelectSong(song, m_audioControl); });
    connect(ui->pushButton_AddPlaylist, &QPushButton::clicked, this, &MainWindow::getNewPlaylistName);
    connect(ui->listWidget_Playlist, &QListWidget::itemClicked, m_playlistManager, &PlaylistManager::selectPlaylist);
    connect(ui->pushButton_AddSong, &QPushButton::clicked, m_playlistManager, &PlaylistManager::displaySongSelectionDialog);
    connect(ui->toggleButton_Shuffle, &QPushButton::clicked, m_playlistManager, &PlaylistManager::toggleShuffleStatus);
    connect(ui->toggleButton_Loop, &QPushButton::clicked, m_playlistManager, &PlaylistManager::toggleLoopStatus);

    // playlist
    connect(ui->pushButton_Skip, &QPushButton::clicked, [=]() { m_playlistManager->onToNextSong(m_audioControl); });
    connect(ui->pushButton_Back, &QPushButton::clicked, [=]() { m_playlistManager->onToPreviousSong(m_audioControl); });

    // CONNECT LOGIC SIGNALS TO METHODS
    // main window
    connect(m_timer, &QTimer::timeout, this, &MainWindow::scrollOverflownText);

    // audio control
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::setMaxDuration);
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::updateSongProgress);
    connect(player, &QMediaPlayer::playingChanged, this, &MainWindow::updatePlayPauseIcon);
    connect(m_audioControl->getAudioOutput(), &QAudioOutput::mutedChanged, this, &MainWindow::updateMuteIcon);
    connect(m_audioControl, &AudioControl::isZeroVolume, this, &MainWindow::updateMuteIcon);
    connect(m_audioControl, &AudioControl::gifUpdated, this, &MainWindow::changeGif);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);
    connect(player, &QMediaPlayer::playbackStateChanged, this, &MainWindow::updateGifState);

    // playlist manager
    connect(m_playlistManager, &PlaylistManager::songsDisplayCleared, ui->listWidget_SongsInPlaylist, &QListWidget::clear);
    connect(this, &MainWindow::playlistAdded, m_playlistManager, &PlaylistManager::addPlaylist);
    connect(m_playlistManager, &PlaylistManager::playlistDisplayUpdated, this, &MainWindow::updateOnPlaylistSelected);
    connect(m_playlistManager, &PlaylistManager::defaultPlaylistDisplayUpdated, this, &MainWindow::updatePlaylistInfo);
    connect(m_playlistManager, &PlaylistManager::songImportButtonHidden, ui->pushButton_AddSong, &QPushButton::hide);
    connect(m_playlistManager, &PlaylistManager::songImportButtonVisible, ui->pushButton_AddSong, &QPushButton::show);
    connect(m_playlistManager, &PlaylistManager::playlistRemoved, this, &MainWindow::removePlaylist);
    connect(m_playlistManager, &PlaylistManager::playlistAdded, this, &MainWindow::addPlaylistWidgetItem);
    connect(m_playlistManager, &PlaylistManager::updateShuffleStatus, this, &MainWindow::onShuffleStatusChanged);
    connect(m_playlistManager, &PlaylistManager::searchBarCleared, ui->lineEdit_SearchBar, &QLineEdit::clear);
    connect(m_playlistManager, &PlaylistManager::playlistRenamed, this, &MainWindow::updatePlaylistsDisplay);
    connect(m_playlistManager, &PlaylistManager::updateLoopStatus, this, &MainWindow::updateLoopIcon);

    // playlist
    connect(selectedPlaylist, &Playlist::songAdded, this, &MainWindow::addSongWidgetItem);

    setupUI();
    setupFonts();
    m_playlistManager->updateDefaultPlaylist();
    ui->slider_AmbienceVolume->setValue(20);
    changeGif();
    m_timer->setInterval(200);
    m_timer->start();
    loadFromJSON(m_dataPath);
    m_playlistManager->onMusicLibraryChanged(FilePath::getProjectRootPath() + "/music_library/");
}

MainWindow::~MainWindow()
{
    saveToJSON(m_dataPath);
    delete ui;
    delete m_audioControl;
    delete m_playlistManager;
    delete m_gif;
    delete m_timer;
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
        updateDuration(progress / 1000);
    }
}

void MainWindow::updateDuration(qint64 duration)
{
    qint64 totalDuration = m_audioControl->getTotalDuration();
    QMediaPlayer::MediaStatus status = m_audioControl->getMediaPlayer()->mediaStatus();
    duration = (status == QMediaPlayer::NoMedia) ? 0 : duration;
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
        QMediaPlayer::MediaStatus status = M_Player->mediaStatus();
        QMediaPlayer::PlaybackState state = M_Player->playbackState();

        if (status != QMediaPlayer::NoMedia) {
            QUrl mediaUrl = M_Player->source();
            QFileInfo fileInfo(mediaUrl.toLocalFile());
            QString fileNameWithoutExtension = fileInfo.fileName();
            fileNameWithoutExtension = fileNameWithoutExtension.left(fileNameWithoutExtension.lastIndexOf('.'));
            QString separator = "        ";
            ui->label_fileName->setText(fileNameWithoutExtension + separator);
            m_audioControl->setTotalDuration(totalDuration);
            updateDuration(0);
            if (!m_appStartup) {
				m_appStartup = true;
                return;
            }
			ui->toggleButton_PlayPause->setIcon(QIcon(m_assetPath + "/icons/pause.png"));
        }
    }
}

void MainWindow::updateNextSongName()
{
    QString separator = "        ";
    QString nextSongName = m_playlistManager->onGetNextSongName(m_audioControl);
    ui->label_nextFileName->setText(nextSongName + separator);
}

void MainWindow::updateMuteIcon(bool muted)
{
    QIcon volumeIcon = muted ? QIcon(m_assetPath + "/icons/volume_mute.png") : QIcon(m_assetPath + "/icons/volume.png");
    ui->toggleButton_Mute->setIcon(volumeIcon);
}

void MainWindow::updatePlayPauseIcon(bool playing)
{
    QIcon playPauseIcon = playing ? QIcon(m_assetPath + "/icons/pause.png") : QIcon(m_assetPath + "/icons/play.png");
    ui->toggleButton_PlayPause->setIcon(playPauseIcon);
}

void MainWindow::updateShuffleIcon(bool shuffled)
{
    QIcon shuffleIcon;
    if (shuffled) {
        int shuffleMode = m_playlistManager->getShuffleMode();
        if (shuffleMode == 0) {
            shuffleIcon = QIcon(m_assetPath + "/icons/shuffle_non_dupe.png");
        }
        else if (shuffleMode == 1) {
            shuffleIcon = QIcon(m_assetPath + "/icons/shuffle_random.png");
        }
    }
    else {
        shuffleIcon = QIcon(m_assetPath + "/icons/shuffle_off.png");
    }
    ui->toggleButton_Shuffle->setIcon(shuffleIcon);
}

void MainWindow::updateLoopIcon(bool looped)
{
    QIcon loopIcon = looped ? QIcon(m_assetPath + "/icons/repeat_on.png") : QIcon(m_assetPath + "/icons/repeat_off.png");
    ui->toggleButton_Loop->setIcon(loopIcon);
}

void MainWindow::setupUI()
{
    ui->toggleButton_PlayPause->setIcon(QIcon(m_assetPath + "/icons/play.png"));
    ui->toggleButton_Mute->setIcon(QIcon(m_assetPath + "/icons/volume.png"));
    ui->pushButton_Skip->setIcon(QIcon(m_assetPath + "/icons/next.png"));
    ui->pushButton_Back->setIcon(QIcon(m_assetPath + "/icons/back.png"));
    ui->toggleButton_Shuffle->setIcon(QIcon(m_assetPath + "/icons/shuffle_off.png"));
    ui->toggleButton_Loop->setIcon(QIcon(m_assetPath + "/icons/repeat_off.png"));
    ui->pushButton_Search->setIcon(QIcon(m_assetPath + "/icons/search.png"));
    ui->pushButton_AddPlaylist->setIcon(QIcon(m_assetPath + "/icons/add.png"));
    ui->pushButton_ViewAllSongs->setIcon(QIcon(m_assetPath + "/icons/music.png"));
    ui->pushButton_AddSong->setIcon(QIcon(m_assetPath + "/icons/add.png"));
    ui->label_Gif->setMovie(m_gif);
    m_gif->setScaledSize(ui->label_Gif->size());
    ui->lineEdit_SearchBar->setPlaceholderText("Search in Playlist");
    ui->listWidget_SongsInPlaylist->setFocusPolicy(Qt::NoFocus);
    ui->listWidget_Playlist->setFocusPolicy(Qt::NoFocus);
}

void MainWindow::setupFonts() const
{
    int fontId = QFontDatabase::addApplicationFont(m_assetPath + "/fonts/GothamBold.ttf");
    QString GothamBold = QFontDatabase::applicationFontFamilies(fontId).at(0);
    int fontId2 = QFontDatabase::addApplicationFont(m_assetPath + "/fonts/GothamBook.ttf");
    QString GothamBook = QFontDatabase::applicationFontFamilies(fontId2).at(0);
}

void MainWindow::addSongWidgetItem(QListWidgetItem* song)
{
    ui->listWidget_SongsInPlaylist->addItem(song);
}

void MainWindow::addPlaylistWidgetItem(QListWidgetItem* playlist)
{
    ui->listWidget_Playlist->addItem(playlist);
}

void MainWindow::getNewPlaylistName()
{
    bool ok;
    QString newPlaylistName = QInputDialog::getText(nullptr, "New Playlist", "Enter playlist name:", QLineEdit::Normal, "", &ok);
    if (!ok) {
        return;
    }
    m_playlistManager->addPlaylist(newPlaylistName);
}

void MainWindow::updatePlaylistsDisplay(const int& index, const QString& name)
{
    ui->listWidget_Playlist->item(index)->setText(name);
}

void MainWindow::updateSongsDisplay()
{
    ui->listWidget_SongsInPlaylist->clear();
    Playlist* selectedPlaylist = m_playlistManager->getSelectedPlaylist();
    if (selectedPlaylist) {
        QList<QString> m_songPaths = selectedPlaylist->getSongPaths();
        for (QString& songPath : m_songPaths) {
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

void MainWindow::updateOnPlaylistSelected()
{
    updateSongsDisplay();
    updatePlaylistInfo();
}

void MainWindow::changeGif()
{
    QString selectedText = ui->comboBox_AmbienceBox->currentText();
    QString gifPath = m_assetPath + "/gifs/" + selectedText.toLower() + ".gif";
    m_gif->stop();
    m_gif->setFileName(gifPath);
    m_gif->start();
    updateGifState(m_audioControl->getMediaPlayer()->playbackState());
}

void MainWindow::updateGifState(QMediaPlayer::PlaybackState state)
{
    if (state == QMediaPlayer::PlayingState) {
        m_gif->start();
    }
    else {
        m_gif->stop();
    }
}

void MainWindow::removePlaylist(const int& index)
{
    QListWidgetItem* item = ui->listWidget_Playlist->takeItem(index);
    delete item;
}

void MainWindow::scrollOverflownText()
{
    QString trimmedCurrentSongName = ui->label_fileName->text().trimmed();
    QFontMetrics currentSongMetrics(ui->label_fileName->font());
    int textWidth = currentSongMetrics.horizontalAdvance(trimmedCurrentSongName);
    int labelWidth = ui->label_fileName->width();
    if (textWidth > labelWidth) {
		QString displayedText = ui->label_fileName->text();
		displayedText = displayedText.sliced(1) + displayedText.at(0);
		ui->label_fileName->setText(displayedText);
	}

    QString trimmedNextSongName = ui->label_nextFileName->text().trimmed();
    QFontMetrics nextSongMetrics(ui->label_nextFileName->font());
    int nextTextPadding = 7;
    int nextTextWidth = nextSongMetrics.horizontalAdvance(trimmedNextSongName);
    int nextLabelWidth = ui->label_nextFileName->width() - nextTextPadding * 2;
    if (nextTextWidth > nextLabelWidth) {
        QString displayedText = ui->label_nextFileName->text();
        displayedText = displayedText.sliced(1) + displayedText.at(0);
        ui->label_nextFileName->setText(displayedText);
    }
}

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    updatePlaybackUI(status);
    m_playlistManager->onSkipOnSongEnd(m_audioControl);
    updateNextSongName();
}

void MainWindow::onSongProgressSliderReleased()
{
    int sliderPosition = ui->slider_SongProgress->sliderPosition();
    ui->slider_SongProgress->setSliderPosition(sliderPosition);
    m_audioControl->setPosition(sliderPosition);
}

void MainWindow::onShuffleStatusChanged(bool shuffled)
{
    updateShuffleIcon(shuffled);
    updateNextSongName();
}

void MainWindow::filterSearchResults(const QString& searchQuery)
{
    if (searchQuery.isEmpty()) {
        updateOnPlaylistSelected();
        return;
    }

    Playlist* selectedPlaylist = m_playlistManager->getSelectedPlaylist();
    if (selectedPlaylist) {
        QList<QString> songNames = selectedPlaylist->getSongNames();
        ui->listWidget_SongsInPlaylist->clear();
        for (QString& songName : songNames) {
            if (songName.toLower().contains(searchQuery.toLower())) {
                QListWidgetItem* songItem = new QListWidgetItem(songName);
                ui->listWidget_SongsInPlaylist->addItem(songItem);
            }
        }
    }
}

void MainWindow::setSongProgressValue()
{
	if (ui->slider_SongProgress->isSliderDown()) {
		return;
	}
	Qt::MouseButtons btns = QApplication::mouseButtons();
    QSlider* slider = ui->slider_SongProgress;
	QPoint localMousePos = slider->mapFromGlobal(QCursor::pos());
	bool clickOnSlider = (btns & Qt::LeftButton) &&
		(localMousePos.x() >= 0 && localMousePos.y() >= 0 &&
			localMousePos.x() < slider->size().width() &&
			localMousePos.y() < slider->size().height());
	if (clickOnSlider)
	{
		float posRatio = localMousePos.x() / static_cast<float>(slider->size().width());
		int sliderRange = slider->maximum() - slider->minimum();
		int sliderPosUnderMouse = slider->minimum() + sliderRange * posRatio;
		if (sliderPosUnderMouse != slider->value())
		{
			slider->setSliderPosition(sliderPosUnderMouse);
			if (slider == ui->slider_SongProgress) {
				m_audioControl->setPosition(sliderPosUnderMouse);
			}
			return;
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
        QAction* renamePlaylistAction = playlistMenu.addAction("Rename");
        connect(renamePlaylistAction, &QAction::triggered, [=]() {
            bool ok;
            QString newPlaylistName = QInputDialog::getText(nullptr, "Rename Playlist", "Enter new playlist name:", QLineEdit::Normal, "", &ok);
            if (!ok) {
                return;
            }
            m_playlistManager->renamePlaylist(playlistIndex, newPlaylistName);
            });
        QAction* removePlaylistAction = playlistMenu.addAction("Remove Playlist");
        connect(removePlaylistAction, &QAction::triggered, [=]() {
            m_playlistManager->removePlaylist(playlistIndex);
            });
        playlistMenu.exec(globalPlaylistPos);
    }
    else if (ui->listWidget_SongsInPlaylist->underMouse() && m_playlistManager->getSelectedPlaylist() != m_playlistManager->getDefaultPlaylist()) {
        if (!songItem) {
            return;
        }
        QMenu songsMenu;
        QAction* removeSongAction = songsMenu.addAction("Remove from Playlist");
        connect(removeSongAction, &QAction::triggered, [=]() {
            QString songPath = FilePath::getProjectRootPath() + "/music_library/" + songItem->text() + ".mp3";
            selectedPlaylist->removeSong(songPath);
            updateOnPlaylistSelected();
            });
        songsMenu.exec(globalSongsPos);
    }
    else if (ui->toggleButton_Shuffle->underMouse()) {
        QMenu shuffleMenu;
        QAction* shuffleFisherYates = shuffleMenu.addAction("Non-Duplicate Shuffle");
        connect(shuffleFisherYates, &QAction::triggered, [=]() {
            m_playlistManager->setShuffleMode(0);
            m_playlistManager->shufflePlaylist();
            });
        QAction* shuffleRandom = shuffleMenu.addAction("Random Shuffle");
        connect(shuffleRandom, &QAction::triggered, [=]() {
            m_playlistManager->setShuffleMode(1);
            m_playlistManager->shufflePlaylist();
            });
        shuffleMenu.exec(globalShufflePos);
    }
}

void MainWindow::saveToJSON(const QString& filePath)
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

	QJsonObject positionObject;
	positionObject["position"] = ui->slider_SongVolume->value();

    QJsonObject activePlaylist;
    activePlaylist["name"] = m_playlistManager->getActivePlaylist()->getName();

    QJsonObject currentSong;
    currentSong["currentSong"] = FilePath::getCurrentSongPath(m_audioControl->getMediaPlayer());

    QJsonObject shuffleMode;
    shuffleMode["shuffleMode"] = m_playlistManager->getShuffleMode();

    QJsonObject shuffleStatus;
    shuffleStatus["shuffleStatus"] = m_playlistManager->getShuffleStatus();

    QJsonObject loopStatus;
    loopStatus["loopStatus"] = m_playlistManager->getLoopStatus();

	QJsonObject rootObject;
	rootObject["playlists"] = playlistsArray;
	rootObject["position"] = positionObject;
    rootObject["activePlaylist"] = activePlaylist;
    rootObject["currentSong"] = currentSong;
    rootObject["shuffleMode"] = shuffleMode;
    rootObject["shuffleStatus"] = shuffleStatus;
    rootObject["loopStatus"] = loopStatus;

	QJsonDocument jsonDoc(rootObject);

	QDir().mkpath(QFileInfo(filePath).path());

	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly)) {
		qDebug() << "Failed to open file for writing:" << file.errorString();
		return;
	}
	file.write(jsonDoc.toJson());
	file.close();
}


void MainWindow::loadFromJSON(const QString& filePath)
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

    // Load playlists
    QJsonArray playlistsArray = doc["playlists"].toArray();
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

    // Load position
    int position = 50;
    if (doc.isObject() && doc.object().contains("position")) {
        position = doc["position"].toObject()["position"].toInt();
    }
    ui->slider_SongVolume->setSliderPosition(position);
    m_audioControl->setVolume(position);

	// Load active playlist
	if (doc.isObject() && doc.object().contains("activePlaylist")) {
		QJsonObject activePlaylistObject = doc["activePlaylist"].toObject();
		QString activePlaylistName = activePlaylistObject["name"].toString();
		Playlist* activePlaylist = m_playlistManager->findPlaylistByName(activePlaylistName);
		if (activePlaylist) {
			m_playlistManager->setActivePlaylist(activePlaylist);
            m_playlistManager->setSelectedPlaylist(activePlaylist);
		}
	}

	// Load current song
	if (doc.isObject() && doc.object().contains("currentSong")) {
		QJsonObject currentSongObject = doc["currentSong"].toObject();
		QString currentSongPath = currentSongObject["currentSong"].toString();
        m_playlistManager->onSelectSong(currentSongPath, m_audioControl);
        m_audioControl->getMediaPlayer()->pause();
        ui->toggleButton_PlayPause->setIcon(QIcon(m_assetPath + "/icons/play.png"));
	}

    // Load shuffle mode
    if (doc.isObject() && doc.object().contains("shuffleMode")) {
		QJsonObject shuffleModeObject = doc["shuffleMode"].toObject();
		int shuffleMode = shuffleModeObject["shuffleMode"].toInt();
		m_playlistManager->setShuffleMode(shuffleMode);
	}   

    // Load shuffle status
    if (doc.isObject() && doc.object().contains("shuffleStatus")) {
        QJsonObject shuffleStatusObject = doc["shuffleStatus"].toObject();
        bool shuffleStatus = shuffleStatusObject["shuffleStatus"].toBool();
        if (shuffleStatus) {
			m_playlistManager->shufflePlaylist();
		}
    }

    // Load loop status
    if (doc.isObject() && doc.object().contains("loopStatus")) {
		QJsonObject loopStatusObject = doc["loopStatus"].toObject();
		bool loopStatus = loopStatusObject["loopStatus"].toBool();
        if (loopStatus) {
			m_playlistManager->toggleLoopStatus();
		}
	}

    updateOnPlaylistSelected();
    file.close();
}