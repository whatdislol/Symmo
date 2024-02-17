//playlistmanager.h

#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QObject>
#include <QInputDialog>
#include <QFileInfoList>
#include <QDir>
#include <QListWidgetItem>
#include "audiocontrol.h"

class PlaylistManager : public QObject
{
	Q_OBJECT
public:
    explicit PlaylistManager(QObject* parent = nullptr);
    ~PlaylistManager();

    void addPlaylist();
    void viewAllSongs();
    void selectSong(QListWidgetItem* song, AudioControl* audioControl);
    QString getTrackQuantity() const;
    void toNextSong();
    void toPreviousSong();
    void skipOnSongEnd();

signals:
    void clearSongList();
    void addSongToPlaylist(QListWidgetItem* song);
    void setPlaylistName(const QString& name);
    void setTrackQuantity(const QString& quantity);
    void onSongStart(const QString& songName);

private:
    QString m_musicLibraryPath = "C:/Users/neung/OneDrive/Desktop/KMITL_Courses/Y1S2/OOP/Project/Symmo/music_library";
};

#endif // PLAYLISTMANAGER_H
