#include "playlistmanager.h"

PlaylistManager::PlaylistManager(QObject* parent)
    : QObject(parent),
    m_defaultPlaylist(new Playlist(this)),
    m_playlists(QList<Playlist*>()),
    m_selectedPlaylist(m_defaultPlaylist),
    m_activePlaylist(m_defaultPlaylist),
    m_songSelectionDialog(new SelectSongDialog(m_selectedPlaylist->getMusicLibraryPath()))
{
    m_watcher.addPath(m_selectedPlaylist->getMusicLibraryPath());
        
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, &PlaylistManager::onMusicLibraryChanged);
    connect(m_songSelectionDialog, &SelectSongDialog::accepted, this, &PlaylistManager::onAddMultipleSongs);
    connect(m_selectedPlaylist, &Playlist::songSelected, this, &PlaylistManager::setActivePlaylist);
    m_defaultPlaylist->setName("All Tracks");
}

PlaylistManager::~PlaylistManager()
{
    qDebug() << m_defaultPlaylist->getName() << m_selectedPlaylist->getName() << m_activePlaylist->getName();
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
    for (Playlist* pl : m_playlists) {
        if (pl->getName() == name) {
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
}

void PlaylistManager::displaySongSelectionDialog()
{
    m_songSelectionDialog->show();
    m_songSelectionDialog->displaySongs();
}

void PlaylistManager::onSelectSong(QListWidgetItem* song, AudioControl* audioControl)
{
    m_selectedPlaylist->selectSong(song, audioControl);
}

void PlaylistManager::onToNextSong(AudioControl* audioControl)
{
    if (m_activePlaylist != nullptr) {
		m_activePlaylist->toNextSong(audioControl);
        qDebug() << "Next song";
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
        m_activePlaylist->toPreviousSong(audioControl);
    }
    else {
        QMediaPlayer* player = audioControl->getMediaPlayer();
        player->setPosition(0);
        player->play();
    }
}

void PlaylistManager::onSkipOnSongEnd(AudioControl* audioControl, QMediaPlayer::MediaStatus status)
{
    if (m_activePlaylist != nullptr) {
        m_activePlaylist->skipOnSongEnd(audioControl, status);
    }
    else {
        QMediaPlayer* player = audioControl->getMediaPlayer();
        player->setPosition(0);
	}
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

void PlaylistManager::setPlaylists(QList<Playlist*> playlists)
{
    m_playlists = playlists;
}

QList<Playlist*> PlaylistManager::getPlaylists()
{
    return m_playlists;
}

void PlaylistManager::onMusicLibraryChanged(const QString& path)
{
    QStringList currentPaths = getMusicLibraryAbsolutePaths(path);

    for (Playlist* playlist : m_playlists) {
        QStringList songPaths = playlist->getSongPaths();
        for (const QString& songPath : songPaths) {
            if (!currentPaths.contains(songPath)) {
                qDebug() << "File removed:" << songPath;
                int index = songPaths.indexOf(songPath);
                playlist->removeSong(index);
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