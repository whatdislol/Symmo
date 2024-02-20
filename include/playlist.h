// playlist.h

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QObject>
#include <QInputDialog>
#include <QFileInfoList>
#include <QDir>
#include <QListWidgetItem>
#include <QCoreApplication>
#include <QList>
#include "audiocontrol.h"

class Playlist : public QObject
{
    Q_OBJECT
public:
    explicit Playlist(QObject* parent = nullptr);
    ~Playlist();

    void addAllSongs();
    void selectSong(QListWidgetItem* song, AudioControl* audioControl) const;
    QString getTrackQuantity() const;
    void toNextSong();
    void toPreviousSong();
    void skipOnSongEnd();
    void addSong(const QString& songPath);
    void removeSong(const QString& songPath);
    bool containsSong(const QString& songPath) const;
    QString getName() const;
    void setName(const QString& name);
    void updatePlaylistInfo();

signals:
    void addSongToPlaylist(QListWidgetItem* song);
    void setPlaylistName(const QString& name);
    void setTrackQuantity(const QString& quantity);
    void songRemoved(const int& index);

private:
    QString getProjectRootPath() const;
    QString m_musicLibraryPath;
    QList<QString> m_songPaths;
    QString m_name;
    QString m_currentSongPath;
};

#endif // PLAYLIST_H
