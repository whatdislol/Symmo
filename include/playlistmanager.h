//playlistmanager.h

#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QObject>
#include <QInputDialog>
#include <QFileInfoList>
#include <QDir>
#include <QListWidgetItem>

class PlaylistManager : public QObject
{
	Q_OBJECT
public:
    explicit PlaylistManager(QObject* parent = nullptr);
    ~PlaylistManager();

    void addPlaylist();
    void viewAllSongs();
    void selectSongs(QListWidgetItem* song);
    int getPlaylistSize(const QString& folderPath);
    void toNextSong();
    void toPreviousSong();
    void skipOnSongEnd();
};

#endif // PLAYLISTMANAGER_H
