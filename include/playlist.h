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
    void toNextSong(AudioControl* audiocontrol);
    void toPreviousSong(AudioControl* audioControl);
    void skipOnSongEnd(AudioControl* audioControl, QMediaPlayer::MediaStatus status);
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
    QString getCurrentSongPath(QMediaPlayer* m_player) const;
    QString m_musicLibraryPath;
    QList<QString> m_songPaths;
    QString m_name;
};

#endif // PLAYLIST_H
