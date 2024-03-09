#include "playlistmanager.h"

PlaylistManager::PlaylistManager(QObject* parent)
    : QObject(parent),
    m_defaultPlaylist(new Playlist(this)),
    m_playlists(QList<Playlist*>()),
    m_selectedPlaylist(m_defaultPlaylist),
    m_activePlaylist(m_defaultPlaylist),
    m_songSelectionDialog(new SelectSongDialog(m_selectedPlaylist->getMusicLibraryPath())),
    m_looped(false),
    m_shuffled(false),
    m_shuffleMode(0)
{
    m_watcher.addPath(m_selectedPlaylist->getMusicLibraryPath());
        
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, &PlaylistManager::onMusicLibraryChanged);
    connect(m_songSelectionDialog, &SelectSongDialog::accepted, this, &PlaylistManager::onAddMultipleSongs);
    connect(m_selectedPlaylist, &Playlist::songSelected, this, &PlaylistManager::setActivePlaylist);
    m_defaultPlaylist->setName("All Tracks");
}

PlaylistManager::~PlaylistManager()
{
    for (Playlist* playlist : m_playlists) {
        delete playlist;
    }
	m_playlists.clear();

    m_activePlaylist = nullptr;
    m_selectedPlaylist = nullptr;
    m_defaultPlaylist = nullptr;
    delete m_defaultPlaylist;

	delete m_songSelectionDialog;
}

void PlaylistManager::updateDefaultPlaylist()
{
    disconnect(m_selectedPlaylist, &Playlist::songSelected, this, &PlaylistManager::setActivePlaylist);
    emit songsDisplayCleared();
    m_defaultPlaylist->addAllSongs();
    m_selectedPlaylist = m_defaultPlaylist;
    emit defaultPlaylistDisplayUpdated();
    emit songImportButtonHidden();
    connect(m_selectedPlaylist, &Playlist::songSelected, this, &PlaylistManager::setActivePlaylist);
}

void PlaylistManager::addPlaylist(QString name)
{
	if (name.trimmed().isEmpty() || name.trimmed().length() > 16 || name == "All Tracks") {
		QMessageBox::warning(nullptr, "Warning", "Invalid playlist name. Please enter a name between 1 and 16 characters.");
		return;
	}
    for (Playlist* pl : m_playlists) {
        if (pl->getName() == name) {
            QMessageBox::warning(nullptr, "Warning", "Invalid playlist name. This name already exists.");
			return;
		}
	}
    Playlist* playlist = new Playlist(this);
	playlist->setName(name);
	m_playlists.append(playlist);

    QListWidgetItem* newPlaylist = new QListWidgetItem(name);
    emit playlistAdded(newPlaylist);
}

void PlaylistManager::removePlaylist(const int& index)
{
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(nullptr, "Confirmation", "Are you sure you want to remove this playlist?",
		QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::No) {
		return;
	}

    changePlaylistDisplayOnRemove(index);
    m_activePlaylist = nullptr;
    delete m_playlists.at(index);
    m_playlists.removeAt(index);
	emit playlistRemoved(index);
}

Playlist* PlaylistManager::getSelectedPlaylist() const
{
    return m_selectedPlaylist;
}

Playlist* PlaylistManager::getDefaultPlaylist() const
{
    return m_defaultPlaylist;
}

void PlaylistManager::selectPlaylist(QListWidgetItem* playlist)
{
    if (!m_selectedPlaylist) {
        return;
    }

    disconnect(m_selectedPlaylist, &Playlist::songSelected, this, &PlaylistManager::setActivePlaylist);
    QString playlistName = playlist->text();
    for (Playlist* pl : m_playlists) {
        if (pl->getName() == playlistName) {
            setSelectedPlaylist(pl);
            break;
        }
    }
    connect(m_selectedPlaylist, &Playlist::songSelected, this, &PlaylistManager::setActivePlaylist);
    emit playlistDisplayUpdated();
    emit songImportButtonVisible();
    emit searchBarCleared();
}

void PlaylistManager::displaySongSelectionDialog()
{
    m_songSelectionDialog->displaySongs();
    m_songSelectionDialog->exec();
}

void PlaylistManager::onSelectSong(QListWidgetItem* song, AudioControl* audioControl)
{
    m_selectedPlaylist->selectSong(song, audioControl);
    if (m_shuffled) {
		shufflePlaylist();
    }
}

void PlaylistManager::onToNextSong(AudioControl* audioControl)
{
    if (m_activePlaylist != nullptr) {
		m_activePlaylist->toNextSong(audioControl, m_shuffled);
    }
    else {
        QMediaPlayer* player = audioControl->getMediaPlayer();
        player->setPosition(0);
        player->play();
    }
}

void PlaylistManager::onToPreviousSong(AudioControl* audioControl)
{
    if (m_activePlaylist != nullptr) {
        m_activePlaylist->toPreviousSong(audioControl, m_shuffled);
    }
    else {
        QMediaPlayer* player = audioControl->getMediaPlayer();
        player->setPosition(0);
        player->play();
    }
}

void PlaylistManager::onSkipOnSongEnd(AudioControl* audioControl)
{
    if (m_activePlaylist != nullptr) {
        if (!m_looped) {
            m_activePlaylist->skipOnSongEnd(audioControl, m_shuffled);
        }
        else {
            loop(audioControl);
        }
    }
    else {
        QMediaPlayer* player = audioControl->getMediaPlayer();
        player->setPosition(0);
	}
}

QString PlaylistManager::onGetNextSongName(AudioControl* audioControl) const
{
    return m_activePlaylist->getNextSongName(audioControl, m_shuffled);
}

void PlaylistManager::onAddMultipleSongs()
{
    QStringList selectedSongPaths = m_songSelectionDialog->getSelectedSongPaths();
    m_selectedPlaylist->addMultipleSongs(selectedSongPaths);
    emit playlistDisplayUpdated();
}

void PlaylistManager::setSelectedPlaylist(Playlist* playlist)
{
	m_selectedPlaylist = playlist;
}

void PlaylistManager::setActivePlaylist(Playlist* playlist)
{
    m_activePlaylist = playlist;
}

void PlaylistManager::changePlaylistDisplayOnRemove(const int& index)
{
    if (m_selectedPlaylist == m_playlists.at(index)) {
		updateDefaultPlaylist();
	}
}

QString PlaylistManager::getMusicLibraryPath() const
{
    return m_defaultPlaylist->getMusicLibraryPath();
}

void PlaylistManager::setPlaylists(QList<Playlist*> playlists)
{
    m_playlists = playlists;
}

QList<Playlist*> PlaylistManager::getPlaylists()
{
    return m_playlists;
}

void PlaylistManager::renamePlaylist(const int& index, const QString& name)
{
	if (name.trimmed().isEmpty() || name.trimmed().length() > 16 || name == "All Tracks") {
		QMessageBox::warning(nullptr, "Warning", "Invalid playlist name. Please enter a name between 1 and 16 characters.");
		return;
	}
    for (Playlist* pl : m_playlists) {
        if (pl->getName() == name) {
			QMessageBox::warning(nullptr, "Warning", "Invalid playlist name. This name already exists.");
            return;
        }
    }
	m_playlists.at(index)->setName(name);
	emit playlistRenamed(index, name);
    emit playlistDisplayUpdated();
}

void PlaylistManager::onMusicLibraryChanged(const QString& path)
{
    QStringList currentPaths = getMusicLibraryAbsolutePaths(path);

    for (Playlist* playlist : m_playlists) {
        QStringList songPaths = playlist->getSongPaths();
        for (const QString& songPath : songPaths) {
            if (!currentPaths.contains(songPath)) {
                playlist->removeSong(songPath);
            }
        }
    }

    if (m_selectedPlaylist == m_defaultPlaylist) {
        updateDefaultPlaylist();
    }
    else {
		emit playlistDisplayUpdated();
	}
}

void PlaylistManager::setShuffleMode(const int& mode)
{
	m_shuffleMode = mode;
}

int PlaylistManager::getShuffleMode() const
{
    return m_shuffleMode;
}

void PlaylistManager::shufflePlaylist()
{
    switch (m_shuffleMode) {
		case 0:
			onShuffleFisherYates();
			break;
		case 1:
			onShuffleRandom();
			break;
        default:
			break;
	}
    m_shuffled = true;
    emit updateShuffleStatus(m_shuffled);
}

void PlaylistManager::toggleShuffleStatus()
{
    m_shuffled = !m_shuffled;
    if (m_shuffled) {
        shufflePlaylist();
	}
    emit updateShuffleStatus(m_shuffled);
}

void PlaylistManager::onShuffleFisherYates()
{
    m_activePlaylist->shuffleFisherYates();
}

void PlaylistManager::onShuffleRandom()
{
	m_activePlaylist->shuffleRandom();
}

void PlaylistManager::loop(AudioControl* audioControl) const
{
    QMediaPlayer::MediaStatus status = audioControl->getMediaPlayer()->mediaStatus();
    if (m_looped && status == QMediaPlayer::EndOfMedia) {
        QMediaPlayer* m_player = audioControl->getMediaPlayer();
        m_player->setPosition(0);
        m_player->play();
    }
}

void PlaylistManager::toggleLoopStatus()
{
	m_looped = !m_looped;
    emit updateLoopStatus(m_looped);
}

QStringList PlaylistManager::getMusicLibraryAbsolutePaths(const QString& path)
{
    QDir directory(path);
    QStringList absoluteFilePaths;
    QFileInfoList fileInfoList = directory.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    for (const QFileInfo& fileInfo : fileInfoList) {
        absoluteFilePaths.append(fileInfo.absoluteFilePath());
    }

    return absoluteFilePaths;
}