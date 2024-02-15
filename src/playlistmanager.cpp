#include "playlistmanager.h"

PlaylistManager::PlaylistManager(QObject* parent) 
	: QObject(parent) 
{

}

PlaylistManager::~PlaylistManager()
{

}

void PlaylistManager::addPlaylist()
{
	
}

QFileInfoList PlaylistManager::getAllSongsDirectory()
{
    //assuming the music library is in the same directory as the executable
    QString directory = "music_library/";

    QStringList musicFilters;
    musicFilters << "*.mp3" << "*.wav";

    QDir musicDir(directory);
    return musicDir.entryInfoList(musicFilters, QDir::Files);
}