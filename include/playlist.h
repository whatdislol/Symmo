// playlist.h

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QObject>
#include <QInputDialog>
#include <QFileInfoList>
#include <QDir>
#include <QListWidgetItem>
#include <QCoreApplication>
#include "audiocontrol.h"

class Playlist : public QObject
{
    Q_OBJECT
public:
    explicit Playlist(QObject* parent = nullptr);
    ~Playlist();

    void loadAllSongs();
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

private:
    QString getProjectRootPath() const;
    QString m_musicLibraryPath;
};

#endif // PLAYLIST_H
