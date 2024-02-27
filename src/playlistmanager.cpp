#include "playlistmanager.h"

PlaylistManager::PlaylistManager(QObject* parent)
    : QObject(parent),
    m_defaultPlaylist(new Playlist(this)),
    m_playlists(new QList<Playlist*>()),
    m_currentPlaylist(m_defaultPlaylist),
    m_songSelectionDialog(new SelectSongDialog(m_currentPlaylist->getMusicLibraryPath()))
{
    connect(m_songSelectionDialog, &SelectSongDialog::accepted, this, &PlaylistManager::onAddMultipleSongs);
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
    emit songsDisplayCleared();
    m_defaultPlaylist->addAllSongs();
    m_currentPlaylist = m_defaultPlaylist;
    emit defaultPlaylistDisplayUpdated();
    emit songImportButtonHidden();
}

void PlaylistManager::addPlaylist(QString name)
{
    Playlist* playlist = new Playlist(this);
    playlist->setName(name);
    m_playlists->append(playlist);
}

void PlaylistManager::removePlaylist(const int& index)
{
    delete m_playlists->at(index);
	m_playlists->removeAt(index);
	emit playlistRemoved(index);
}

Playlist* PlaylistManager::getCurrentPlaylist() const
{
    return m_currentPlaylist;
}

Playlist* PlaylistManager::getDefaultPlaylist() const
{
    return m_defaultPlaylist;
}

void PlaylistManager::selectPlaylist(QListWidgetItem* playlist)
{
    QString playlistName = playlist->text();
    for (Playlist* pl : *m_playlists) {
        if (pl->getName() == playlistName) {
            m_currentPlaylist = pl; // Set current playlist 
            break;
        }
    }
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
    m_currentPlaylist->selectSong(song, audioControl);
}

void PlaylistManager::onToNextSong(AudioControl* audioControl)
{
    m_currentPlaylist->toNextSong(audioControl);
}

void PlaylistManager::onToPreviousSong(AudioControl* audioControl)
{
    m_currentPlaylist->toPreviousSong(audioControl);
}

void PlaylistManager::onSkipOnSongEnd(AudioControl* audioControl, QMediaPlayer::MediaStatus status)
{
    m_currentPlaylist->skipOnSongEnd(audioControl, status);
}

void PlaylistManager::onAddMultipleSongs()
{
    QStringList selectedSongPaths = m_songSelectionDialog->getSelectedSongPaths();
    m_currentPlaylist->addMultipleSongs(selectedSongPaths);
    emit playlistDisplayUpdated();
}
