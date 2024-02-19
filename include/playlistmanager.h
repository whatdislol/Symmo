//playlistmanager.h

#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QObject>
#include <QInputDialog>
#include <QFileInfoList>
#include <QDir>
#include <QListWidgetItem>
#include "audiocontrol.h"
#include "playlist.h"

class PlaylistManager : public QObject
{
	Q_OBJECT
public:
    explicit PlaylistManager(QObject* parent = nullptr);
    ~PlaylistManager();

    void addPlaylist();
    
    Playlist* getCurrentPlaylist() const;
    Playlist* getDefaultPlaylist() const;

signals:
    

private:
    QString m_musicLibraryPath = "C:/Users/neung/OneDrive/Desktop/KMITL_Courses/Y1S2/OOP/Project/Symmo/music_library/";
    Playlist* m_defaultPlaylist;
};

#endif // PLAYLISTMANAGER_H
