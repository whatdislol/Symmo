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
#include "selectsongdialog.h"

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
    void addMultipleSongs();
    void removeSong(const QString& songPath);
    bool containsSong(const QString& songPath) const;
    QString getName() const;
    void setName(const QString& name);
    QList<QString> getSongPaths() const;
    QStringList getAllSongNames() const;

signals:
    void songAdded(QListWidgetItem* song);
    void songRemoved(const int& index);

private:
    QString getProjectRootPath() const;
    QString getCurrentSongPath(QMediaPlayer* m_player) const;
    QString m_musicLibraryPath;
    QList<QString> m_songPaths;
    QString m_name;
};

#endif // PLAYLIST_H
