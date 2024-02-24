// playlistmanager.cpp

#include "playlistmanager.h"

PlaylistManager::PlaylistManager(QObject* parent) 
	: QObject(parent),
    m_defaultPlaylist(new Playlist(this)),
    m_currentPlaylist(m_defaultPlaylist),
    m_playlists(new QList<Playlist*>())
{
    m_defaultPlaylist->setName("All Tracks");
    updateDefaultPlaylist();
}

PlaylistManager::~PlaylistManager()
{

}

void PlaylistManager::updateDefaultPlaylist()
{
    emit songsDisplayCleared();
	m_defaultPlaylist->addAllSongs();
    m_currentPlaylist = m_defaultPlaylist;
	emit defaultPlaylistDisplayUpdated();
}

void PlaylistManager::addPlaylist(QString name)
{
    Playlist* playlist = new Playlist(this);
	playlist->setName(name);
	m_playlists->append(playlist);
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
    emit songsDisplayCleared();
    QString playlistName = playlist->text();
    for (Playlist* pl : *m_playlists) {
        if (pl->getName() == playlistName) {
			m_currentPlaylist = pl; // Set current playlist 
			break;
		}
	}
	emit playlistDisplayUpdated();
}
