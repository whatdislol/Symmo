#include "playlistmanager.h"

PlaylistManager::PlaylistManager(QObject* parent)
    : QObject(parent),
    m_defaultPlaylist(new Playlist(this)),
    m_playlists(new QList<Playlist*>()),
    m_selectedPlaylist(m_defaultPlaylist),
    m_activePlaylist(m_defaultPlaylist),
    m_songSelectionDialog(new SelectSongDialog(m_selectedPlaylist->getMusicLibraryPath()))
{
    connect(m_songSelectionDialog, &SelectSongDialog::accepted, this, &PlaylistManager::onAddMultipleSongs);
    connect(m_selectedPlaylist, &Playlist::songSelected, this, &PlaylistManager::setActivePlaylist);
    m_defaultPlaylist->setName("All Tracks");
}

PlaylistManager::~PlaylistManager()
{
	delete m_defaultPlaylist;
	delete m_playlists;
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
    for (Playlist* pl : *m_playlists) {
        if (pl->getName() == name) {
			return;
		}
	}
    Playlist* playlist = new Playlist(this);
	playlist->setName(name);
	m_playlists->append(playlist);

    QListWidgetItem* newPlaylist = new QListWidgetItem(name);
    emit playlistAdded(newPlaylist);
}

void PlaylistManager::removePlaylist(const int& index)
{
    delete m_playlists->at(index);
	m_playlists->removeAt(index);
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
    for (Playlist* pl : *m_playlists) {
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
    m_activePlaylist->toNextSong(audioControl);
}

void PlaylistManager::onToPreviousSong(AudioControl* audioControl)
{
    m_activePlaylist->toPreviousSong(audioControl);
}

void PlaylistManager::onSkipOnSongEnd(AudioControl* audioControl, QMediaPlayer::MediaStatus status)
{
    m_activePlaylist->skipOnSongEnd(audioControl, status);
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
